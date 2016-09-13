#include "global.h"

#ifdef HAS_USOUND_SRC

static volatile uint16_t usound_range = 0xffff;
void usound_proc(void) {
	static volatile uint16_t startCnt;

//	USART_DBG_printf("\n[%d] it %d %x", _sysTicks, TIM2->CNT, TIM2->SR);

	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
		TIM_Cmd(TIM2, DISABLE);
		TIM_CCxCmd(TIM2, TIM_Channel_2, TIM_CCx_Disable);

		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Prescaler = 4299;  // 33500 cm/s + double way (2149*2)
		TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		// echo	-  PA2 (TIM2_CH3)
		TIM_ICInitTypeDef TIM_ICInitStructure;
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICFilter = 0xF;
		TIM_ICInit(TIM2, &TIM_ICInitStructure);

		TIM2->CNT = 0;
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3|TIM_IT_Update);
		TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
		startCnt = 0;
		TIM_Cmd(TIM2, ENABLE);
		TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	}
	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET) {
		if(startCnt == 0) {
			startCnt = TIM2->CCR3;
			TIM_ICInitTypeDef TIM_ICInitStructure;
			TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
			TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
			TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
			TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			TIM_ICInitStructure.TIM_ICFilter = 0xF;
			TIM_ICInit(TIM2, &TIM_ICInitStructure);
		} else {
			usound_range = TIM2->CCR3 - startCnt;
			TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);
			TIM_Cmd(TIM2, DISABLE);

			//USART_DBG_printf("\n[%d] l: %d", _sysTicks, usoundGetRange());
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	}

	if (TIM_GetFlagStatus(TIM2, TIM_FLAG_CC3OF) != RESET)  {
		usound_range = 0xffff;
		TIM_ClearFlag(TIM2, TIM_FLAG_CC3OF);
	}
}

uint16_t usoundGetRange(void) {
	return usound_range;
}

void usoundInit(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	 // strobe	-  PA1 (TIM2_CH2)
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIOA->BRR = GPIO_Pin_1;

	// echo	-  PA2 (TIM2_CH3)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void usoundGenPulse(void) {
	// strobe	-  PA1 (TIM2_CH2)
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 720; // 10us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM2->CNT = 0;

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1; // one pulse mode delay time CNT < CCRx < ARR
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update|TIM_IT_CC3| TIM_IT_CC1|TIM_IT_CC1|TIM_IT_CC4);
	TIM_ClearFlag(TIM2, TIM_FLAG_CC3OF);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	//USART_DBG_printf("\n[%d] usoundGenPulse", _sysTicks);
}

#endif // HAS_USOUND_SRC
