#include "stm32f10x_gpio.h"

#include "adc.h"
#include "tft_display.h"
#include "linear_ccd.h"

unsigned short wb_threshold;

// unsigned short ccd_data[PIXELS];
unsigned char clk_state;

/*
 * Helper functions
 */
unsigned short get_averaged_data(unsigned char channel, unsigned short cnt) {
	// Note: I know the return type from get_adc() is u32,
	// but the resolution of this MPU is ~12bit, using u32 is just a waste.
	// You should modify the getter function in adc.c file.

	unsigned short tmp = 0;
	for(unsigned short i = 0; i < cnt; i++) {
		tmp += (unsigned short)get_adc(channel);
		tmp /= 2; // Note: Keil MDK should be able to optimize this.
	}

	return tmp;
}

void set_si(void) {
	GPIO_SetBits(SI1_PORT, SI1_PIN);
	GPIO_SetBits(SI2_PORT, SI2_PIN);
}

void clear_si(void) {
	GPIO_ResetBits(SI1_PORT, SI1_PIN);
	GPIO_ResetBits(SI2_PORT, SI2_PIN);
}

void toggle_clk(void) {
	if(clk_state)
		GPIO_ResetBits(CLK_PORT, CLK_PIN);
	else
		GPIO_SetBits(CLK_PORT, CLK_PIN);

	clk_state = ~clk_state;
}

void init_linear_ccd(void) {
	// Initialize the manual clock source.
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CLK_PORT, &GPIO_InitStructure);

	// Initialize the SI pins.
	// Note: Who arrange the SI pins on seperate buses? Nicht gut.
	GPIO_InitStructure.GPIO_Pin = SI1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SI2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SI2_PORT, &GPIO_InitStructure);

	// Rest the IOs.
	clk_state = 0;
	GPIO_ResetBits(CLK_PORT, CLK_PIN);
	GPIO_ResetBits(SI1_PORT, SI1_PIN);
	GPIO_ResetBits(SI2_PORT, SI2_PIN);

	wb_threshold = DEFAULT_TH;
}

void linear_ccd_read(unsigned short *buffer) {
	// Note: I need the specification of the sensor.
	// It seems like SI pull up and then down will tirgger the sensor to dump
	//  the data of all the pixels, I'm writing in this logic of yours.

	set_si();
	toggle_clk();
	// Note: Please check the spec for a more fine-grained delay value.
	// Does this sensor have busy wait status line?
	_delay_us(1);

	clear_si();
	toggle_clk();

	for(unsigned char i = 0; i < PIXELS; i++) {
		toggle_clk();

		// Save a bit to the buffer.
		buffer[i] = get_averaged_data(AO1_channel, AVERAGE_CNT);
		// Print it out on the screen.
		// Note: Don't your TFT screen has line write function?
		tft_put_pixel(i, buffer[i], GREEN);

		toggle_clk();
	}

	// Clear the screen.
	// Note: Don't the TFT library has page wipe mode?
	for(unsigned char y = 0; y < PIXELS; y++)
		tft_put_pixel(y, buffer[y], BLACK);
}

/*
 * Kadane's algorithm to find the maximum subarray.
 */

typedef struct {
	unsigned int sum;
	unsigned char lower;
	unsigned char upper;
} subarray;

inline void init_subarray(subarray *sa, unsigned char i) {
	sa->sum = 0;
	sa->lower = sa->upper = i;
}

inline void extend_subarray(subarray *sa, unsigned char i, unsigned int increase) {
	sa->sum += increase;
	sa->upper = i + 1;
}

unsigned int max_subarray(const unsigned char array[], unsigned char *lower, unsigned char *upper) {
	subarray max, tmp;
	init_subarray(&max, *lower);
	init_subarray(&tmp, *lower);

	for(int i = *lower; i < *upper; i++) {
		if(tmp.sum < 0) {
			init_subarray(&tmp, i);
		}
		extend_subarray(&tmp, i, array[i]);

		if(tmp.sum > max.sum) {
			max = tmp;
		}
	}
	*lower = max.lower;
	*upper = max.upper;

	return max.sum;
}

unsigned char find_center_pos(unsigned short *buffer) {
	unsigned char lower = 0, upper = PIXEL-1;
	max_subarray(buffer, &lower, &upper);
	return (lower+upper)/2;
}
