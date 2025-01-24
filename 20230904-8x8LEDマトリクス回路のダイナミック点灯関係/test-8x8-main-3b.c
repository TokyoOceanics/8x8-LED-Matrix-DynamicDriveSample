/*
 * test-8x8-main-3b.c
 *
 *  Created on: 2023/09/03
 *      Author: x-lib
 */

/*
 * シリアル書き込みの評価
 * 8x8LEDマトリクス回路の動作確認
 * ダイナミック点灯　割り込み版
 *
 * 変数名変更、コメントを訂正強化
 * mainでマトリクスデータのフォントを指定するように改定
 *
 * SW1,SW2の入力に応じて、LEDマトリクス上の表示を変える予定だが、この版では未実装。
 *
 */

/* =======================================
 * 74595-SerialToParallel-Board Connection
 * =======================================
 *
 * --------------------------------------
 * Fnctn	MpuPrt	Nucleo-F103B PinNum
 * --------------------------------------
 * SER		PA4		CN7-32
 * SRCLK	PA5		CN10-11
 * RCLK		PA6		CN10-13
 *

 *　SW1		PA7		CN10-15
 *　SW2		PA8		CN10-23

 *
 *
 * --------------------------------------
 *
 * --------------------------------------
 * Fnc(Out)		8x8 MTRX Board Pin
 * --------------------------------------
 * Q0			COL7
 * Q1			COL6
 * Q2			COL5
 * Q3			COL4
 * Q4			COL3
 * Q5			COL2
 * Q6			COL1
 * Q7			COL0
 * Q8			ROW7
 * Q9			ROW6
 * QA			ROW5
 * QB			ROW4
 * QC			ROW3
 * QD			ROW2
 * QE			ROW1
 * QF			ROW0
 *
 *
 * -----------------------------------
 * PWR Line		Nucleo-F103RB
 * -----------------------------------
 * Red			3V3
 * GND			GND
 *
 *
 * ==========================================
 * 8x8 LED MATRIX BOARD Power Line Connection
 * ==========================================
 *
-----------------------------------
 * PWR Line		Nucleo-F103RB
 * -----------------------------------
 * Red			5V(Developping)/12V(Real Operation)
 * GND			GND
 */






/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* マクロ定義　*/
#define ROW_SIZE 8	//行数



//グローバル変数
int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）

int pattern_num;//表示させたい8x8LEDマトリクスのフォントデータの番号

//8x8 LEDマトリクスのフォントデータ
//uint8_t myfont[][ROW_SIZE]={
//		{0x10,0x28,044,0x44,0x7c,0x44,0x44,0x44}, // A
//		{0x7C,0x44,0x44,0x44,0x7C,0x44,0x44,0x44}, // H
//		{0xFE,0x6,0xC,0x18,0x30,0x60,0xFE,0x0}, // Z
//		{0x24,0xEE,0x44,0x0,0x10,0x44,0x38,0x0}, // face
//		{0x55,0xAA,0xFF,0xAA,0xDB,0xEE,0x77,0xFF} //random
//};

uint8_t myfont[][ROW_SIZE]={
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0xBF,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0xDF,0xBF,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0xEF,0xDF,0xBF,0x7F,0xFF,0xFF,0xFF,0xFF},
		{0xF7,0xEF,0xDF,0xBF,0x7F,0xFF,0xFF,0xFF},
		{0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xFF,0xFF},
		{0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,0xFF},
		{0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F},
		{0xFF,0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF},
		{0xFF,0xFF,0xFE,0xFD,0xFB,0xF7,0xEF,0xDF},
		{0xFF,0xFF,0xFF,0xFE,0xFD,0xFB,0xF7,0xEF},
		{0xFF,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB,0xF7},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFD,0xFB},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFD},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE}
};

//ある時点でLEDマトリクスに表示させたいパターンのフォントデータの先頭アドレス
uint8_t *font_pattern_data;














//関数のプロトタイプ宣言
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handlerを使う待ち時間処理


//send_data_to_sr.cに記述された関数群
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

extern void Flash_SR(void);





//
//メイン関数
//
int main(void)
{

	my_init_system();//ハードウェアの初期化



	//font_pattern_dataに表示させたいフォントデータ領域の先頭アドレスをセットすると、
	//LEDマトリクス上にそのパターンが表示される。
	//
	//Wait_Timeで指定される時間間隔で、LEDマトリクス表示が更新され、その動作が繰り返される。
	//
	while(1){

		for(pattern_num=0; pattern_num<(sizeof myfont)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
			font_pattern_data=myfont[pattern_num];	//表示させたいフォントデータの先頭アドレスを取得。これはSysTick_Handlerに渡される。
			Wait_Time(100);	//待ち
		}

	}
}




//
//ハードウェアの初期化
//
void my_init_system(void)
{


	/* クロック情報取得／設定の為の構造体定義 */
	RCC_ClocksTypeDef RCC_Clocks;

	/* システムのクロック情報の取得 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick タイマの設定 */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick タイマ割込みの間隔を 10ms に設定

	//GPIO　Aを有効化
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA のクロックを有効にする

	//ポートAの入出力設定

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;
}


//
//シリアル・パラレル変換回路に送り出すデータを生成する関数
//
//SysTick_Handler内で呼び出される
//
uint16_t Setup_Serial_Data(uint8_t font_data[]){

	static int row_index=0;
	static int col_pos=0x8000; //LEDマトリクスのカラムColumnを指定する変数。MSBがCOL0に接続
	uint16_t output_data=0x0;



	switch(row_index){
		case 0:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;
		case 1:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;

		case 2:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;
		case 3:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;

		case 4:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;

		case 5:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;

		case 6:
			output_data=font_data[row_index]|col_pos;
			col_pos=col_pos>>1;
			row_index++;
			break;

		case 7:
			output_data=font_data[row_index]|col_pos;
			col_pos=0x8000;
			row_index=0;
			break;
	}

	return output_data;

}





void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}

//
// 割り込み間隔はSysTick_Config()の引数を
// RCC_Clocks.HCLK_Frequency / 1000　として、1msec
//

void SysTick_Handler(void)
{

	//
	//待ち時間関数　Wait_Time()用の変数の処理
	if(TimeInt>0) TimeInt--;



	//
	//シリアル・パラレル変換回路へデータを送出（LEDマトリクスが表示される）

	Send_Word_To_SR_From_MSB(Setup_Serial_Data(font_pattern_data));

	//Send_Word_To_SR(Setup_Serial_Data(font_pattern_data));



}
