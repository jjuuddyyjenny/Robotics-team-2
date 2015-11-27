#ifndef LINEAR_CCD_NEW_H
#define LINEAR_CCD_NEW_H

#include "stm32f10x_gpio.h"

/*
 * CCD related.
 */
#define PIXELS      128

#define CLK_PORT    GPIOA
#define CLK_PIN     GPIO_Pin_8

#define SI1_PORT    GPIOA
#define SI1_PIN     GPIO_Pin_0

#define SI2_PORT    GPIOB
#define SI2_PIN     GPIO_Pin_0

/*
 * ADC related
 */
#define AO1_channel 1
#define AO2_channel 2

#define AVERAGE_CNT 10

// Note: This value dictates the threshold between white and black.
extern unsigned short wb_threshold;
#define DEFAULT_TH  100 // Default value.

/*
 * Prototypes
 */
void linear_ccd_init(void);
void read_linear_ccd(unsigned short *buffer);
void set_threshold(unsigned short threshold);
unsigned char find_center_pos(unsigned short *buffer);

#endif
