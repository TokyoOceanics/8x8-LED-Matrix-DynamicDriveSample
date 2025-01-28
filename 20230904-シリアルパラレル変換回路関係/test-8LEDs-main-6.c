/*
 * test-8LEDs-main-5.c
 *
 *  Created on: 2023/07/11
 *      Author: x-lib
 */

/*
 * シリアル書き込みの評価
 * 鳥海先生ボードのLED点灯
 * ダイナミック点灯　割り込み版
 *
 * プログラム起動後に、経過時間をmm:ss形式で表示する。
 *
 *2023/08/24 コメント追加
 *          カウント動作の見直し
 *
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "ToriBoard01.h"

/*
 * 74595-SerialToParallel-Board Connection
 *
 * --------------------------------------
 * Fnc(In)	MpuPrt	Nucleo-F103B PinNum
 * --------------------------------------
 * SER		PA4		CN7-32
 * SRCLK	PA5		CN10-11
 * RCLK		PA6		CN10-13
 * --------------------------------------
 *
 * --------------------------------------
 * Fnc(Out)		Tori.Board Pin	(Color)
 * --------------------------------------
 * Q0		SEG-A	茶
 * Q1		SEG-B	赤
 * Q2		SEG-C	橙
 * Q3		SEG-D	黄
 * Q4		SEG-E	緑
 * Q5		SEG-F	青
 * Q6		SEG-G	紫
 * Q7		SEG-DP	灰
 * Q8		N.C.
 * Q9		N.C.
 * QA		AN3(Digit4)		白
 * QB		AN2(Digit3)		灰
 * QC		AN1(Digit2)		紫
 * QD		AN0(Digit1)		青
 *
 * Vdd	Vcc		赤
 * GND	GND		黄
 *
 */



//グローバル変数
int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）


int time_count;	//時刻情報　秒
int minutes_data[2];	//7セグ上位2桁に表示する数値（分）
int seconds_data[2];	//7セグ下位2桁に表示する数値（秒）


//関数のプロトタイプ宣言
void my_init_system(void);

void wait_delay(int delay);
void Wait_Time(int time);	//SysTick_Handlerを使う待ち時間処理

uint16_t make_digit_data(int i);
void display_number_on_7SegLED(int num, int digit_position);


//send_data_so_sr.cで定義されている関数群
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

void Flash_SR(void);




//
//メイン関数
//
int main(void)
{

	time_count=0;	//秒数を0で初期化する。

	my_init_system();




	while(1){

		Wait_Time(100);	//100msec待ち,1000として1000msec待ち
		if(time_count==(60*60-1))
			time_count=0;
		else
			time_count++;
	}


}

//時刻情報（MM:SS)を７セグLED表示するための各桁への表示データ生成
void  sec_to_mm_and_ss(void){

	int min, sec;


	min=time_count/60;
	sec=time_count%60;

	minutes_data[1]=min/10;	//分　10の位
	minutes_data[0]=min%10;	//分　1の位

	seconds_data[1]=sec/10;	//秒　10の位
	seconds_data[0]=sec%10;	//秒　1の位


}



//数値1桁を7セグ表示用データに変換する
uint16_t make_digit_data(int num)
{
	uint16_t output_data;
	output_data=(0xff00|(~seg7[num]));
	return output_data;
}

//指定の桁に数値1桁を表示する
void display_number_on_7SegLED(int num, int digit_position)
{
	uint16_t output_data;
	output_data=make_digit_data(num)&disp_digit[digit_position];
	Send_Word_To_SR_From_MSB(output_data);
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
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick タイマ割込みの間隔を 1ms に設定



	//GPIO　Aを有効化
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA のクロックを有効にする

//ポートAの入出力設定

//               76543210
	GPIOA->CRL=0x41114444;

}






void wait_delay(int delay){

	for(int i=0; i<delay; i++);


}



void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}



void SysTick_Handler(void)
{

	int segment_digit;	//7セグ表示桁位置番号　0で1桁目、1で2桁目、2で3桁目、3で4桁目、4で無選択(消灯)
	static int lighting_state=0;//7セグ表示桁の点灯制御用の状態変数、詳細は下記switch-case参照


	//
	//待ち時間関数　Wait_Time()用の変数の処理
	//割り込み間隔１ｍｓ、1回のダウンカウントに１ｍｓ
	if(TimeInt>0) TimeInt--;


	//7セグLED表示データ生成
	sec_to_mm_and_ss();


	//
	//7セグLED表示処理
	//

	//各桁の表示切替と、表示切替の瞬間にいったん全桁表示OFFのブランクを入れる処理
	switch(lighting_state){

		case 0:
			lighting_state=1;
			segment_digit=0;	//1桁目
			display_number_on_7SegLED(seconds_data[0], segment_digit);
			break;
		case 1:
			lighting_state=2;
			segment_digit=1;	//2桁目
			display_number_on_7SegLED(seconds_data[1], segment_digit);
			break;
		case 2:
			lighting_state=3;
			segment_digit=2;	//3桁目
			display_number_on_7SegLED(minutes_data[0], segment_digit);
			break;
		case 3:
			lighting_state=4;
			segment_digit=3;	//4桁目
			display_number_on_7SegLED(minutes_data[1], segment_digit);
			break;
		case 4:
			lighting_state=0;
			segment_digit=4;	//表示ブランク
			break;
	}



}
