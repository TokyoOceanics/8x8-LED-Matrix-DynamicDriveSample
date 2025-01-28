/*
 * send_data_to_sr.c
 *
 *  Created on: 2023/06/28
 *      Author: x-lib
 *
 *      2023/09/03 Flash_SR関数の訂正
 */


#include "main.h"


#if 0
//バグ含みの未検証版　4ビット、1nibbleごとの転送
void Send_Nibble_To_SR(uint8_t bytes)
{
	uint8_t i;

	for(i=0; i<4; i++){
		if((bytes>>i)&0b00000001){
			//Turn on SER
			GPIOA->ODR |= GPIO_ODR_ODR4; //PA4=on
		}else{
			//Turn off SER
			GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off
		}

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}

	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off


}
#endif


//8bit,1byteをLSBから送出する
void Send_Byte_To_SR(uint8_t bytes)
{
	uint8_t i;

	for(i=0; i<8; i++){
		if((bytes>>i)&0b00000001){
			//Turn on SER
			GPIOA->ODR |= GPIO_ODR_ODR4; //PA4=on
		}else{
			//Turn off SER
			GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off
		}

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}

	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off


}



//8bit,1byteをMSBから送出する
void Send_Byte_To_SR_From_MSB(uint8_t bytes)
{
	uint8_t i;

	for(i=0; i<8; i++){
		if((bytes<<i)&0b10000000){
			//Turn on SER
			GPIOA->ODR |= GPIO_ODR_ODR4; //PA4=on
		}else{
			//Turn off SER
			GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off
		}

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}

	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off


}



//16bit,1wordをLSBから送出する

void Send_Word_To_SR(uint16_t words)
{
	uint8_t i;

	for(i=0; i<16; i++){
		if((words>>i)&0x01){
			//Turn on SER
			GPIOA->ODR |= GPIO_ODR_ODR4; //PA4=on
		}else{
			//Turn off SER
			GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off
		}

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}

	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off


}


//16bit,1wordをMSBから送出する

void Send_Word_To_SR_From_MSB(uint16_t words)
{
	uint8_t i;

	for(i=0; i<16; i++){
		if((words<<i)&0x8000){
			//Turn on SER
			GPIOA->ODR |= GPIO_ODR_ODR4; //PA4=on
		}else{
			//Turn off SER
			GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off
		}

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}

	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off


}



void Flash_SR(void)
{
	GPIOA->ODR &= (~GPIO_ODR_ODR4); //PA4=off

	for(int i=0; i<16; i++){

		//Send SRCLK
		GPIOA->ODR |= GPIO_ODR_ODR5; //PA5=on
		GPIOA->ODR &= (~GPIO_ODR_ODR5); //PA5=off
	}
	//Send RCLK
	GPIOA->ODR |= GPIO_ODR_ODR6; //PA6=on
	GPIOA->ODR &= (~GPIO_ODR_ODR6); //PA6=off

}

