#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_usart.h"

#include "defines.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_nrf24l01.h"

#include "tm_stm32f4_gpio.c"
#include "tm_stm32f4_disco.c"
#include "tm_stm32f4_delay.c"
#include "tm_stm32f4_spi.c"
#include "tm_stm32f4_nrf24l01.c"

// /* Receiver address */
// 	uint8_t TxAddress[] = {
// 		0xE7,
// 		0xE7,
// 		0xE7,
// 		0xE7,
// 		0xE7
// 	};
	/* My address */
	uint8_t MyAddress[] = {
		0x7E,
		0x7E,
		0x7E,
		0x7E,
		0x7E
	};

uint8_t dataInUSART[32];
uint8_t dataOut[32]={0}; /* Data received and data for send */
int i=0;
int time=0,time1=0,time2=0,time3=0;
int USART_flag = 1;
int count=0;

TM_NRF24L01_Transmit_Status_t transmissionStatus;  /* NRF transmission status */

void USART_pins_initialise()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//GPIO clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef y;
	y.GPIO_Pin = GPIO_Pin_7;  //X pin
	y.GPIO_Mode = GPIO_Mode_AF;
	y.GPIO_OType = GPIO_OType_PP;
	y.GPIO_PuPd = GPIO_PuPd_UP;
	y.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&y);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
}

void USART_initialise()
{
	USART_InitTypeDef z;
	z.USART_WordLength = USART_WordLength_8b;
	z.USART_BaudRate = 9600;
	z.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	z.USART_Mode = USART_Mode_Rx;
	z.USART_StopBits = USART_StopBits_1;
	z.USART_Parity = USART_Parity_No;
	USART_Init(USART1, &z);
}

void NVIC_initialise()
{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = USART1_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}

void TransmissionStatus()  //NRF
{
	if (transmissionStatus == TM_NRF24L01_Transmit_Status_Ok)
				{
					/* Transmit went OK */
					//TM_USART_Puts(USART1, ": OK\n");
					TM_DISCO_LedOn(LED_BLUE);
					//for(i=0;i<20000;i++); //This is a delay
					TM_DISCO_LedOff(LED_BLUE);
				}
	else if (transmissionStatus == TM_NRF24L01_Transmit_Status_Lost)
				{
					/* Message was LOST */
					//TM_USART_Puts(USART1, ": LOST\n");
					TM_DISCO_LedOn(LED_RED);
				//	for(i=0;i<200000;i++); //This is a delay
					TM_DISCO_LedOff(LED_RED);
				}
	else
				{
					/* This should never happen */
					//TM_USART_Puts(USART1, ": SENDING\n");
					TM_DISCO_LedOn(LED_ORANGE);
					TM_DISCO_LedOff(LED_ORANGE);
				}
}

void LED_initialise()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef x;
	x.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	x.GPIO_Mode = GPIO_Mode_OUT;
	x.GPIO_Speed = GPIO_Speed_50MHz;
	x.GPIO_OType = GPIO_OType_PP;
	x.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&x);
}


void Data()
{
//			dataOut[0] = 32; //check bit
//			dataOut[1] = 100; //velocity 1
//			dataOut[2] = 100; //velocity 2
//			dataOut[3] = 100; //velocity 3
//			dataOut[4] = 100; //velocity 4
//			dataOut[5] = 100;

			for(i=0;i<32;i++)
			{
				dataOut[i] = 8;
			}
}

void ssl_LED_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //Initialisation of clock
	  GPIO_InitTypeDef y;     //Defining a structure named 'x'
		y.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
		y.GPIO_Mode = GPIO_Mode_OUT;
		y.GPIO_Speed = GPIO_Speed_50MHz;
		y.GPIO_OType = GPIO_OType_PP;
		y.GPIO_PuPd = GPIO_PuPd_NOPULL;
		//GPIO_StructInit(x);   //why do we add this satetment? This refers to the same statements as given  above

		GPIO_Init(GPIOE, &y);
}



void USART1_IRQHandler()
{
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		USART_flag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		dataInUSART[count] = USART_ReceiveData(USART1);
		count++;
}


int main(void) {

	//LED_initialise();
	ssl_LED_init();
	USART_pins_initialise();
	USART_initialise();
	NVIC_initialise();
	USART_Cmd(USART1, ENABLE); //usart enable
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_flag=0;

	//NRF FUNCTIONS
	TM_DISCO_LedInit();
	//Funtion for setting sending data
	/* Initialize NRF24L01+ on channel 1 and 32bytes of payload */
	/* By default 2Mbps data rate and 0dBm output power */
	/* NRF24L01 goes to RX mode by default */
	TM_NRF24L01_Init(18, 32);
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_250k, TM_NRF24L01_OutputPower_M18dBm); /* Set 2MBps data rate and -18dBm output power */
	TM_NRF24L01_SetMyAddress(MyAddress); /* Set my address, 5 bytes */
	//TM_NRF24L01_SetTxAddress(TxAddress); /* Set TX address, 5 bytes */

	TM_DELAY_Init(); //initialise delay

	Data();
	while (1) {

		if(USART_flag == 1){

			TM_DELAY_SetTime(0); //start counter

			TM_DISCO_LedOn(LED_RED);
			USART_flag = 0;

			if(count > 5)
			{
				count=0;
				for(i=0;i<32;i++){
			dataOut[i] = dataInUSART[i];}

				TM_NRF24L01_Transmit(dataOut); /* Transmit data, goes automatically to TX mode */

				TM_DISCO_LedOn(LED_GREEN); /* Turn on led to indicate sending */
				GPIO_SetBits(GPIOE, GPIO_Pin_1);

				for(i=0;i<10000;i++){};

				TM_DISCO_LedOff(LED_GREEN); /* Turn off led */
				GPIO_ResetBits(GPIOE, GPIO_Pin_1);
			//TM_NRF24L01_PowerUpRx(); /* Go back to RX mode */
	while (TM_NRF24L01_DataReady() && TM_DELAY_Time() < 100);
			/* Wait received data, wait max 100ms, if time is larger, then data were probably lost */
			/* Get data from NRF2L01+ */
			//TM_NRF24L01_GetData(dataIn);*
			//TransmissionStatus(); // Check transmission status and flash corresponding LED
				//for(i=0;i<10000;i++){};
				//Delayms(100);
			}
		}
	}
}
