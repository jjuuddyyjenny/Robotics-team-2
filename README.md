# Robotics-team-2
#include "main.h"
#include "uart.h"
#include "linear_ccd.h"
#include "pneumatic_control.h"
#include "motor.h"
#include "GPIO_switch.h"

int main(){
	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init();
  GPIO_switch_init();
  pneumatic_init();
	tft_init(0,WHITE,BLACK,RED);
	uart_init(COM3, 115200);
	LED_INIT();
	uint8_t byte;
	while (true){
		LED_OFF(GPIOB, LED_M);
		tft_update();
    uart_interrupt_init(COM3,&listener);
		byte = uart_get_byte();
		switch (byte) {
			case 'u':
				tft_prints(1,1,"received! Congratulation!");
				tft_update();
			break;
			default:
				tft_prints(1,1, "Garbage Received.");
				tft_update();
			break;
		}
	}
}
