/*
 * StopWatch1a.c
 *
 *  Created on: 2023/09/13
 *      Author: x-lib
 */

/*
 *　10秒までのストップウォッチ
 *
 * SW2でスタート，ストップ、再開
 * ストップ時にSW1押しでリセット
 *
  stop watch その１-自宅作業

 *
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

/*
 * F103RB LD2
 *
 */




/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* マクロ定義　*/
#define ROW_SIZE 8	//行数


//          0bFEDCBA9876543210
#define SW1 0b0000000010000000	//Sw1(PA7)用のマスク値
#define SW2 0b0000000100000000	//Sw1(PA8)用のマスク値

#define	STOP	0	//停止状態
#define RUN		1	//カウント状態


//グローバル変数
int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）

int pattern_num;//表示させたい8x8LEDマトリクスのフォントデータの番号

int time_data = 0;	//時間データ


int State_ID=STOP;



//8x8 LEDマトリクスのフォントデータ

//uint8_t face[]={0x0,0x00,0x00,0x00,0x0,0x00,0x00,0x00};		//表示0
//uint8_t one []={0x00,0x08,0x18,0x28,0x};//1
//uint8_t cat[]={0x22,0x66,0xFF,0x99,0xBB,0xFF,0x46,0x38};		//猫
//uint8_t apple[]={0xC,0x8,0x7E,0xDF,0xBF,0xBF,0xDF,0x7E};	//リンゴ

//数字0~10までのフォントデータ
uint8_t number_data[][8]= {
		{0x0,0x38,0x44,0x44,0x44,0x44,0x44,0x38},	//0
		{0x0,0x8,0x18,0x28,0x8,0x8,0x8,0x3E}, 		//1
		{0x0,0x18,0x24,0x24,0x8,0x10,0x20,0x3C},	//2
		{0x0,0x18,0x24,0x4,0x18,0x4,0x24,0x18},		//3
		{0x0,0x8,0x10,0x28,0x48,0x7C,0x8,0x8},		//4
		{0x0,0x3C,0x20,0x3C,0x4,0x4,0x3C,0x0},		//5
		{0x0,0x18,0x24,0x20,0x38,0x24,0x24,0x38},	//6
		{0x0,0x3C,0x24,0x24,0x4,0x4,0x4,0x4},		//7
		{0x0,0x18,0x24,0x24,0x18,0x24,0x24,0x18},	//8
		{0x0,0x18,0x24,0x24,0x1C,0x4,0x4,0x1C},		//9
		{0x0,0x26,0x69,0xA9,0x29,0x29,0x29,0xF6}	//10
};



//ある時点でLEDマトリクスに表示させたいパターンのフォントデータの先頭アドレス
uint8_t *font_pattern_data;



//スイッチ関係の変数
int up=0;		//スイッチの立ち上がり検出フラグ、0で初期化
int sw_now=0;	//現在のスイッチ押下状態、0で初期化




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

	while(1){

		while(!up);	//スイッチが何か押されるまで待つ。
		up=0;		//押されたら、フラグをクリアする。

		switch(sw_now){	//押されたスイッチの状態によって、State_IDを変える

			case SW1:
				if(State_ID==STOP)
					time_data=0;
				break;

			case SW2:
				if(State_ID==STOP)
					State_ID=RUN;
				else
					State_ID=STOP;
				break;

//			case SW1|SW2:
//				if(State_ID==STOP)
//					time_data=0;
//				break;
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
	/*SysTick タイマ割込みの間隔を 1ms に設定 */
	// 割り込み間隔はSysTick_Config()の引数をRCC_Clocks.HCLK_Frequency / 1000　として、1msec
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//

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
// 2023/09/12 安藤先生
//
//SysTick_Handler内で呼び出される
//

uint16_t Setup_Serial_Data_2(uint8_t font_data[]){

	uint16_t col_data, row_data;
	uint16_t serial_data;

	int j;

	static int col_id=0;


	if(col_id==8){
			serial_data=0;	//表示OFFの場合のフォントデータ
	}else{
		col_data=(0x8000 >> col_id);	//表示させたい桁の生成
		row_data=0;
		for(j=0; j<8; j++){
			if(font_data[j]&(0x80>>col_id)){
				row_data |= (0x1<<j);
			}
		}
		serial_data=row_data|col_data;
	}

	if(col_id==8){
		col_id=0;
	}else{
		col_id++;
	}

	return serial_data;

}


/*
 * 待ち時間関数　（SysTick_Handlerと連携）
 */

void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}





/*
**	get_sw --- 単純に現在のスイッチの値を正論理（アクティブハイ）で取得する関数
*/

int get_sw(void)
{
	int sw;

	sw = GPIOA->IDR;		// 現在のスイッチの値を読み込む
	sw = ~sw;				// 負論理なのでビットを反転する
	sw = sw & (SW1|SW2);	// スイッチ以外のビットをマスクする

	return sw;
}

/*
**    read_sw --- チャタリング防止機能付きのスイッチの確定値を読み込む関数
*/

int read_sw(void)
{
	static int swc  = 0;	// 今回読み込んだスイッチの値
	static int swp1 = 0;	// 前回読み込んだスイッチの値
	static int swp2 = 0;	// 前々回に読み込んだスイッチの値
	static int sw   = 0;	// 現在のスイッチの確定値

	swp2 = swp1;				// 前々回の値を保存する
	swp1 = swc;					// 前回の値を保存する
	swc  = get_sw();			// 新たに今回のスイッチの値を読み込む

	if ((swp2 == swp1) && (swp1 == swc)) {	// 今回、前回、前々回の値が全て等しい
		sw = swc;							// 場合、今回の値を現在の確定値とする
	}

	return sw;		// 現在のスイッチの確定値を返す
}



/*
 * SysTick_Handler関数
*/

void SysTick_Handler(void)
{

	static int count_1000ms=0;	//1秒を作るために1000回の呼び出しをカウントする変数　（1msec*1000回＝1秒）

	static int sw_last = 0;	// 前回のスイッチの確定値

	//
	//スイッチ関係の処理
	//
	sw_now = read_sw();			// スイッチの確定値を読み込む

	if (sw_now != sw_last) {	// 前回のスイッチの確定値と異なる場合だけ以下の処理を行う
		up = sw_now & ~sw_last;	// スイッチの立上り（何れかのスイッチが押されたか）を検出
		sw_last = sw_now;		// 現在のスイッチの確定値を sw_last に保存する
	}




	//
	//待ち時間関数　Wait_Time()用の変数の処理
	//
	if(TimeInt>0) TimeInt--;


	//1秒のカウントアップ処理
	if(State_ID==RUN){
		if(count_1000ms<1000){
			count_1000ms++;	//SysTick_Handlerが1000回呼び出されるごとにカウントを1増やす
		}else{
			time_data++;	//1000回目の呼び出しで、1秒経過をカウントアップする。
			count_1000ms=0;	//1000回目で、カウンタをリセットする。
		}
	}

	//時間情報の表示
	font_pattern_data=number_data[time_data];
	Send_Word_To_SR(Setup_Serial_Data_2(font_pattern_data));




}
