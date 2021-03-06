#include "motor.h"
#include "linear_ccd.h"
void motor_init(){

    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	// Enable bus

	//DIRECTION:PB12 PB14  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;						// Push-Pull Output Alternate-function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_14;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	// PWM:PB13 PB15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// Push-Pull Output Alternate-function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_15;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	

	
	//-------------TimeBase Initialization-----------//
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			// counter will count up (from 0 to FFFF)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					//timer clock = dead-time and sampling clock 	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	//------------------------------//
	TIM_TimeBaseStructure.TIM_Prescaler = 71;												//clk=72M/(71+1)= Hz, interval=? 
	TIM_TimeBaseStructure.TIM_Period = 200;												//pulse cycle=x  xperiod for one up
	//------------------------------//366hz
	//40300for turn on  

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	//final freq= timer freq / prescaler / period
	
	// ------------OC Init Configuration------------//
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;   		// set "high" to be effective output
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	           		// produce output when counter < CCR
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		// Reset OC Idle state
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;	// Reset OC NIdle state
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	// this part enable the normal output
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // this part enable the Nstate
	//------------------------------//
	TIM_OCInitStructure.TIM_Pulse = 0;													// this part sets the initial CCR value that mean the pwm value
	//------------------------------//
	\
	// OC Init
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, ENABLE);

	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, ENABLE);
	
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);	
	TIM_SetCounter(TIM1,0);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

}


void motor_control(u8 PWMx,u8 dir,u16 magnitude){//dir can be 1 or 0  pwm can be 1 or 2

	if( PWMx == 1 ){
		if(dir==0){
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		TIM_SetCompare1(TIM1,magnitude);
		}
		else{
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		TIM_SetCompare1(TIM1,magnitude);
		}
	}
	
	else if(PWMx==2){
		if(dir==0){
		GPIO_SetBits(GPIOB,GPIO_Pin_14);
		TIM_SetCompare3(TIM1,magnitude);
		}
		else{
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		TIM_SetCompare3(TIM1,magnitude);
		}
	
	}
}
void start()
{ 
	
	 motor_control(1,0,100);
   motor_control(2,0,100);
	 _delay_ms(2000);
	 motor_control(1,0,0);
   motor_control(2,0,100);
	_delay_ms(2000);
	 motor_control(1,0,100);
   motor_control(2,0,100);
	 _delay_ms(4000);
	motor_control(1,0,0);
  motor_control(2,0,0);
	
}
void turnn(int x){
    if (x>0 && x<20){
        motor_control(1, 0, 0);  //left motor
        motor_control(2, 0, 100); //right motor
        tft_prints(5,5,"Turn left");
        tft_update();
    }else if(x>20 && x<40){
        motor_control(1, 0, 0);
        motor_control(2, 0, 75);
        tft_prints(5,5,"Turn left");
        tft_update();
    }else if(x>40 && x<54){
        motor_control(1, 0, 0);
        motor_control(2, 0, 70);
        tft_prints(5,5,"Turn left");
        tft_update();
    }
    if(x>74 && x<100){
        motor_control(1, 0, 100);   //left motor
        motor_control(2, 0, 0);
        tft_prints(5,5,"Turn right");
        tft_update();
    }else if(x<100 && x>120){
        motor_control(1,0, 75);
        motor_control(2,0, 0);
        tft_prints(5,5,"Turn right");
        tft_update();
    }else if (x>120){
        motor_control(1,0,70);
        motor_control(2,0, 0);
        tft_prints(5,5,"Turn right");
        tft_update();
    }
}

int checkendstage()
	{int max =sortedArray[1];
	 int min =sortedArray[126];
    if(min>100){
    if((max-min)<10){ 
		return 1 ;}
	 }
    else
    {return 0;}			
		
		
	}
	void stop()
	{
		motor_control(1,1,0);
    motor_control(2,0,0);
	}