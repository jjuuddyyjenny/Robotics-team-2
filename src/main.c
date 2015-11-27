#include "main.h"

/*
   w: forward
   a: left
   d: right
   s: backward
   m: software emergency button

   o: debug LED on/off

   1,2,3,4,5,6,7,8,9: speedsssssssss

   q : pneumatic flag raising (Pnaematic Pin5)
   pneumatic serving
   pneumatic flag lower
   pneumatic serving lower
   // use delay for this automatic raise flag - serving - lower flag - serving lower

   h, j, k, l : grippers pneumatic control (grabbing)

   g, v, b, ' ' : gripper slider
 */

int speed = 100;
int staten = 1;
int countt = 0;
int countw = 0;
int counto = 0;
int afterw = 0;
int temptime;
int motor_state = 1;
int y;

void initialize() {
	adc_init();
	ticks_init();
	linear_ccd_init();
	motor_init();
	GPIO_switch_init();
	pneumatic_init();
	tft_init(0,WHITE,BLACK,RED);

	uart_init(COM3, 9600);
	uart_init(COM1, 115200);
	uart_interrupt_init(COM3, &listener);

	LED_INIT();
}

int main() {
	unsigned char center_pos;
	unsigned short ccd_data[PIXELS];

	uint8_t byte;

	initialize();

	while(true) {
		countt+=1;
		LED_OFF(GPIOB, LED_M);
		tft_prints(3,3,"%d",countt);
		tft_update();

		byte = uart_get_byte();
		temptime = get_ms_ticks();

		switch(byte) {

		case '1':
			staten=1;
			break;
		case '2':
			staten=2;
			break;

		/* case 'n':
		   speed+=10;
		   if( speed >150){
		   speed=150;
		   }
		   tft_prints(1,1, "increase speed 10");
		   tft_prints(1,1, "speed now",speed);
		   tft_update();
		   break;

		   case 'b':
		   speed-=10;
		   if( speed <50){
		   speed=50;
		   }
		   tft_prints(1,1, "decrease speed 10");
		   tft_prints(1,1, "speed now",speed);
		   tft_update();
		   break;

		 */
		case '8':
			speed=100;
			tft_prints(1,1, "Speed %d",speed);
			tft_update();

		case '9':
			speed=190;

		case 'w':
			motor_control(1, 1, speed);
			motor_control(2, 0, speed);
			// tft_update();
			break;

		case 's':
			// tft_prints(1,1, "Backward");
			motor_control(1, 0, speed);
			motor_control(2, 1, speed);
			// tft_update();
			break;

		case 'a':
			// tft_prints(1,1, "Left");
			// tft_update();
			motor_control(1,0,speed);
			motor_control(2, 0,speed);

			break;

		case 'd':
			// tft_prints(1,1, "Right");
			// tft_update();
			motor_control(1,1,speed);
			motor_control(2, 1,speed);
			break;

		case 'm':
			tft_prints(1,1, "Emergency Stop");
			tft_update();
			motor_control(1,1,0);
			motor_control(2,1,0);
			break;

// debug LED on/off

		case 'o':
			counto+=1;
			if(counto%10==0) {
				LED_ON(GPIOB, LED_M);
				_delay_ms(500);
			} else {
				LED_OFF(GPIOB, LED_M);
			}
			break;

// pneumatic flag raising & serving	---- to slave mainboard

		case 'q':
			tft_clear();
			uart_tx_byte(COM1, byte);
			tft_prints(2,2,"serving/flag");
			tft_update();
			break;

// grippers pneumatic control(grabbing)	---- master mainboard

		case 'h':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_5, 1);
			tft_prints(2,2,"grab1");
			tft_update();
			break;

		case 'j':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_6, 1);
			tft_prints(2,2,"grab2");
			tft_update();
			break;

		case 'k':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_7, 1);
			tft_prints(2,2,"grab3");
			tft_update();
			break;

		case 'l':
			tft_clear();
			pneumatic_control(GPIOB, GPIO_Pin_8, 1);
			tft_prints(2,2,"grab4");
			tft_update();
			break;

// gripper slider(upDownLeftRight)	--- to slave mainboard

		case 'g':
			tft_clear();
			uart_tx_byte(COM1, byte);
			tft_prints(2,2,"gripper up");
			tft_update();
			break;

		case 'v':
			tft_clear();
			uart_tx_byte(COM1, byte);
			tft_prints(2,2,"gripper left");
			tft_update();
			break;

		case 'b':
			tft_clear();
			uart_tx_byte(COM1, byte);
			tft_prints(2,2,"gripper right");
			tft_update();
			break;

		case ' ':
			tft_clear();
			uart_tx_byte(COM1, byte);
			tft_prints(2,2,"gripper down");
			tft_update();
			break;

/* ----------------------------------------------------------- */

		default:
			tft_prints(1,1, "default");
			temptime=get_ms_ticks();
			tft_update();
			break;
			afterw+=1;
			tft_prints(6,6,"afterw %d", afterw);
			tft_update();
		}

		if(staten == 1) {

			motor_control(1, 1, 0);
			motor_control(2, 0, 0);
		} else if(staten == 2) {
			int count=get_ms_ticks();
			if(count%50==0) {
				linear_ccd_read(ccd_data);
				center_pos = find_center_pos(ccd_data);

				if(center_pos>55 && center_pos<62) {
					motor_state=1;
					if(y==motor_state) {
					} else {

						tft_prints(3,10,"go straight la ");
						tft_update();
						motor_control(1,1,90);

						motor_control(2,0,90);
						y=1;
					}
				}
				if(center_pos<51) {
					motor_state=2;
					if(y==motor_state) {
					} else {

						tft_prints(3,6,"turn left ");
						tft_update();
						motor_control(1,0,80);
						motor_control(2,0,130);
						y=2;
					}
				}
				if(center_pos>61) {
					motor_state=3;
					if(y==motor_state) {
					} else {
						tft_prints(3,6,"turn right ");
						tft_update();
						motor_control(1,1,130);
						motor_control(2,1,80);
						y=3;
					}

				}

				tft_prints(4,4,"%d", find_center_pos(ccd_data));
				// linear_ccd_clear();
				tft_update();
			}
		}
	}
}
