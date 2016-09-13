#include <stdlib.h>
#include <string.h>
#include "global.h"

RGB_t leds[NUM_LEDS];

void delayMs(uint32_t msec)  {
 uint32_t tmp = 7000 * msec  ;
 while( tmp-- ) __NOP();
}

inline void delayUs(uint32_t usec)  {
 uint32_t tmp = 7 * usec  ;
 while( tmp-- ) __NOP();
}

void initRandomByTempSensor() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_1Cycles5);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);

//	ADC_ResetCalibration(ADC1);
//	while(ADC_GetResetCalibrationStatus(ADC1));
//	ADC_StartCalibration(ADC1);
//	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	unsigned val = ADC_GetConversionValue(ADC1);
	srand(val);
#ifdef RS322_DEBUG_ON
        	USART_DBG_printf("\srand: %d",  val);
#endif
}

//=========================================

typedef void (*EFFECT_FUNC_TYPE)();

EFFECT_FUNC_TYPE effects_list[] = {
	effectBlastPoints,
	effectRainbow,
    effectWave,
    effectMoveSpot,
    effectRunningLights,
#ifdef HAS_SOUND_FFT_SRC
    effectColourMusic,
#endif
    effectHeartRhythm,
    effect2Wave,
    effectFlame,
};

#define EFFECTS_LIST_SZ ((int)(sizeof(effects_list)/sizeof(effects_list[0])))

#ifdef HAS_SOUND_LEVEL_SENSOR
static inline uint8_t colorUP(uint8_t v) {
 uint16_t r = ((uint16_t)v * 3 + (uint16_t)v) / 2;
 if(r > 255) return 255;
 return r;
}
volatile bool has_loud_sound = FALSE;
#endif

int main() {
	memset((void*)leds, 0, sizeof(leds));
	SystemStartup();
	ws2812b_Init();
#ifdef HAS_USOUND_SRC
	usoundInit();
#endif
	USART_init();
	initRandomByTempSensor();
#ifdef HAS_SOUND_FFT_SRC
	soundFFT_init();
#endif

#ifdef HAS_SOUND_LEVEL_SENSOR
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
		GPIO_EventOutputConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

		EXTI_InitTypeDef EXTI_InitStructure;
		EXTI_InitStructure.EXTI_Line = EXTI_Line4;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
#endif
	uint32_t t = _sysTicks;
	int mode = -1;
#ifdef HAS_USOUND_SRC
	uint16_t prev_usoundRange = usoundGetRange();
#else
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = PIR_SENSOR_PIN;
		GPIO_Init(PIR_SENSOR_PORT, &GPIO_InitStructure);
	}
#endif

    while (1) {
#ifdef HAS_USOUND_SRC
        if(usoundGetRange() < 5) {
         	mode = -1;
        }
#endif

        if(ws2812b_IsReady()) {

        	if(mode >= 0 && mode < EFFECTS_LIST_SZ) {
        		(effects_list[mode])();
        	} else {
            	memset((void*)leds, 0, sizeof(leds));
        	}
#ifdef HAS_SOUND_LEVEL_SENSOR
        	if(has_loud_sound) {
        		for(int i = 0; i < NUM_LEDS; i++) {
        			leds[i].r = colorUP(leds[i].r);
        			leds[i].g = colorUP(leds[i].g);
        			leds[i].b = colorUP(leds[i].b);
        		}
        	}
        	has_loud_sound = FALSE;
#endif
        	ws2812b_SendRGB(leds, NUM_LEDS);
        }

        if((_sysTicks - t) > 1000) {
        	t = _sysTicks;
#ifdef HAS_USOUND_SRC
        	uint16_t cur_usoundRange = usoundGetRange();
        	if((prev_usoundRange-cur_usoundRange) > 50 && cur_usoundRange > 15) {
        		mode = rand() % EFFECTS_LIST_SZ;
        	}
        	prev_usoundRange = usoundGetRange();
#ifdef RS322_DEBUG_ON
        	USART_DBG_printf("\nl: %d",  cur_usoundRange);
#endif
#else // PIR Motion sensor
        	static uint8_t pir_cnt_sec_with_move = 0;
        	static uint8_t pir_cnt_sec_without_move = 0;
        	if((PIR_SENSOR_PORT->IDR & PIR_SENSOR_PIN) != 0) {
#ifdef RS322_DEBUG_ON
        		USART_DBG_printf("\ns on: cnt1:%d cnt2:%d m:%d", pir_cnt_sec_with_move, pir_cnt_sec_without_move, mode);
#endif
        		if(pir_cnt_sec_with_move == 0 || pir_cnt_sec_with_move > 6) {
        			mode = rand() % EFFECTS_LIST_SZ;
        			pir_cnt_sec_with_move = 0;
        		}
           		pir_cnt_sec_with_move++;
           		pir_cnt_sec_without_move = 0;
        	} else {
#ifdef RS322_DEBUG_ON
        		USART_DBG_printf("\ns off: cnt1:%d cnt2:%d m:%d", pir_cnt_sec_with_move, pir_cnt_sec_without_move, mode);
#endif
        		if(pir_cnt_sec_without_move > 15) {
        			mode = -1;
            		pir_cnt_sec_without_move = 0;
        		}
        		pir_cnt_sec_without_move++;
        		pir_cnt_sec_with_move = 0;
        	}
#endif


#ifdef HAS_SOUND_FFT
#ifdef RS322_DEBUG_ON
        	USART_DBG_puts("\n---->\n");
        	uint16_t *res = soundFFT_result();
        	for(int i = 0; i < 64; i++) { USART_DBG_printf(" %4d", scan_result[i]); }
        	for(int i = 0; i < FFT_RESULT_SIZE; i++) { USART_DBG_printf(" %4d",  res[i]); }
#endif // RS322_DEBUG_ON
#endif // HAS_SOUND_FFT
        }
    }
}


