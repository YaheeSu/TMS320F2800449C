/*
 * Init.h
 *
 */

#ifndef INIT_H_
#define INIT_H_



extern void Init_GPIO(void);
extern void Init_PIE(void);
extern void Init_CpuTimer(void);
extern void Init_ePWM(unsigned int period, unsigned int PWM_duty);
void Init_ADC(void);
void Init_ADCSOC(void);
void Set_DeadBand(unsigned int deadtime);
//void Set_ePWM(unsigned int period, unsigned int PWM_duty);



#endif /* INIT_H_ */
