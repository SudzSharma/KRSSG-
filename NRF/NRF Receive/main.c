//Can refer to SPI_USART.c if necessary

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_usart.h"

#include "defines.h"
#include "tm_stm32f4_gpio.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_nrf24l01.h"

#include "tm_stm32f4_gpio.c"
#include "tm_stm32f4_delay.c"
#include "tm_stm32f4_spi.c"
#include "tm_stm32f4_nrf24l01.c"

//USART is used to print data on screen. As USART is 8 bit. 32 bit input data from nrf is converted into
//4 parts

/* Receiver address */
// uint8_t TxAddress[] = {
// 	0xE7,
// 	0xE7,
// 	0xE7,
// 	0xE7,
// 	0xE7
// 		//0,0,0,0,0
// };
 // My address
uint8_t MyAddress[] = {
 	0x7E,
 	0x7E,
 	0x7E,
 	0x7E,
 	0x7E

 };

//uint8_t TxAddress[] = {"1Node"};
//uint8_t MyAddress[] = {"1Node"};

/* Data received and data for send */
uint8_t  Receive[32];
int flag=0,i=0; //Flag=1: correct data, Flag=0: incorrect data

/* NRF transmission status */
TM_NRF24L01_Transmit_Status_t transmissionStatus;

void USART_initialise()  //Initialising 8-bit usart
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef z;
	z.USART_WordLength = USART_WordLength_8b;
	z.USART_BaudRate = 9600;
	z.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	z.USART_Mode = USART_Mode_Tx;
	z.USART_StopBits = USART_StopBits_1;
	z.USART_Parity = USART_Parity_No;
	USART_Init(USART1, &z);
	USART_Cmd(USART1, ENABLE);
}

void ssl_LED_init()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //Initialisation of clock
	  GPIO_InitTypeDef x;     //Defining a structure named 'x'
		x.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
		x.GPIO_Mode = GPIO_Mode_OUT;
		x.GPIO_Speed = GPIO_Speed_50MHz;
		x.GPIO_OType = GPIO_OType_PP;
		x.GPIO_PuPd = GPIO_PuPd_NOPULL;
		//GPIO_StructInit(x);   //why do we add this satetment? This refers to the same statements as given  above

		GPIO_Init(GPIOE, &x);
}

void Data()
{
	//use flag
	if(Receive[0] == 5)// && (Receive[1] == 50) && (Receive[2] == 40) && (Receive[3] == 60) && (Receive[4] == 70))
	{
		flag = 1;
		//GPIO_SetBits(GPIOD, GPIO_Pin_14);
					//GPIO_SetBits(GPIOD, GPIO_Pin_12);
	}
	else
	{
		flag = 1;
		//GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		///GPIO_ResetBits(GPIOD, GPIO_Pin_12);
						//GPIO_ResetBits(GPIOD, GPIO_Pin_14);
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


int main(void) {

	ssl_LED_init(); //Initialising LED PE1 on new ssl board. 
	LED_initialise();
	/* Initialize NRF24L01+ on channel 1 and 32bytes of payload */
	/* By default 2Mbps data rate and 0dBm output power */
	/* NRF24L01 goes to RX mode by default */
	TM_NRF24L01_Init(18, 32);
	//TM_NRF24L01_SetCrcLength(NRF24L01_CRC_8);
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_250k, TM_NRF24L01_OutputPower_M18dBm); /* Set RF settings, Data rate to 2Mbps, Output power to -18dBm */
	TM_NRF24L01_SetMyAddress(MyAddress);/* Set my address, 5 bytes */
	//TM_NRF24L01_SetTxAddress(TxAddress); /* Set TX address, 5 bytes */
	//USART_initialise(); //Calling USART function

	GPIO_SetBits(GPIOE, GPIO_Pin_0);
	GPIO_ResetBits(GPIOE, GPIO_Pin_1);
	 TM_NRF24L01_PowerUpRx();

	while (1) {
		
		/* If data is ready on NRF24L01+ */
		if (TM_NRF24L01_DataReady()) {


			//GPIO_SetBits(GPIOE, GPIO_Pin_0);
			TM_NRF24L01_GetData(Receive); /* Get data from NRF24L01+ */
			//TM_NRF24L01_Transmit(Receive);
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
			GPIO_SetBits(GPIOD, GPIO_Pin_14);

			if(Receive[0] == 6 && Receive[1] == 6 && Receive[2] == 6 && Receive[3] == 6 && Receive[4] == 6 && Receive[5] == 6 )// && Receive[1] == 1 && Receive[2] == 2 && Receive[3] == 3)
				{
			GPIO_SetBits(GPIOE, GPIO_Pin_1);}


			//Data();

//			do {
//			                transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
//			            } while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending);
			            /* Send done */
			           // TM_DISCO_LedOff(LED_GREEN);

			            for(i =0;i<10000;i++){};
			            	
			            GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			            GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			            GPIO_ResetBits(GPIOE, GPIO_Pin_1);


			            /* Go back to RX Mode */
			            TM_NRF24L01_PowerUpRx();

//			if(flag == 1)
//			{
//			GPIO_SetBits(GPIOE, GPIO_Pin_1);
//			//GPIO_ResetBits(GPIOE, GPIO_Pin_0);
//			GPIO_SetBits(GPIOD, GPIO_Pin_13);
//			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
//			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
//			}
//			else
//			{
//				GPIO_ResetBits(GPIOE, GPIO_Pin_1);
//				GPIO_SetBits(GPIOD, GPIO_Pin_13);
//				GPIO_SetBits(GPIOD, GPIO_Pin_12);
//				GPIO_ResetBits(GPIOD, GPIO_Pin_14);
//			}
			
			/* Go back to RX mode */
			TM_NRF24L01_PowerUpRx();			
		}
//
		//GPIO_ResetBits(GPIOE, GPIO_Pin_1);
	}
}
