#ifndef SOUNDFFT_H_
#define SOUNDFFT_H_

#ifdef HAS_SOUND_FFT_SRC

#define FFT_SIZE 256

#define SOUND_IN_PIN GPIO_Pin_4
#define SOUND_IN_PORT GPIOA

#define SOUND_IN_ADC1_CHANEL ADC_Channel_4
#define FFT_RESULT_SIZE (FFT_SIZE/2)

#if FFT_SIZE == 1024
	#define FFT_CALC	cr4_fft_1024_stm32
#elif FFT_SIZE == 256
	#define FFT_CALC	cr4_fft_256_stm32
#elif FFT_SIZE == 64
	define FFT_CALC	cr4_fft_64_stm32
#endif

// 239.5+12.5 = 252 cycles.   72000/6/252 -> 47.6 Khz
//RCC_ADCCLKConfig(RCC_PCLK2_Div6);
#define SOUND_IN_SAMPLE_TIME ADC_SampleTime_239Cycles5

void soundFFT_init();
void soundFFT_startADC();

uint16_t* soundFFT_result();

extern volatile uint16_t scan_result[FFT_SIZE];

#endif // HAS_SOUND_FFT_SRC

#endif /* SOUNDFFT_H_ */
