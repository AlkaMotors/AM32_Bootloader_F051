/* Bootloader */

#define BOOTLOADER_VERSION 6

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include "main.h"

#if defined(BOOTLOADER_PB4)
#define USE_PB4        // for iflight
#elif defined(BOOTLOADER_PA2)
#define USE_PA2
#else
#error Variant not defined
#endif

//#define USE_ADC_INPUT      // will go right to application and ignore eeprom

#include <string.h>
#include "bootloader.h"

#define STM32_FLASH_START 0x08000000
#define FIRMWARE_RELATIVE_START 0x1000
#define EEPROM_RELATIVE_START 0x7c00

uint8_t __attribute__ ((section(".bootloader_info"))) bootloader_version = BOOTLOADER_VERSION;

typedef void (*pFunction)(void);

#define APPLICATION_ADDRESS     (uint32_t)(STM32_FLASH_START + FIRMWARE_RELATIVE_START) // 4k

#define EEPROM_START_ADD         (uint32_t)(STM32_FLASH_START + EEPROM_RELATIVE_START)
#define FLASH_END_ADD           (uint32_t)(STM32_FLASH_START + 0x7FFF)               // 32 k


#define CMD_RUN             0x00
#define CMD_PROG_FLASH      0x01
#define CMD_ERASE_FLASH     0x02
#define CMD_READ_FLASH_SIL  0x03
#define CMD_VERIFY_FLASH    0x03
#define CMD_VERIFY_FLASH_ARM 0x04
#define CMD_READ_EEPROM     0x04
#define CMD_PROG_EEPROM     0x05
#define CMD_READ_SRAM       0x06
#define CMD_READ_FLASH_ATM  0x07
#define CMD_KEEP_ALIVE      0xFD
#define CMD_SET_ADDRESS     0xFF
#define CMD_SET_BUFFER      0xFE


#ifdef USE_PA2
#define input_pin     LL_GPIO_PIN_2
#define input_port       GPIOA
#define shift_amount       2
#endif

#ifdef USE_PB4
#define input_pin       LL_GPIO_PIN_4
#define shift_amount        4
#define input_port        GPIOB
#endif


char receviedByte;
int receivedCount;
int count = 0;
char messagereceived = 0;
uint16_t invalid_command = 0;
uint16_t address_expected_increment;
int cmd = 0;
char eeprom_req = 0;
int received;
uint8_t deviceInfo[9] = {0x34,0x37,0x31,0x64,0x1f,0x06,0x06,0x01, 0x30};      // stm32 device info

//uint8_t deviceInfo[9] = {0x34,0x37,0x31,0x64,0xf3,0x90,0x06,0x01, 0x30};       // silabs device id
//uint8_t deviceInfo[9] = {0x34,0x37,0x31,0x64,0xe8,0xb2,0x06,0x01, 0x30};     // blheli_s identifier


size_t str_len;
char connected = 0;
uint8_t rxBuffer[258];
uint8_t payLoadBuffer[256];
char rxbyte=0;
uint32_t address;
int tick = 0;

typedef union __attribute__ ((packed)) {
    uint8_t bytes[2];
    uint16_t word;
} uint8_16_u;
uint16_t len;
uint8_t received_crc_low_byte;
uint8_t received_crc_high_byte;
uint8_t calculated_crc_low_byte;
uint8_t calculated_crc_high_byte;
uint16_t payload_buffer_size;
char incoming_payload_no_command = 0;

char bootloaderactive = 1;

uint32_t JumpAddress;
pFunction JumpToApplication;


void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
static void MX_GPIO_INPUT_INIT(void);
void processmessage(void);
void serialwriteChar(char data);
void sendString(uint8_t data[], int len);
void recieveBuffer();

#define BAUDRATE              19200
#define BITTIME          1000000/BAUDRATE
#define HALFBITTIME       500000/BAUDRATE



void delayMicroseconds(uint32_t micros){
	TIM2->CNT = 0;
	while (TIM2->CNT < micros){

	}
}

void jump(){

	__disable_irq();
	JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
	uint8_t value = *(uint8_t*)(EEPROM_START_ADD);
#ifdef USE_ADC_INPUT
#else
	if (value != 0x01){      // check first byte of eeprom to see if its programmed, if not do not jump
		invalid_command = 0;
		return;
	}
#endif
    JumpToApplication = (pFunction) JumpAddress;
  __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
   JumpToApplication();

}



void makeCrc(uint8_t* pBuff, uint16_t length){
	static uint8_16_u CRC_16;
		CRC_16.word=0;

		for(int i = 0; i < length; i++) {


		     uint8_t xb = pBuff[i];
		     for (uint8_t j = 0; j < 8; j++)
		     {
		         if (((xb & 0x01) ^ (CRC_16.word & 0x0001)) !=0 ) {
		             CRC_16.word = CRC_16.word >> 1;
		             CRC_16.word = CRC_16.word ^ 0xA001;
		         } else {
		             CRC_16.word = CRC_16.word >> 1;
		         }
		         xb = xb >> 1;
		     }
		 }
		calculated_crc_low_byte = CRC_16.bytes[0];
		calculated_crc_high_byte = CRC_16.bytes[1];

}

char checkCrc(uint8_t* pBuff, uint16_t length){

		char received_crc_low_byte2 = pBuff[length];          // one higher than len in buffer
		char received_crc_high_byte2 = pBuff[length+1];
		makeCrc(pBuff,length);

		if((calculated_crc_low_byte==received_crc_low_byte2)   && (calculated_crc_high_byte==received_crc_high_byte2)){
			return 1;
		}else{
			return 0;
		}
}


void setReceive(){
	MX_GPIO_INPUT_INIT();
received = 0;

}

void setTransmit(){
LL_GPIO_SetPinMode(input_port, input_pin, LL_GPIO_MODE_OUTPUT);       // set as reciever // clear bits and set receive bits..
}


void send_ACK(){
    setTransmit();
    serialwriteChar(0x30);             // good ack!
	setReceive();
}

void send_BAD_ACK(){
    setTransmit();
 		serialwriteChar(0xC1);                // bad command message.
 		setReceive();
}

void send_BAD_CRC_ACK(){
    setTransmit();
 		serialwriteChar(0xC2);                // bad command message.
 		setReceive();
}

void sendDeviceInfo(){
	setTransmit();
	sendString(deviceInfo,9);
	setReceive();

}

bool checkAddressWritable(uint32_t address) {
	return address >= APPLICATION_ADDRESS;
}

void decodeInput(){
	if(incoming_payload_no_command){
		len = payload_buffer_size;
	//	received_crc_low_byte = rxBuffer[len];          // one higher than len in buffer
	//	received_crc_high_byte = rxBuffer[len+1];
		if(checkCrc(rxBuffer,len)){
			memset(payLoadBuffer, 0, sizeof(payLoadBuffer));             // reset buffer

			for(int i = 0; i < len; i++){
				payLoadBuffer[i]= rxBuffer[i];
			}
			send_ACK();
			incoming_payload_no_command = 0;
			return;
		}else{
			send_BAD_CRC_ACK();
			return;
		}
	}

	cmd = rxBuffer[0];

	if(rxBuffer[16] == 0x7d){
		if(rxBuffer[8] == 13 && rxBuffer[9] == 66){
			sendDeviceInfo();
			rxBuffer[20]= 0;

		}
		return;
	}

	if(rxBuffer[20] == 0x7d){
			if(rxBuffer[12] == 13 && rxBuffer[13] == 66){
				sendDeviceInfo();
				rxBuffer[20]= 0;
				return;
			}

	}
	if(rxBuffer[40] == 0x7d){
				if(rxBuffer[32] == 13 && rxBuffer[33] == 66){
					sendDeviceInfo();
					rxBuffer[20]= 0;
					return;
				}
		}

	if(cmd == CMD_RUN){         // starts the main app
		if((rxBuffer[1] == 0) && (rxBuffer[2] == 0) && (rxBuffer[3] == 0)){
			invalid_command = 101;
		}
	}

	if(cmd == CMD_PROG_FLASH){
		len = 2;
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}

		if (!checkAddressWritable(address)) {
			send_BAD_ACK();

			return;
		}

		save_flash_nolib((uint8_t*)payLoadBuffer, payload_buffer_size,address);
		send_ACK();

	 	return;
	}

	if(cmd == CMD_SET_ADDRESS){             //  command set addressinput format is: CMD, 00 , High byte address, Low byte address, crclb ,crchb
		len = 4;  // package without 2 byte crc
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}


	    // will send Ack 0x30 and read input after transfer out callback
		invalid_command = 0;
		address = STM32_FLASH_START + (rxBuffer[2] << 8 | rxBuffer[3]);
		send_ACK();

		return;
	}

	if(cmd == CMD_SET_BUFFER){        // for writing buffer rx buffer 0 = command byte.  command set address, input , format is CMD, 00 , 00 or 01 (if buffer is 256), buffer_size,
		len = 4;  // package without 2 byte crc
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}

        // no ack with command set buffer;
       	if(rxBuffer[2] == 0x01){
       		payload_buffer_size = 256;                          // if nothing in this buffer
       	}else{
	        payload_buffer_size = rxBuffer[3];
        }
	    incoming_payload_no_command = 1;
	    address_expected_increment = 256;
        setReceive();

        return;
	}

	if(cmd == CMD_KEEP_ALIVE){
		len = 2;
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}

	   	setTransmit();
	 	serialwriteChar(0xC1);                // bad command message.
		setReceive();

		return;
	}

	if(cmd == CMD_ERASE_FLASH){
		len = 2;
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}

		if (!checkAddressWritable(address)) {
			send_BAD_ACK();

			return;
		}

		send_ACK();
		return;
	}

	if(cmd == CMD_READ_EEPROM){
		eeprom_req = 1;
	}

	if(cmd == CMD_READ_FLASH_SIL){     // for sending contents of flash memory at the memory location set in bootloader.c need to still set memory with data from set mem command
		len = 2;
		if (!checkCrc((uint8_t*)rxBuffer, len)) {
			send_BAD_CRC_ACK();

			return;
		}

		count++;
		uint16_t out_buffer_size = rxBuffer[1];//
		if(out_buffer_size == 0){
			out_buffer_size = 256;
		}
		address_expected_increment = 128;

		setTransmit();
		uint8_t read_data[out_buffer_size + 3];        // make buffer 3 larger to fit CRC and ACK
		memset(read_data, 0, sizeof(read_data));
        //    read_flash((uint8_t*)read_data , address);                 // make sure read_flash reads two less than buffer.
		read_flash_bin((uint8_t*)read_data , address, out_buffer_size);

        makeCrc(read_data,out_buffer_size);
        read_data[out_buffer_size] = calculated_crc_low_byte;
        read_data[out_buffer_size + 1] = calculated_crc_high_byte;
        read_data[out_buffer_size + 2] = 0x30;
        sendString(read_data, out_buffer_size+3);

		setReceive();

		return;
	}

    setTransmit();

	serialwriteChar(0xC1);                // bad command message.
	invalid_command++;
 	setReceive();
}


void serialreadChar()
{
rxbyte=0;
while(!(input_port->IDR & input_pin)){ // wait for rx to go high
	if(TIM2->CNT > 200000){
			invalid_command = 101;
			return;
	}
}
while((input_port->IDR & input_pin)){   // wait for it go go low
	if(TIM2->CNT > 250 && messagereceived){
		return;
	}
}

delayMicroseconds(HALFBITTIME);//wait to get the center of bit time

int bits_to_read = 0;
while (bits_to_read < 8) {
	delayMicroseconds(BITTIME);
	rxbyte = rxbyte | ((( input_port->IDR & input_pin)) >> shift_amount) << bits_to_read;
  bits_to_read++;
}

delayMicroseconds(HALFBITTIME); //wait till the stop bit time begins
messagereceived = 1;
receviedByte = rxbyte;
//return rxbyte;

}




void serialwriteChar(char data)
{
input_port->BRR = input_pin;; //initiate start bit
char bits_to_read = 0;
while (bits_to_read < 8) {
  delayMicroseconds(BITTIME);
  if (data & 0x01) {
	  input_port->BSRR = input_pin;
  }else{
	  input_port->BRR = input_pin;
  }
  bits_to_read++;
  data = data >> 1;
}

delayMicroseconds(BITTIME);
input_port->BSRR = input_pin; //write the stop bit

// if more than one byte a delay is needed after stop bit,
//if its the only one no delay, the sendstring function adds delay after each bit

//if(cmd == 255 || cmd == 254 || cmd == 1  || incoming_payload_no_command){
//
//}else{
//	delayMicroseconds(BITTIME);
//}


}


void sendString(uint8_t *data, int len){

	for(int i = 0; i < len; i++){
		serialwriteChar(data[i]);
		delayMicroseconds(BITTIME);

	}
}

void recieveBuffer(){

	//int i = 0;
	count = 0;
	messagereceived = 0;
	memset(rxBuffer, 0, sizeof(rxBuffer));

	for(int i = 0; i < sizeof(rxBuffer); i++){
	serialreadChar();


	if(incoming_payload_no_command){
		if(count == payload_buffer_size+2){

			break;
		}
		rxBuffer[i] = rxbyte;
		count++;
	}else{
		if(TIM2->CNT > 250){
		count = 0;
		break;
	    }else{
		rxBuffer[i] = rxbyte;
		if(i == 257){
			invalid_command+=20;       // needs one hundred to trigger a jump but will be reset on next set address commmand

		}
	}
	}
	}
		decodeInput();
}

int main(void)
{

//Prevent warnings
(void)bootloader_version;

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);



  FLASH->ACR |= FLASH_ACR_PRFTBE;   // prefetch buffer enable

  SystemClock_Config();


  MX_TIM2_Init();
  LL_TIM_EnableCounter(TIM2);

  MX_GPIO_INPUT_INIT();     // init the pin with a pulldown

  LL_GPIO_SetPinPull(input_port, input_pin, LL_GPIO_PULL_DOWN);
  delayMicroseconds(1000);

  for(int i = 0 ; i < 1000; i ++){
	 if( !(input_port->IDR & input_pin)){  // if the pin is low for 100ms straight there is no signal jump to application to beep
		 jump();
	 }
	  delayMicroseconds(10);
  }

  LL_GPIO_SetPinPull(input_port, input_pin, LL_GPIO_PULL_UP);

  #ifdef USE_ADC_INPUT  // go right to application
  jump();

#endif
//  sendDeviceInfo();
  while (1)
  {

	  recieveBuffer();
	  if (invalid_command > 100){
		  jump();
	  }

  }

}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(48000000);
  LL_SetSystemCoreClock(48000000);
}


static void MX_TIM2_Init(void)
{



  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);


  TIM_InitStruct.Prescaler = 47;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFFFFFFFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);

}



static void MX_GPIO_INPUT_INIT(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
 // LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_2);
  /* GPIO Ports Clock Enable */
#ifdef USE_PB4
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif
#ifdef USE_PA2
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif


  /**/
  GPIO_InitStruct.Pin = input_pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(input_port, &GPIO_InitStruct);

}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


