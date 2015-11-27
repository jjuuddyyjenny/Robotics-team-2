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

int main() {

	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init();
	GPIO_switch_init();
	pneumatic_init();
	tft_init(0,WHITE,BLACK,RED);
	uart_init(COM3, 9600);
	uart_interrupt_init(COM3,&listener);
	int y;

	while(1) {

		if(staten == 1) {
			motor_control(1, 1, 0);
			motor_control(2, 0, 0);
		} else if(staten == 2)   {
			int count=get_ms_ticks();
			if(count%50==0) {
				// checkendstage();
				linear_ccd_read();
				linear_ccd_prints();
				int x = find_white_line();
				// int z =find_center_pos(linear_ccd_buffer1);

				if(x>55&& x<62) {
					motor_state=1;
					if(y==motor_state) {
					} else   {
						tft_prints(3,10,"go straight la ");
						tft_update();
						motor_control(1,1,100);
						motor_control(2,0,100);
						y=1;
					}
				} else if(x<30)   {
					motor_state=2;
					if(y==motor_state) {
					} else   {
						tft_prints(3,10,"go more left");
						tft_update();
						motor_control(1,0, 50);
						motor_control(2,0, 150);
						y=2;
					}
				} else if(x<56)  {
					motor_state=3;
					if(y==motor_state) {
					} else   {
						tft_prints(3,6,"turn left ");
						tft_update();
						motor_control(1,0,80);
						motor_control(2,0,120);
						y=3;
					}
				} else if(x>61&&x<85)  {
					motor_state=4;
					if(y==motor_state) {
					} else   {
						tft_prints(3,6,"turn right ");
						tft_update();
						motor_control(1,1,120);
						motor_control(2,1,80);
						y=4;
					}
				} else if(x>85)  {
					motor_state=5;
					if(y==motor_state) {
					} else   {
						tft_prints(3,6,"turn more right");
						tft_update();
						motor_control(1,1,150);
						motor_control(2,1,50);
					}
					y=5;
					/*if(x==0)
					   {tft_prints(3,6,"backward");
					   tft_update();
					   motor_control(1,0,80);
					   motor_control(2,1,80);
					   linear_ccd_read();
					   }*/

				}

				tft_prints(4,4,"%d",find_white_line());
				linear_ccd_clear();
				tft_update();
			}

		}
	}
}
