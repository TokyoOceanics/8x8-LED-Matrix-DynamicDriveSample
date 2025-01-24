/*
 * test-8x8-main-5a.c
 *
 *  Created on: 2023/09/12
 *      Author: x-lib
 */

/*
 *
 * SW0,SW1の入力に応じて、LEDマトリクス上の表示を変える。
 *
 * 起動直後は、インベーダを表示。
 * SW0を押すと猫、SW1を押すとリンゴ、SW0とSW1を同時に押すとインベーダが、
 * それぞれ表示される。
 *
 * フォントデータ生成の方法を修正
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

 *　SW0		PA7		CN10-15
 *　SW1		PA8		CN10-23

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


#define Display_Enable  0x4		// ダイナミック点灯の為の表示フラグ
#define LP_Count_Enable 0x8 	// スイッチ長押し有効を示すフラグ

#define SW0 0b0000000010000000	//SW0(PA7)用のマスク値
#define SW1 0b0000000100000000	//SW0(PA8)用のマスク値
#define SW_OFF 0x0
//#define start_flg 0b10000

#define Divide_Count 1000		// SysTick タイマーの割込み間隔の係数（1 ms 用）
#define LP_Interval  1000		// スイッチ長押し処理開始までの時間間隔（1 ms 単位）


//TIMによるブザー音階データ
#define C2 134 //ど
#define D2 151 //れ
#define E2 170 //み
#define F2 179 //ふぁ
#define G2 201 //そ
#define A2 226 //ら
#define B2 254 //し
#define C3 267 //ど
#define D3 300 //れ
#define E3 336 //み
#define F3 353 //ふぁ
#define G3 396 //そ
#define A3 444 //ら
#define B3 498 //し
#define C4 523 //ど
#define D4 587 //れ
#define E4 659 //み
#define F4 698 //ふぁ
#define G4 783 //そ
#define A4 880 //ら
#define B4 987 //し
#define C5 1046 //ど
#define D5 1174 //れ
#define E5 1315 //み
#define F5 1392 //ふぁ
#define G5 1562 //そ
#define A6 1751 //ら
#define B6 1964 //し
#define C6 2063 //ど
#define D6 2318 //れ
#define E6 2596 //み
#define F6 2727 //ふぁ
#define G6 3059 //そ

#define S4 1015 //し#
#define P4 732 //ふぁ#
#define O6 2985

#define LEN1	500

#define LEN2	250 // LEN1の1/2倍
#define LEN3	750 // LEN1の1.5倍
#define LEN4	125 // LEN1の1/4倍
#define LEN5	100 // LEN1の1/5倍
#define LEN6	63  // LEN1の1/6倍


//音符データの構造体
typedef struct{
	int pitch;
	int duration;
} Note;

//=======================< グローバル・データ定義 >============================

unsigned int Up   = 0;	// 表示タイミング及びスイッチ押下を示すフラグ変数
int Switch_ID     = 0;	// 現在長押し中のスイッチ（0:なし、1:スイッチ０、2:スイッチ１）
int LP_Count_Flag = 0;	// スイッチ長押し時のカウントアップタイミングを示すフラグ変数




//グローバル変数
int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）

int m_TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）


int pattern_num;//表示させたい8x8LEDマトリクスのフォントデータの番号

int music_play = 0; //1でsystick内で音を鳴らす
int m_time = 0;
int melody_number = 0;
int m_i = 0;
//unsigned int act_flg = 1;----うまく動作しない
/*-----act_flg-----*/
/*  0: 動かない              */
/*  1: ptr,PTRが動く  */
/*  2: bee,BEEが動く  */
/*-----------------*/

int start_flg = 0;
int Bee_flg   = 0;
int ptr_flg   = 0;
int btn_flg   = 0;
int btn_cnt_start = 0;

int change_flg   = 0;
int change_flg2  = 0;
int change_flg_a = 0;
int change_flg_b = 1;

int time_data = 0;	//時間データ

unsigned int cnt_flg   = 0;

unsigned int cnt_flg2  = 0;

unsigned int cnt_flg3  = 0;

unsigned int cnt_flg4  = 0;

unsigned int cnt_flg5  = 0;

unsigned int cnt_flg6  = 0;

unsigned int cnt_flg_s = 0; //横向きの時の状態を保存する

unsigned int cnt_flg_p = 0; //正面の時の状態を保存する

unsigned int wait_flg  = 0;


//8x8 LEDマトリクスのフォントデータ

uint8_t face[]={0x0,0x22,0x77,0x22,0x0,0x8,0x22,0x1C};		//顔
uint8_t invader[]={0x81,0x42,0x3C,0x7E,0xDB,0x7E,0x42,0xC3};//インベーダ
uint8_t cat[]={0x22,0x66,0xFF,0x99,0xBB,0xFF,0x7E,0x00};		//猫
uint8_t apple[]={0xC,0x8,0x7E,0xDF,0xBF,0xBF,0xDF,0x7E};	//リンゴ
uint8_t bee[]={0x40,0xB1,0x3F,0xF,0x6,0x3E,0x7C,0x38};      //蜂
uint8_t BEE[]={0x40,0xB0,0x3E,0xE,0x7,0x3F,0x7D,0x38};      //蜂2
uint8_t mantis[]={0x0,0xC0,0xB0,0x3E,0x4F,0xC7,0x9B,0x91};  //蟷螂
uint8_t coconutcrab[]={0x24,0x24,0x5A,0xDB,0x3C,0x7E,0xBD,0xBD};  //ヤシガニ
uint8_t ptr[]={0x7,0x2F,0x6E,0xFC,0x19,0x1F,0x6,0x0};
uint8_t ptr2[]={0x0,0x20,0x6E,0xFC,0x1D,0x1F,0x6,0x0};
uint8_t PTR[]={0x20,0x60,0xF8,0x19,0x3F,0x76,0xF0,0xE0};
uint8_t PTR2[]={0x20,0x60,0xF8,0x19,0x3F,0x76,0x0,0x0};

uint8_t ptr_s[]={0x42,0x89,0x99,0xB9,0xCB,0x7E,0x18,0x0};
uint8_t ptr2_s[]={0x0,0x8,0x99,0xB9,0xCB,0x7E,0x18,0x0};
uint8_t PTR_s[]={0x8,0x18,0x38,0x8,0x7E,0xDB,0x81,0x42};
uint8_t PTR2_s[]={0x8,0x18,0x38,0x8,0x7E,0xDB,0x0,0x0};

uint8_t ptr_0[]={0x7,0x2F,0x6F,0xFE,0x1D,0x1F,0x6,0x0};
uint8_t ptr2_0[]={0xF,0x1B,0x37,0x7E,0x1D,0x1F,0xC,0x0};
uint8_t PTR_0[]={0x13,0x2B,0x79,0x7B,0x6E,0x3D,0xE,0x0};
uint8_t PTR2_0[]={0x23,0x69,0xDB,0xFB,0x4E,0x3C,0xE,0x0};

uint8_t ptr_1[]={0x0,0x2C,0x6F,0xFE,0x19,0x1F,0x6,0x0};
uint8_t ptr2_1[]={0x0,0x18,0x37,0x7E,0x1D,0x1F,0xC,0x0};
uint8_t PTR_1[]={0x0,0x8,0xD9,0x7B,0x6E,0x3D,0xE,0x0};
uint8_t PTR2_1[]={0x0,0x8,0xD9,0xFB,0x4E,0x3C,0xE,0x0};

uint8_t ptr_2[]={0x20,0x60,0xF8,0x39,0x7F,0xF6,0x0,0x0};
uint8_t ptr2_2[]={0x10,0x30,0x78,0x1D,0x6E,0xEF,0x0,0x0};
uint8_t PTR_2[]={0x8,0x18,0x38,0xC,0x3F,0xED,0x0,0x0};
uint8_t PTR2_2[]={0x8,0x18,0x38,0xC,0x7F,0xCD,0x0,0x0};

uint8_t ptr_3[]={0x20,0x60,0xF8,0x19,0x7F,0xFE,0xF8,0xF0};
uint8_t ptr2_3[]={0x10,0x30,0x78,0x1D,0x7F,0xEE,0xDC,0xB8};
uint8_t PTR_3[]={0x8,0x18,0x38,0xD,0x7F,0xCF,0xC6,0x84};
uint8_t PTR2_3[]={0x8,0x18,0x38,0xC,0x7E,0xCF,0x83,0x82};

uint8_t test_bee[]={0x41,0xB3,0x3F,0xE,0xC,0x7C,0xF8,0x70};      //test蜂
uint8_t test_BEE[]={0x40,0xB0,0x3C,0xC,0xE,0x7F,0xFB,0x71};      //test蜂2

//楽曲の音符データ
Note melody[][20]={
		{{B4,LEN1},{G4,LEN1},{D4,LEN1},{G4,LEN1},{A4,LEN1},{D5,LEN1*2},
				{D4,LEN1},{A4,LEN1},{B4,LEN1},{A4,LEN1},{D4,LEN1},{G4,LEN1*2},{0,0}},
		{{C4,LEN1},{C4,LEN1},{G4,LEN1},{G4,LEN1},{A4,LEN1},{A4,LEN1},{G4,LEN1*2},
				{F4,LEN1},{F4,LEN1},{E4,LEN1},{E4,LEN1},{D4,LEN1},{D4,LEN1},{C4,LEN1*2},{0,0}},
				{{C4,LEN1},{D4,LEN1},{E4,LEN1},{F4,LEN1},{G4,LEN1},{A4,LEN1},{B4,LEN1},{C5,LEN1},{D5,LEN1*2},{0,0}},
				{{C3,LEN4},{A3,LEN4},{0,0}},
				{{D3,LEN5},{F2,LEN5},{0,0}},
				{{C3,LEN5},{C4,LEN5},{C5,LEN5},{0,0}},
				{{E3,LEN4},{E4,LEN4},{E5,LEN4},{E6,LEN4},{0,0}},
				{{C2,LEN1},{D2,LEN1},{E2,LEN1},{F2,LEN1},{G2,LEN1},{A2,LEN1},{B2,LEN1},{0,0}} //低い音のテスト
		};

Note* melody_address;	//演奏させる曲の楽譜データの先頭アドレス

//Note m_data[];




//ある時点でLEDマトリクスに表示させたいパターンのフォントデータの先頭アドレス
uint8_t *font_pattern_data;



//スイッチ関係の変数
int up=0;		//スイッチの立ち上がり検出フラグ、0で初期化
int sw_now=0;	//現在のスイッチ押下状態、0で初期化





//関数のプロトタイプ宣言
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handlerを使う待ち時間処理

//ブザー出力関係
void play_tone(unsigned int frequency);
void stop_tone();
int play_music(Note data[]);


//send_data_to_sr.cに記述された関数群
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

extern void Flash_SR(void);





//
//メイン関数
//



//main()内のwhileループ内で、
//font_pattern_dataに表示させたいフォントデータ領域の先頭アドレスをセットすると、
//LEDマトリクス上にそのパターンが表示される。
//
//Wait_Timeで指定される時間間隔で、LEDマトリクス表示が更新され、その動作が繰り返される。
//
//絵柄をスイッチの状態に応じて変化させる。




int main(void)
{

	my_init_system();//ハードウェアの初期化

	//Up = Up | start_flg;



/*
	//初期画面
	switch (cnt_flg) {
	case 0:
		font_pattern_data=bee;
		break;
	case 1:
		font_pattern_data=BEE;
	}
*/
/* 音を鳴らすテスト  */
//melody_number = 4;
//music_play = 1;
//while(1);
	while(1) {

		//初期画面
		if (!start_flg) {
			Bee_flg    = 0;
			ptr_flg    = 0;
			change_flg = 1;

			if (change_flg2){
				switch(cnt_flg_p){
				case 0:
				{
					switch(cnt_flg5){
					case 0:
						font_pattern_data=PTR_0;
						break;
					case 1:
						font_pattern_data=PTR2_0;
						break;
					case 2:
						font_pattern_data=ptr2_0;
						break;
					case 3:
						font_pattern_data=ptr_0;
						change_flg2 = 0;
						change_flg_b = 1;
						break;
					}
					break;
				}

				case 1:
				{
					switch(cnt_flg5){
					case 0:
						font_pattern_data=PTR_1;
						break;
					case 1:
						font_pattern_data=PTR2_1;
						break;
					case 2:
						font_pattern_data=ptr2_1;
						break;
					case 3:
						font_pattern_data=ptr_1;
						change_flg2 = 0;
						change_flg_b = 2;
						break;
					}
					break;
				}

				case 2:
				{
					switch(cnt_flg5){
					case 0:
						font_pattern_data=PTR_2;
						break;
					case 1:
						font_pattern_data=PTR2_2;
						break;
					case 2:
						font_pattern_data=ptr2_2;
						break;
					case 3:
						font_pattern_data=ptr_2;
						change_flg2 = 0;
						change_flg_b = 3;
						break;
					}
					break;
				}

				case 3:
				{
					switch(cnt_flg5){
					case 0:
						font_pattern_data=PTR_3;
						break;
					case 1:
						font_pattern_data=PTR2_3;
						break;
					case 2:
						font_pattern_data=ptr2_3;
						break;
					case 3:
						font_pattern_data=ptr_3;
						change_flg2 = 0;
						change_flg_b = 4;
						break;
					}
					break;
				}

				}
			}else{
				switch(cnt_flg6){
				case 0:
					font_pattern_data=ptr;
					break;
				case 1:
					font_pattern_data=ptr2;
					break;
				case 2:
					font_pattern_data=PTR2;
					break;
				case 3:
					font_pattern_data=PTR;
					melody_number = 3;
					music_play = 1;
					break;
				}
			}
		}

		if (Bee_flg) {
			start_flg    = 1;
			change_flg   = 0;
			change_flg2  = 0;
			change_flg_a = 1;
			change_flg_b = 1;
			ptr_flg      = 0;

			switch (cnt_flg2) {
			case 0:
				font_pattern_data=test_bee;
				break;
			case 1:
				font_pattern_data=test_BEE;
				melody_number = 4;
				music_play = 1;
				break;
			}
		}

		if (ptr_flg){
			if (change_flg){
				switch(cnt_flg_s){
				case 0:
				{
					switch(cnt_flg3){
					case 0:
						font_pattern_data=ptr_0;
						melody_number = 6;
						music_play = 1;
						break;
					case 1:
						font_pattern_data=ptr2_0;
						break;
					case 2:
						font_pattern_data=PTR2_0;
						break;
					case 3:
						font_pattern_data=PTR_0;
						change_flg = 0;
						change_flg_a = 1;
						break;
					}
					break;
				}

				case 1:
				{
					switch(cnt_flg3){
					case 0:
						font_pattern_data=ptr_1;
						melody_number = 6;
						music_play = 1;
						break;
					case 1:
						font_pattern_data=ptr2_1;
						break;
					case 2:
						font_pattern_data=PTR2_1;
						break;
					case 3:
						font_pattern_data=PTR_1;
						change_flg = 0;
						change_flg_a = 2;
						break;
					}
					break;
				}

				case 2:
				{
					switch(cnt_flg3){
					case 0:
						font_pattern_data=ptr_2;
						melody_number = 6;
						music_play = 1;
						break;
					case 1:
						font_pattern_data=ptr2_2;
						break;
					case 2:
						font_pattern_data=PTR2_2;
						break;
					case 3:
						font_pattern_data=PTR_2;
						change_flg = 0;
						change_flg_a = 3;
						break;
					}
					break;
				}

				case 3:
				{
					switch(cnt_flg3){
					case 0:
						font_pattern_data=ptr_3;
						melody_number = 6;
						music_play = 1;
						break;
					case 1:
						font_pattern_data=ptr2_3;
						break;
					case 2:
						font_pattern_data=PTR2_3;
						break;
					case 3:
						font_pattern_data=PTR_3;
						change_flg = 0;
						change_flg_a = 4;
						break;
					}
					break;
				}

				}
			}else{
				change_flg2 = 1;
				switch(cnt_flg4){
				case 0:
					font_pattern_data=ptr_s;
					break;
				case 1:
					font_pattern_data=ptr2_s;
					break;
				case 2:
					font_pattern_data=PTR2_s;
					break;
				case 3:
					font_pattern_data=PTR_s;
					melody_number = 3;
					music_play = 1;
					break;
				}
			}
		}

	if (Up & LP_Count_Enable) {			// 長押し処理が有効である間は
		if (LP_Count_Flag) {			// 長押し時のカウント間隔フラグ
			LP_Count_Flag = 0;			// が立ったら、まずリセットして
			//act_flg = 0;
			switch (Switch_ID) {		// スイッチに応じて処理を行う
			case 1:							// スイッチ０の長押しならば
				//act_flg = 1;
				start_flg = 0;
				break;
			case 2:							// スイッチ１の長押しならば
				//act_flg = 2;
				Bee_flg = 1;
			}
		}
	} else {
			switch(Up){
			case SW0:
				Up &= ~SW0;					// スイッチ０の押下フラグをリセット
				//act_flg = 0;
				start_flg  = 1;
				Bee_flg    = 0;
				ptr_flg = 1;
				break;
			case SW1:
				Up &= ~SW1;					// スイッチ１の押下フラグをリセット
				//act_flg = 0;
				start_flg    = 1;
				ptr_flg      = 0;
				Bee_flg      = 0;
				change_flg   = 0;
				change_flg2  = 0;
				change_flg_a = 1;
				change_flg_b = 1;
				font_pattern_data=invader;
				melody_number = 5;
				music_play = 1;
			}

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
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPBEN;	// GPIOA のクロックを有効にする

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	//ポートAの入出力設定

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;

	GPIOB->CRL=0x4444444B;

    // PWMモードを選択し、PWM信号をピンに出力する
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // プリスケーラを設定する
    TIM3->PSC = 720-1;

    // タイマを有効にするコントロールレジスタ。　タイマのON・OFFなど設定
    TIM3->CR1 |= TIM_CR1_CEN;
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



//
//シリアル・パラレル変換回路に送り出すデータを生成する関数
//
//SysTick_Handler内で呼び出される
//
/*
 * with_making_selected_column_data
 *
 *
 *
 */

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




void Wait_Time(int time)
{
	TimeInt=time;
	while(!wait_flg && TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}





/*
**	get_sw --- 単純に現在のスイッチの値を正論理（アクティブハイ）で取得する関数
*/

int get_sw(void)
{
	int sw;

	sw = GPIOA->IDR;		// 現在のスイッチの値を読み込む
	sw = ~sw;				// 負論理なのでビットを反転する
	sw = sw & (SW0|SW1);	// スイッチ以外のビットをマスクする

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

void play_tone(unsigned int frequency) {
    // 自動リロードレジスタと比較レジスタを周波数に応じて設定する
    TIM3->ARR = (72000000 / frequency / 720) - 1;
    TIM3->EGR = TIM_EGR_UG;  // 追加: レジスタの変更をすぐに反映する
    TIM3->CCR3 = (TIM3->ARR + 1) / 2;  // 50% デューティサイクル
}

void stop_tone() {
    // PWM信号を停止する
    TIM3->CCR3 = 0;
}




int play_music(Note data[]){
	int i=0;
	int return_value = 0;
	while(data[i].pitch!=0){
		play_tone(data[i].pitch);
		Wait_Time(data[i].duration);
		i++;
		time_data=i;
		stop_tone();
	}
	stop_tone();
	if(wait_flg) wait_flg = 0;
	return Up | return_value;
}












//
// 割り込み間隔はSysTick_Config()の引数を
// RCC_Clocks.HCLK_Frequency / 1000　として、1msec
//

void SysTick_Handler(void)
{
//	static unsigned int cnt  = 0;
	static unsigned int cnt2 = 0;
	static unsigned int cnt3 = 0;
	static unsigned int cnt4 = 0;
	static unsigned int cnt5 = 0;
	static unsigned int cnt6 = 0;

//	static unsigned int btn_cnt = 0;

//	static unsigned int act_flg  = 0;
	static unsigned int act_flg2 = 0;
	static unsigned int act_flg3 = 0;



	static unsigned int swc  = 0;	// 今回読み込んだスイッチの値
	static unsigned int swp1 = 0;	// 前回読み込んだスイッチの値
	static unsigned int swp2 = 0;	// 前々回に読み込んだスイッチの値
	static unsigned int now  = 0;	// 今回（現在）のスイッチの確定値
	static unsigned int last = 0;	// 前回のスイッチの確定値

	static int chat_count    = 0;	// チャタリング除去用のカウンタ変数

	static int lp_start   = 0;	// スイッチ長押し処理開始フラグ
	static int lp_counter = 0;	// スイッチ長押し有効化までの時間（1 秒）を測るカウンタ
	static int lp_int_cnt = 0;	// スイッチ長押し時のタイミング（0.1 秒）生成用カウンタ


/* チャタリング除去及び立上り（スイッチの押下）検出 */

	if (chat_count == 9) {

		chat_count = 0;

		swp2 = swp1;		// 前々回のスイッチの読み取り値を保存する
		swp1 = swc;			// 前回のスイッチの読み取り値を保存する
		swc  = get_sw();	// 新たに今回のスイッチの値を読み込む

		if ((swp2 == swp1) && (swp1 == swc)) {	// 今回、前回、前々回の値が全て等しい
			now = swc;							// 場合、今回の値を現在の確定値とする
		}

		if (now != last) {			// 前回のスイッチの値と異なる場合だけ以下の処理を行う
			if (now & ~last) {		// 立上り（いずれかのスイッチの押下）を検出したら
				Up |= now & ~last;	// スイッチ（０または１）押下のフラグを立てる

			/* スイッチ長押しの開始処理 */

				if (Up & SW0) {			// スイッチ０が押されたら、
					if (!Switch_ID) {	// 長押しスイッチＩＤを確認し、０であれば、
						Switch_ID = 1;	// 長押しスイッチＩＤを１に設定する
					}
				}
				if (Up & SW1) {			// スイッチ１が押されたら、
					if (!Switch_ID) {	// 長押しスイッチＩＤを確認し、０であれば、
						Switch_ID = 2;	// 長押しスイッチＩＤを２に設定する
					}
				}
				lp_start   = 1;		// スイッチの長押し時間のカウントを有効にする
				lp_counter = 0;		// スイッチの長押し時間カウンタを０にリセットする
				lp_int_cnt = 0;		// 長押し時のカウント・タイミング用カウンタを０に
									// リセットする
			}
			last = now;				// 現在のスイッチの値を last に保存する
		}


	/* スイッチ長押しの終了処理 */

		if (Switch_ID) {			// 長押しスイッチが確定している場合
			switch (Switch_ID) {	// そのスイッチに応じて解除を行う
			case 1:						// スイッチ０が長押しスイッチの場合に
				if (!(now & SW0)) {		// スイッチ０が放された（オフになった）ら
					lp_start = 0;		// スイッチの長押し時間カウントのフラグをオフし
					Up &= ~LP_Count_Enable;	// 長押し処理有効のフラグをオフにし
					Switch_ID = 0;			// 長押しスイッチＩＤを０にリセットする
				}
				break;
			case 2:						// スイッチ１が長押しスイッチの場合に
				if (!(now & SW1)) {		// スイッチ１が放された（オフになった）ら
					lp_start = 0;		// スイッチの長押し時間カウントのフラグをオフし
					Up &= ~LP_Count_Enable;	// 長押し処理有効のフラグをオフにし
					Switch_ID = 0;			// 長押しスイッチＩＤを０にリセットする
				}
			}
		}

	} else {

		chat_count++;

	}

/* スイッチ長押しの有効化チェック */

	if (lp_start) {						// スイッチの長押し時間カウントのフラグが有効な場合は
		if (lp_counter < LP_Interval) {	// 指定された時間（１秒）までカウントを続け
			lp_counter++;
		} else {						// 指定された時間（１秒）が満了したら
			Up |= LP_Count_Enable;		// 長押し処理有効のフラグを立てる
		}
	}

/* スイッチ長押し有効時のカウントアップのタイミング生成 */

	if (Up & LP_Count_Enable) {		// 長押し処理が有効な間は
		if (lp_int_cnt < 99) {		// 指定された間隔（0.1秒）
			lp_int_cnt++;			// までカウントアップして
		} else {					// 時間（0.1秒）が満了したら
			lp_int_cnt = 0;			// カウンタを０にリセットして
			LP_Count_Flag = 1;		// 長押し時のカウント間隔の
		}							// タイミングのフラグをセットする
	}





	//
	//待ち時間関数　Wait_Time()用の変数の処理
	//
	if(TimeInt>0) {
		if(Up) {
			wait_flg = 1;
		}else
			TimeInt--;
	}

	if(m_TimeInt>0) {//音楽用待ち時間カウント
		if(Up) {
			wait_flg = 1;
		}else
			m_TimeInt--;
	}


	//
	//シリアル・パラレル変換回路へデータを送出（LEDマトリクスが表示される）

	//Send_Word_To_SR_From_MSB(Setup_Serial_Data(font_pattern_data));

	//Send_Word_To_SR(Setup_Serial_Data(font_pattern_data));

	//2023/09/12 改定
	//Send_Word_To_SR_From_MSB(Setup_Serial_Data_2(font_pattern_data));

	Send_Word_To_SR(Setup_Serial_Data_2(font_pattern_data));



	if (change_flg && ptr_flg) {
		if (cnt3 < LP_Interval/8) {
				cnt3++;
		}else{
			if (cnt_flg3 == 0) {
				cnt_flg3 = 1;
				cnt3 = 0;
			}else {
				if (cnt_flg3 == 1) {
					cnt_flg3 = 2;
					cnt3 = 0;
				}else {
					if (cnt_flg3 == 2) {
						cnt_flg3 = 3;
						cnt3 = 0;
					}else {
						if (cnt_flg3 == 3) {
							change_flg = 0;
							cnt3 = 0;
						}
					}
				}
			}
		}
	}
	if (!change_flg && ptr_flg) {
		cnt_flg3 = 0;
		if (cnt4 < LP_Interval/10) {
				cnt4++;
		}else{
			if(act_flg2 == 0){

			if (change_flg_a == 1){

				if (cnt_flg4 == 0) {
					cnt_flg4 = 1;
					cnt4 = 0;
				}else {
					if (cnt_flg4 == 1) {
						cnt_flg4 = 2;
						cnt4 = 0;
					}else {
						if (cnt_flg4 == 2) {
							cnt_flg4 = 3;
							cnt4 = 0;
						}else {
							if (cnt_flg4 == 3) {
								act_flg2 = 1;
								cnt4 = 0;
							}
						}
					}
				}
			}

			if (change_flg_a == 2){
				if ((cnt_flg4 != 2) & (cnt_flg4 != 3)) {
					cnt_flg4 = 2;
					cnt4 = 0;
				}else {
					if (cnt_flg4 == 2) {
						cnt_flg4 = 3;
						cnt4 = 0;
					}else {
						if (cnt_flg4 == 3) {
							act_flg2 = 1;
							cnt4 = 0;
						}
					}
				}
			}

			if (change_flg_a == 3){
				if (cnt_flg4 != 3) {
					cnt_flg4 = 3;
					cnt4 = 0;
				}else {
					if (cnt_flg4 == 3) {
						act_flg2 = 1;
						cnt4 = 0;
					}
				}
			}

			if (change_flg_a == 4){
					cnt_flg4 = 3;
					act_flg2 = 1;
					cnt4 = 0;
			}
			}else {
				change_flg_a = 1;
				if (cnt_flg4 == 3) {
					cnt_flg4 = 2;
					cnt4 = 0;
				}else {

					if (cnt_flg4 == 2) {
						cnt_flg4 = 1;
						cnt4 = 0;
					}else {

						if (cnt_flg4 == 1) {
							cnt_flg4 = 0;
							cnt4 = 0;
						}else {

							if (cnt_flg4 == 0) {
								act_flg2 = 0;
								cnt4 = 0;
							}
						}
					}
				}
			}
		}
		cnt_flg_p =cnt_flg4;
	}else{
		cnt4 = 0;
	}


	if (change_flg2 && !start_flg) {
		if (cnt5 < LP_Interval/8) {
				cnt5++;
		}else{
			if (cnt_flg5 == 0) {
				cnt_flg5 = 1;
				cnt5 = 0;
			}else {
				if (cnt_flg5 == 1) {
					cnt_flg5 = 2;
					cnt5 = 0;
				}else {
					if (cnt_flg5 == 2) {
						cnt_flg5 = 3;
						cnt5 = 0;
					}else {
						if (cnt_flg5 == 3) {
							change_flg2 = 0;
							cnt5 = 0;
						}
					}
				}
			}
		}
	}
	if (!change_flg2 && !start_flg) {
		cnt_flg5 = 0;
		if (cnt6 < LP_Interval/10) {
				cnt6++;
		}else{
			if(act_flg3 == 0){

			if (change_flg_b == 1){

				if (cnt_flg6 == 0) {
					cnt_flg6 = 1;
					cnt6 = 0;
				}else {
					if (cnt_flg6 == 1) {
						cnt_flg6 = 2;
						cnt6 = 0;
					}else {
						if (cnt_flg6 == 2) {
							cnt_flg6 = 3;
							cnt6 = 0;
						}else {
							if (cnt_flg6 == 3) {
								act_flg3 = 1;
								cnt6 = 0;
							}
						}
					}
				}
			}

			if (change_flg_b == 2){
				if ((cnt_flg6 != 2) & (cnt_flg6 != 3)) {
					cnt_flg6 = 2;
					cnt6 = 0;
				}else {
					if (cnt_flg6 == 2) {
						cnt_flg6 = 3;
						cnt6 = 0;
					}else {
						if (cnt_flg6 == 3) {
							act_flg3 = 1;
							cnt6 = 0;
						}
					}
				}
			}

			if (change_flg_b == 3){
				if (cnt_flg6 != 3) {
					cnt_flg6 = 3;
					cnt6 = 0;
				}else {
					if (cnt_flg6 == 3) {
						act_flg3 = 1;
						cnt6 = 0;
					}
				}
			}

			if (change_flg_b == 4){
					cnt_flg6 = 3;
					act_flg3 = 1;
					cnt6 = 0;
			}
			}else {
				change_flg_b = 1;
				if (cnt_flg6 == 3) {
					cnt_flg6 = 2;
					cnt6 = 0;
				}else {

					if (cnt_flg6 == 2) {
						cnt_flg6 = 1;
						cnt6 = 0;
					}else {

						if (cnt_flg6 == 1) {
							cnt_flg6 = 0;
							cnt6 = 0;
						}else {

							if (cnt_flg6 == 0) {
								act_flg3 = 0;
								cnt6 = 0;
							}
						}
					}
				}
			}
		}
		cnt_flg_s = cnt_flg6;
	}else{
		cnt_flg6 = 0;
		cnt6 = 0;
	}


///	if (act_flg == 2) {
		if (cnt2 < LP_Interval/10) {
			cnt2++;
		}else{

			if (cnt_flg2 == 0) {
				cnt_flg2 = 1;
				cnt2 = 0;
			}else {

				if (cnt_flg2 == 1) {
					cnt_flg2 = 0;
					cnt2 = 0;
				}
			}
		}

//	}
	//music
		if(music_play == 1)  //演奏中
		    if (( m_TimeInt==0)||(wait_flg==1)){
				if (melody[melody_number][m_i].pitch!=0){
					stop_tone();
					play_tone(melody[melody_number][m_i].pitch);
					//Wait_Time(data[i].duration);
					m_time = melody[melody_number][m_i].duration;
					m_TimeInt=m_time;
					//while(!wait_flg && TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。
					m_i++;
					time_data=m_i;
					//stop_tone();
				}else{
					stop_tone();
					m_i =0;
					music_play = 0;
					if(wait_flg) wait_flg = 0;
					//return Up | return_value;
				}
		    }
}
