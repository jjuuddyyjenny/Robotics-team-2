#include "adc.h"
#include "stm32f10x.h"
#include "ticks.h"
#include "tft_display.h"

extern u32 linear_ccd_buffer1[128];
extern u32 linear_ccd_buffer2[128];

#define CLK_PORT GPIOA
#define CLK_PIN GPIO_Pin_8

#define SI1_PORT GPIOA
#define SI1_PIN GPIO_Pin_0

#define SI2_PORT GPIOB
#define SI2_PIN GPIO_Pin_0

#define AO1_channel 1
#define AO2_channel 2
extern int sortedArray[128];



void linear_ccd_init(void);
void linear_ccd_read(void);


//
#ifndef LINEAR_CCD_NEW_H
#define LINEAR_CCD_NEW_H

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
 typedef struct {
	unsigned int sum;
	unsigned char lower;
	unsigned char upper;
} subarray;
 
void init_linear_ccd(void);
void init_subarray(subarray *sa, unsigned char i);
// void read_linear_ccd(unsigned short *buffer);
void set_threshold(unsigned short threshold);
unsigned char find_center_pos(unsigned short *buffer);

#endif


