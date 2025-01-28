/*
 * test-8LEDs-main-01.c
 *
 *  Created on: 2023/06/26
 *      Author: x-lib
 */

/*
 *
 * シリアルパラレル変換回路のテストプログラム
 * ７４５９５（シリアル・パラレル変換IC）に、マイコンからシリアルでデータを書き込むサンプル。
 *
 * 動作は、LED-SW基板上の8個のLEDを2進数的に点灯させる。
 *
 *
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* S/P回路          NUCLEO-F103RB
 * SER		PA4	CN7-32
 * SRCLK	PA5	CN10-11
 * RCLK		PA6	CN10-13
 *
 *　3.3　　　　　3V3 CN7-16
 *　GND　　　　　GND CN7-32など


 * S/P回路　　　LED-SW基板
 *　Q0　　　　　　一番右側のLED（赤）
 *　Q1　　　　　　その左隣のLED（赤）
 *　・　　　　　　　　・
 *　・　　　　　　　　・
 *　Q8　　　　　　一番左側のLED（緑）
 *
 *　3.3ピン　　　3.3
 *　GNDピン　　　GND
 *
 *
 *
 */

//関数のプロトタイプ宣言
void my_init_system(void);

void wait_delay(int delay);


//send_data_to_sr.cで定義されている関数
extern void Send_Byte_To_SR(uint8_t bytes);


//
//メイン関数
//
int main(void)
{

	uint16_t test_data;

	my_init_system();

	for(test_data=0x00; test_data<0x0fff; test_data++){

		Send_Byte_To_SR(~test_data);
		wait_delay(99999);
	}
	while(1);
}


//
//ハードウェアの初期化
//
void my_init_system(void)
{
	//GPIO　Aを有効化
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA のクロックを有効にする

//ポートAの入出力設定

//               76543210
	GPIOA->CRL=0x41114444;

}


void wait_delay(int delay){

	for(int i=0; i<delay; i++);

}




void SysTick_Handler(void)
{

}
