#include "main.h"
#include "uart.h"
#include "linear_ccd.h"
#include "pneumatic_control.h"
#include "motor.h"
#include "GPIO_switch.h"

int staten = 1; 
int speed = 100;
int motor_state = 0; 

void stop(){
	motor_control(1,1,0);
	motor_control(2,1,0);
}

while (true){
	countt+=1;
	LED_OFF(GPIOB, LED_M); 
	//temptime=get_ms_ticks();
	tft_prints(3,3,"%d",countt);
	tft_update();
    	uart_interrupt_init(COM3,&listener);
	byte = uart_get_byte();
	temptime = get_ms_ticks();
			
	switch (byte) {
		case 'w':
			//forward (one of the motor drivers had the direction reversed)
			countw+=1;
			tft_prints(1,1, "Forward %d",countw);
			motor_control(1, 1, 150);
			motor_control(2, 0, 150);
			tft_update();
			break;
		case 's':
			tft_prints(1,1, "Backward");
			motor_control(1, 0, 150);
			motor_control(2, 1, 150);
			tft_update();
			break;

		case 'a':
			tft_prints(1,1, "Left");
			tft_update();
			motor_control(1,1,0);
			motor_control(2, 0,100);
			break;

		case 'd':
			tft_prints(1,1, "Right");
			tft_update();
			motor_control(1,1,100);
			motor_control(2, 0,0);
			break;
			
		case NULL:
			stop();
			break;
		case 'o':
			counto+=1;
			
			if(counto%10==0){
				LED_ON(GPIOB, LED_M); 
				_delay_ms(500);
			}
			
			else{
				LED_OFF(GPIOB, LED_M); 
			}
			break;
		case 'u':
			uart_tx_byte(COM1, byte);
			break;
		case 'p':
			break;
			
		case '8': 
			//adjusting speed to 100
			speed = 100; 
			tft_prints(1, 1, "100"); 
			tft_update(); 
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
		
		case '9':
			//adjusting speed to 190
			speed = 190;
			tft_prints(1, 1, "190"); 
			tft_update(); 
			break; 
		case '1': 
			staten = 1;
			break; 
		case '2':
			staten = 2; 
			break; 
			
			
		default:
			tft_prints(1,1, "stop");
			temptime=get_ms_ticks();
			tft_update();
			break;
	}
		
		
		/*
		if(get_ms_ticks()- temptime>100){
			stop();
		}else{
		}
		*/
		
	afterw+=1;
	tft_prints(6,6,"afterw %d", afterw);
	tft_update();
	stop();
	}
}

 void listener(const uint8_t byte) {
	switch(byte) {


		case 'w':
			
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
	uart_init(COM3, 9600);
	uart_interrupt_init(COM3,&listener);
	int y; 
		
	while(1)
	{ 
		if (staten == 1) {
			motor_control(1, 1, 0); 
			motor_control(2, 0, 0); 
		}
		
		else if (staten == 2) {
			int count=get_ms_ticks();
			if (count%50==0)
			{
			 linear_ccd_read();		
			 linear_ccd_prints();
				int x = find_white_line();
			
					
			if(x>55 && x<62)
			{
				motor_state=1;
				if (y==motor_state){
				}
				else{
				
		
			tft_prints(3,10,"go straight la ");
			tft_update();
			motor_control(1,1,100);
			motor_control(2,0,100);
					y=1;
				}
			}
			 if(x<56)
			{
				motor_state=2;
				if(y==motor_state){
				}
				else{
			
				tft_prints(3,6,"turn left ");
				tft_update();
				motor_control(1,0,80);
				motor_control(2,0,120);
					y=2;
				}
			}
			 if(x>61)
			{
			 motor_state=3;
				if(y==motor_state){
				}
				else{
				tft_prints(3,6,"turn right ");
				tft_update();
				motor_control(1,1,120);
				motor_control(2,1,80);
					y=3;
				}
				
			}
		
			//tft_prints(4,4,"%d",find_white_line());
			linear_ccd_clear();
			tft_update(); 
		}
		if (checkendstage())
		{stop();}
			
			
		}
	}
}


	



/*

int staten= 0;



int main(){
	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init();
  	GPIO_switch_init();
  	pneumatic_init();
	tft_init(0,WHITE,BLACK,RED);
	uart_init(COM3, 9600);
	uart_init(COM1, 115200);
	LED_INIT(); 
	uint8_t byte;
	
	//stop();
	int countt = 0;
	int countw = 0;
	int counto=0;
	int afterw=0;
	int temptime;
	
	


	

 /*


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

 
 */
	
	
	


	
	
	




		

	

