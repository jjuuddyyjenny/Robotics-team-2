#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

void motor_init(void);
void motor_control(u8 PWMx, u8 dir, u16 magnitude);

#endif
