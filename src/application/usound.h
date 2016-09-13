#ifndef USOUND_H_
#define USOUND_H_

#ifdef HAS_USOUND_SRC // HC-SR04 Ultrasonic Range Sensor

// echo		PA2 (TIM2_CH3)
// strobe 	PA1 (TIM2_CH2)

void usound_proc(void);
void usoundInit(void);
uint16_t usoundGetRange(void);
void usoundGenPulse(void);

#endif //HAS_USOUND_SRC

#endif /* USOUND_H_ */
