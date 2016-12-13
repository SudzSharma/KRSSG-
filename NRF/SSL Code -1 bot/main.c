#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_tim.h>

#include "misc.h"

#include "defines.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_nrf24l01.h"

#include "tm_stm32f4_gpio.c"
#include "tm_stm32f4_delay.c"
#include "tm_stm32f4_spi.c"
#include "tm_stm32f4_nrf24l01.c"

#define pld 26
#define chnl 5
#define TRUE 1
#define FALSE 0

volatile int velocity[5] = {0};
volatile int dribbler = 0;
volatile int bot_id;

int i=0;
int flag=0;
int BotData[6]={0},count=0,c=0,duty,pulse_l,temp,x;
int binaryNumber[8]={0};
int temp=0;
int j=8;

uint8_t MyAddress[] = {
 	0x7E,
 	0x7E,
 	0x7E,
 	0x7E,
 	0x7E

 };

uint8_t  Receive[32];

void init_kicker()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void GPIOLED_TEST()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void GPIOB_Motors()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_0 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void GPIOE_Motors()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7|GPIO_Pin_8| GPIO_Pin_9| GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void setFWDbit(int motorID, int value)
{
	switch (motorID){
		case 1:
			if(value==1){
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
			}else if(value==0){
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
			}break;
		case 2:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_7);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_7);
			}break;
		case 3:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_9);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_9);
			}break;
		case 4:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_11);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_11);
			}break;
		case 5:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_13);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_13);
			}break;
	}
}
void setBKDbit(int motorID, int value)
{
	switch (motorID){
		case 1:
			if(value==1){
				GPIO_SetBits(GPIOB, GPIO_Pin_1);
			}else if(value==0){
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			}break;
		case 2:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_8);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_8);
			}break;
		case 3:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_10);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_10);
			}break;
		case 4:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_12);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_12);
			}break;
		case 5:
			if(value==1){
				GPIO_SetBits(GPIOE, GPIO_Pin_14);
			}else if(value==0){
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			}break;
	}
}

void pwmToWheels_init()
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 127;
		TIM_TimeBaseStructure.TIM_Prescaler = 131;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
		/* PWM1 Mode configuration: Channel2 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
		/* PWM1 Mode configuration: Channel3 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
		/* PWM1 Mode configuration: Channel4 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM4, ENABLE);
		/* TIM4 enable counter */
		TIM_Cmd(TIM4, ENABLE);
}
void pwmToWheels_set(int channel, int duty_cycle)
{
	switch (channel){
		case 1: TIM4->CCR1 = duty_cycle; break;
		case 2: TIM4->CCR2 = duty_cycle; break;
		case 3: TIM4->CCR3 = duty_cycle; break;
		case 4: TIM4->CCR4 = duty_cycle; break;
	}
}
void pwmToDribbler_init()
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 255;
		TIM_TimeBaseStructure.TIM_Prescaler = 66;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
		/* PWM1 Mode configuration: Channel3 */
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 0;
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM2, ENABLE);
		/* TIM2 enable counter */
		TIM_Cmd(TIM2, ENABLE);
}
void pwmToDribbler_set(int duty_cycle)
{
	TIM2->CCR3 = duty_cycle;
}

void NRF_init()
{
	TM_NRF24L01_Init(18, 32);
		//TM_NRF24L01_SetCrcLength(NRF24L01_CRC_8);
		TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_250k, TM_NRF24L01_OutputPower_M18dBm); /* Set RF settings, Data rate to 2Mbps, Output power to -18dBm */
		TM_NRF24L01_SetMyAddress(MyAddress);/* Set my address, 5 bytes */
		 TM_NRF24L01_PowerUpRx();
}

void DataCheck()
{
	if(Receive[0] == 6)
		{
			BotData[1] = Receive[1];
			BotData[2] = Receive[2];
			BotData[3] = Receive[3];
			BotData[4] = Receive[4];
		}
}

int delay=0;

int main(void)
	{
	    init_kicker();
	    //init_botid();
		GPIOB_Motors();
		GPIOE_Motors();
		GPIOLED_TEST();
		pwmToWheels_init();
		//pwmToDribbler_init();
		//velocity_set();
		//GPIO_SetBits(GPIOE, GPIO_Pin_0);
		//GPIO_SetBits(GPIOE, GPIO_Pin_0);
		GPIO_SetBits(GPIOE, GPIO_Pin_1);
		GPIO_ResetBits(GPIOD,GPIO_Pin_0);
		//pwmToDribbler_set(200);
		//setBKDbit(5,0);
		//setFWDbit(5,1);
		NRF_init();

		//setFWDbit(1, 0);
		//setBKDbit(1, 1);
		//pwmToWheels_set(1,200);
		while(1)
		{
			if (TM_NRF24L01_DataReady()) {
				TM_NRF24L01_GetData(Receive);
				DataCheck();
				TM_NRF24L01_PowerUpRx();

			count=0;

			/*while(temp!=0){
				binaryNumber[--j]=temp%2;
				temp=temp/2;
				if(binaryNumber[0]==1){

					GPIO_SetBits(GPIOD,GPIO_Pin_0);
					GPIO_SetBits(GPIOE,GPIO_Pin_0);
					for(delay=0;delay<10000;delay+=0.01){}
					GPIO_ResetBits(GPIOE,GPIO_Pin_0);
					GPIO_ResetBits(GPIOD,GPIO_Pin_0);
				}
			}*/
			temp=BotData[5];

			if(temp==128){
				GPIO_SetBits(GPIOD,GPIO_Pin_0);
				GPIO_SetBits(GPIOE,GPIO_Pin_1);
				for(delay=0;delay<10000;delay+=1){}
				GPIO_ResetBits(GPIOE,GPIO_Pin_1);
				GPIO_ResetBits(GPIOD,GPIO_Pin_0);
			}



			if(BotData[2]<=127||BotData[2]==1){
			//ccw init pe0+ve and pe1 gnd
			//x=((BotData[x]*(127-26))/127)+13
				if(BotData[2]==0){
					BotData[2]+=5;
				}
				if(BotData[2]<=14){
					setFWDbit(2, 0);
					setBKDbit(2, 1);
					pwmToWheels_set(2,14);
				}
				if(BotData[2]>=113){
					setFWDbit(2, 0);
					setBKDbit(2, 1);
					pwmToWheels_set(2,113);
				}
				else{
					x=BotData[2];
					setFWDbit(2, 0);
					setBKDbit(2, 1);
					pwmToWheels_set(2,x);
				}
			}
			else{
			if(BotData[2]==255){
				BotData[2]=250;
			}
			BotData[2]=255-BotData[2];
				if(BotData[2]<=14){
					setFWDbit(2, 1);
					setBKDbit(2, 0);
					pwmToWheels_set(2,14);
				}
				if(BotData[2]>=113){
					setFWDbit(2, 1);
					setBKDbit(2, 0);
					pwmToWheels_set(2,113);
				}
				else{
					x=BotData[2];
					setFWDbit(2, 1);
					setBKDbit(2, 0);
					pwmToWheels_set(2,x);
				}
			}

			if(BotData[1]<=127||BotData[1]==0){
						//ccw init pe0+ve and pe1 gnd
						//x=((BotData[x]*(127-26))/127)+13

							if(BotData[1]==0){
							  BotData[1]+=5;
								//setFWDbit(1, 0);
								//setBKDbit(1, 1);
								//pwmToWheels_set(1,14);
							}
							if(BotData[1]<=14){
								setFWDbit(1, 0);
								setBKDbit(1, 1);
								pwmToWheels_set(1,14);
							}
							if(BotData[1]>=113){
								setFWDbit(1, 0);
								setBKDbit(1, 1);
								pwmToWheels_set(1,113);
							}
							else{
								x=BotData[1];
								setFWDbit(1, 0);
								setBKDbit(1, 1);
								pwmToWheels_set(1,x);
							}
						}
						else{
						if(BotData[1]==255){
							BotData[1]=250;
						}
					    BotData[1]=255-BotData[1];
							if(BotData[1]<=14){
								setFWDbit(1, 1);
								setBKDbit(1, 0);
								pwmToWheels_set(1,14);
							}
							if(BotData[1]>=113){
								setFWDbit(1, 1);
								setBKDbit(1, 0);
								pwmToWheels_set(1,113);
							}
							else{
								x=BotData[1];
								setFWDbit(1, 1);
								setBKDbit(1, 0);
								pwmToWheels_set(1,x);
							}
					    }

			if(BotData[3]<=127||BotData[3]==0){
			//ccw init pe0+ve and pe1 gnd
			//x=((BotData[x]*(127-26))/127)+13
				if(BotData[3]==0){
					BotData[3]+=5;
				}
				if(BotData[3]<=14){
					setFWDbit(3, 0);
					setBKDbit(3, 1);
					pwmToWheels_set(3,14);
				}
				if(BotData[3]>=113){
					setFWDbit(3, 0);
					setBKDbit(3, 1);
					pwmToWheels_set(3,113);
				}
				else{
					x=BotData[3];
					setFWDbit(3, 0);
					setBKDbit(3, 1);
					pwmToWheels_set(3,x);
				}
			}
			else{
			if(BotData[3]==255){
				BotData[3]=250;
			}
			BotData[3]=255-BotData[3];
				if(BotData[3]<=14){
					setFWDbit(3, 1);
					setBKDbit(3, 0);
					pwmToWheels_set(3,14);
				}
				if(BotData[3]>=113){
					setFWDbit(3, 1);
					setBKDbit(3, 0);
					pwmToWheels_set(3,113);
				}
				else{
					x=BotData[3];
					setFWDbit(3, 1);
					setBKDbit(3, 0);
					pwmToWheels_set(3,x);
				}
			}

			if(BotData[4]<=127||BotData[4]==0){
			//ccw init pe0+ve and pe1 gnd
			//x=((BotData[x]*(127-26))/127)+13
				if(BotData[4]==0){
					BotData[4]+=5;
				}
				if(BotData[4]<=14){
					setFWDbit(4, 0);
					setBKDbit(4, 1);
					pwmToWheels_set(4,14);
				}
				if(BotData[4]>=113){
					setFWDbit(4, 0);
					setBKDbit(4, 1);
					pwmToWheels_set(4,113);
				}
				else{
					x=BotData[4];
					setFWDbit(4, 0);
					setBKDbit(4, 1);
					pwmToWheels_set(4,x);
				}
			}
			else{
			if(BotData[4]==255){
				BotData[4]=250;
			}
			BotData[4]=255-BotData[4];
				if(BotData[4]<=14){
					setFWDbit(4, 1);
					setBKDbit(4, 0);
					pwmToWheels_set(4,14);
				}
				if(BotData[4]>=113){
					setFWDbit(4, 1);
					setBKDbit(4, 0);
					pwmToWheels_set(4,113);
				}
				else{
					x=BotData[4];
					setFWDbit(4, 1);
					setBKDbit(4, 0);
					pwmToWheels_set(4,x);
				}
			}
		}
	}
}
