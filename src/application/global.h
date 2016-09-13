/*
 * global.h
 *
 *  Created on: 14.04.2011
 *      Author: mm
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_rtc.h"

#include "hw_config.h"
#include "ws2812b.h"
#include "USART_io.h"
#include "effects.h"

extern volatile uint32_t _sysTicks;
void delayMs(uint32_t msec);
void delayUs(uint32_t usec);

#define NUM_LEDS    105
extern RGB_t leds[NUM_LEDS];

#define HAS_SOUND_LEVEL_SENSOR // LM393. Analog Sound Sensor Arduino (RKP-SS-LM393)
#ifndef HAS_SOUND_LEVEL_SENSOR
#define HAS_SOUND_FFT_SRC
#endif // HAS_SOUND_LEVEL_SENSOR

//#define RS322_DEBUG_ON

//#define HAS_USOUND_SRC // HC-SR04 Ultrasonic Range Sensor

// PA2 - 'PIR Motion sensor' vs 'HC-SR04 Ultrasonic Range Sensor'
#define PIR_SENSOR_PORT GPIOA
#define PIR_SENSOR_PIN GPIO_Pin_2

#include "usound.h"
#include "soundFFT.h"

#endif /* GLOBAL_H_ */
