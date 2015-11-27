#include "main.h"
#include "uart.h"
#include "linear_ccd.h"
#include "pneumatic_control.h"
#include "motor.h"
#include "GPIO_switch.h"

int staten = 1;
int speed = 100;
int motor_state = 0;
uint8_t byte;

void initialize() {
	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init();
	GPIO_switch_init();
	pneumatic_init();
	tft_init(0, WHITE, BLACK, RED);

	//uart_init(COM3, 9600);
	//uart_interrupt_init(COM3, &listener);
}

int main() {
	unsigned char center_pos;
	unsigned short ccd_data[PIXELS];

	int y;

	initialize();

	while(1) {
		if(staten == 1) {
			motor_control(1, 1, 0);
			motor_control(2, 0, 0);
		} else if(staten == 2) {
			int count=get_ms_ticks();
			if(count%50==0) {
				// checkendstage();
				linear_ccd_read(ccd_data);
				center_pos = find_center_pos(ccd_data);

				if(center_pos > 55 && center_pos < 62) {
					motor_state = 1;
					if(y == motor_state) {
					} else {
						tft_prints(3,10,"go straight la ");
						tft_update();
						motor_control(1,1,100);
						motor_control(2,0,100);
						y = 1;
					}
				} else if(center_pos < 30) {
					motor_state = 2;
					if(y == motor_state) {
					} else {
						tft_prints(3,10,"go more left");
						tft_update();
						motor_control(1,0, 50);
						motor_control(2,0, 150);
						y = 2;
					}
				} else if(center_pos < 56) {
					motor_state = 3;
					if(y == motor_state) {
					} else {
						tft_prints(3,6,"turn left ");
						tft_update();
						motor_control(1,0,80);
						motor_control(2,0,120);
						y = 3;
					}
				} else if(center_pos > 61 && center_pos < 85) {
					motor_state = 4;
					if(y == motor_state) {
					} else {
						tft_prints(3,6,"turn right ");
						tft_update();
						motor_control(1,1,120);
						motor_control(2,1,80);
						y = 4;
					}
				} else if(center_pos > 85) {
					motor_state = 5;
					if(y == motor_state) {
					} else {
						tft_prints(3,6,"turn more right");
						tft_update();
						motor_control(1,1,150);
						motor_control(2,1,50);
					}
					y = 5;
				}

				//tft_prints(4,4,"%d",find_white_line());
				//linear_ccd_clear();
				tft_update();
			}
		}
	}
}
