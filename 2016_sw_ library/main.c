#include "main.h"
#include "uart.h"
#include "linear_ccd.h"
#include "pneumatic_control.h"
#include "motor.h"
#include "GPIO_switch.h"


int speed = 100;
 void listener(const uint8_t byte) {
	switch(byte) {
		case '1': 
			//adjusting speed to 100
			speed = 100; 
			tft_prints(1, 1, "100"); 
			tft_update(); 
			break;
		case '2':
			//adjusting speed to 190
			speed = 190;
			tft_prints(1, 1, "190"); 
			tft_update(); 
			break; 
		case 'w':
			//forward (one of the motor drivers had the direction reversed)
			motor_control(1, 1, speed); 
			motor_control(2, 0, speed); 
			_delay_ms(1000); 
			break;
		case 'd':
			//right
			motor_control(1, 1, speed); 
			motor_control(2, 1, 0); 
			_delay_ms(1000);
			break;
		case 'a':
			//left
			motor_control(1, 1, 0); 
			motor_control(2, 0, speed); 
			_delay_ms(1000); 
			break;
		case 's':
			stop(); 
			break;
		case 'x': 
			//reverse
			motor_control(1, 0, speed); 
			motor_control(2, 1, speed); 
			_delay_ms(1000); 
			break;
		case 'g': 
			//swivel clockwise
			motor_control(1, 1, speed); 
			motor_control(2, 1, speed); 
			_delay_ms(1000); 
			break;
		case 'h':
			//swivel counter clockwise
			motor_control(1, 0, speed); 
			motor_control(2, 0, speed); 
			_delay_ms(1000); 
			break; 
		
		//grip and hit
		case 'r':
			checksignal();
			break;
		case 'm': 
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_8, 0); 
			tft_prints(2, 2, "let go"); 
			tft_update(); 
			break; 
		case 'n':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_8, 1); 
			tft_prints(2, 2, "left grab for 1 sec"); 
			tft_update(); 
			_delay_ms(1000); 
			break; 
		case 'f':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_5, 1); 
			tft_prints(2, 2, "Raise Flag"); 
			tft_update(); 
			//_delay_ms(1000); should we put a delay here? 
			break; 
		case 'v': 
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_5, 0); 
			tft_prints(2, 2, "Put Down"); 
			tft_update(); 
			break; 
    default:
			break; 
	}
}

int main()
	{

	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init(); 
  GPIO_switch_init();
  pneumatic_init();
	tft_init(0,WHITE,BLACK,RED);
	uart_init(COM3, 115200);
	uart_interrupt_init(COM3,&listener);
		
	while(1)
	{ 
	  int count=get_ms_ticks();
		if (count%50==0)
		{
	   linear_ccd_read();		
	   linear_ccd_prints();
			int x = find_white_line();
		if(x>50 && x<72)
		{
		
		tft_prints(3,10,"go straight la ");
		tft_update();
		motor_control(1,1,70);
		motor_control(2,0,70);
		}
		 if(x<51)
		{
			
		
		  tft_prints(3,6,"turn left ");
			tft_update();
			motor_control(1,1,0);
			motor_control(2,0,120);
		}
		 if(x>71)
		{
		 
		  tft_prints(3,6,"turn right ");
			tft_update();
			motor_control(1,1,120);
			motor_control(2,0,0);
			
		}
		
		 //tft_prints(4,4,"%d",find_white_line());
	  linear_ccd_clear();
		tft_update(); 
			
			
		}
	}
}


	




	

 
	
	
	


	
	
	




		

	

