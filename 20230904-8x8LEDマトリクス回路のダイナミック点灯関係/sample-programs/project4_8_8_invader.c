
/*LEDマトリックスシューティングゲーム
 *
 * 電源を入れた直後にOpeningが起動
 * Openingが終了したら、戦闘モードへ
 * SW1を押すと上へ、SW2を押すと下へ移動する。枠外に出た場合はローテーションする（下へはみ出ると上から出てくる）
 * SW1とSW2を両方同時に押すと、弾が発射する
 *
 * ・First_Stage
 * 壁をすべて破壊すると、Second_Stageへ
 * ・Second_Stage
 * 動く敵をすべて破壊すると、Finish
 *　元のFirst_Stageへ戻る
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* マクロ定義　*/
#define ROW_SIZE 8	//行数
#define SW1 0x1
#define SW2 0x2
#define Switch_On 0x8

#define Delay_Count 800
#define Basic_Time 200

#define C4 523 //ど
#define D4 587 //れ
#define E4 659 //み
#define F4 698 //ふぁ
#define G4 783 //そ
#define A4 880 //ら
#define B4 987 //し
#define C5 1046 //ど
#define D5 1174 //れ
#define bom_tune 3000
#define destory 100
#define LEN1	50


//グローバル変数



int TimeInt=0;	//delay処理（Waitする間隔を指定するため）のグローバル変数（SysTickで減算）

int pattern_num = 0;//表示させたい8x8LEDマトリクスのフォントデータの番号
int count_flag =0;
int set_time =10; //ルーレットの回転スピード

int up=0;		//スイッチの立ち上がり検出フラグ、0で初期化
int sw_now=0;	//現在のスイッチ押下状態、0で初期化
int Bom_Flag =0; //SW1,SW2スイッチが両方押された場合に、弾を発射するフラグ変数
int Bom_Tune_Flag=0;
int Bom_Count =0; //弾が無限にシフトローテーションしないように弾の移動をカウントする変数
int enemy_move =0; //敵キャラのシフトローテーションを制御する変数
int enemy_number =0; //壁、敵を倒したかどうか確認する変数
int second_flag =0; //First.Secondステージを制御する変数
int rate_count=0;
int time_count=0;
int time_data =0;

int stage_clear_music = 4; //melody配列のスタート位置
//音符データの構造体
typedef struct{
	int pitch;
	int duration;
} Note;

Note* melody_address;	//演奏させる曲の楽譜データの先頭アドレス

//楽曲の音符データ
Note melody[][20]={
		{{bom_tune,LEN1}}, //ボムの発射
		{{destory,LEN1}}, //敵の破壊
		{{C4,LEN1}},{{E4,LEN1}}, //オープニング
		{{D4,LEN1}},{{A4,LEN1}},{{B4,LEN1}},{{A4,LEN1}},{{D4,LEN1}},{{G4,LEN1*2}}, //クリア
		{{C5,LEN1}},{{D5,LEN1}}, //敵の出現（セカンドステージ）

};


//8x8 LEDマトリクスのフォントデータ

//Openingの際に起動する描写を格納する配列
uint8_t start[][ROW_SIZE]={
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
};

uint8_t invader[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x80,0xC0,0x80},
};//飛行機

uint8_t bom[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x00},
};//ボム


uint8_t kabe[][ROW_SIZE]={
		{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};//壁

uint8_t kabe_stock[][ROW_SIZE]={
		{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};//壊された壁を補充する配列

uint8_t enemy[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
};//敵

uint8_t enemy_stock[][ROW_SIZE]={
		{0x1,0x1,0x0,0x0,0x1,0x1,0x0,0x0},
};//倒された敵を補充する配列

uint8_t second_stage[][ROW_SIZE]={
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},

};//SecondStageへ移行する際に動くキャラ描写

uint8_t finish[][ROW_SIZE]={
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},


};//敵を全滅させた場合に出る描写


uint8_t judge[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x00,0x0},
};//判定


uint8_t Result[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x00,0x0},
};//全体

//ある時点でLEDマトリクスに表示させたいパターンのフォントデータの先頭アドレス
uint8_t *font_pattern_data;


//関数のプロトタイプ宣言

void my_init_system(void); //ハードウェアの初期設定
void Opening(); //電源入力直後の挙動を記載
void Second_Stage(); //壁をすべて破壊した後の挙動を記載
void Finish(); //Second_Stageの敵をすべて破壊した後の挙動を記載
void UpMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row); //SW1スイッチを押すと上に動く
void DownMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row); //SW2スイッチを押すと上に動く
void Bom(uint8_t bom[][8]); ////SW1スイッチ、SW2スイッチ同時に押すと弾が発射される挙動を記載
void Judge(); //弾が壁、敵に当たって際の判定を記載
unsigned int Get_SW(void); //スイッチが押された際の判定を記載
void play_music(Note data[]);
void play_tone(unsigned int frequency);

//send_data_to_sr.cに記述された関数群
void Wait_Time(int time);	//SysTick_Handlerを使う待ち時間処理
void Send_Byte_To_SR(uint8_t bytes);
void Send_Word_To_SR(uint16_t words);
void Send_Byte_To_SR_From_MSB(uint8_t bytes);
void Send_Word_To_SR_From_MSB(uint16_t words);
void Flash_SR(void);
uint16_t Setup_Serial_Data_2(uint8_t font_data[]);

//void shiftRowRight(uint8_t arr[][8], int row) ;


//メイン関数

int main(void)
{


	my_init_system();//ハードウェアの初期化

	Opening(); //電源入力直後の挙動を記載


	int temp;


	//font_pattern_dataに表示させたいフォントデータ領域の先頭アドレスをセットすると、
	//LEDマトリクス上にそのパターンが表示される。
	//Wait_Timeで指定される時間間隔で、LEDマトリクス表示が更新され、その動作が繰り返される。
	//

	while(1){

		// ビットを描画する為の準備。飛行機、弾、敵などのパーツを組み合わせてResultにすべてのパーツを反映。
		for(int i = 0; i < ROW_SIZE; ++i) {
			Result[0][i] = 0x0; //リセットを入れる。これがないと弾が壁に当たっても消える描写が描けない。
			Result[0][i] = invader[0][i] | bom[0][i] | enemy[0][i] | kabe[0][i];
		}


		//敵キャラが画面外に出た際にもう一度画面に戻るようにローテションするように設定。
		temp= enemy[0][0];

		//配列の列のサイズ（8）を繰り返し描画する。
		for(pattern_num=0; pattern_num<(sizeof invader)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){

			if(Bom_Flag==1){

				if(Bom_Count==8){
					Bom_Count=0;
					Bom_Flag=0;
					break;

				}else{
					Bom(bom);
					font_pattern_data=Result[pattern_num];	//表示させたいフォントデータの先頭アドレスを取得。これはSysTick_Handlerに渡される。
					Wait_Time(40);
					Bom_Count++;

				}
			}else{

				if(enemy_move==7){
					enemy_move=0;
				}else{
					enemy[0][enemy_move] = enemy[0][enemy_move + 1];
					enemy_move++;
				}


			font_pattern_data=Result[pattern_num];
			Wait_Time(100);	//待ち

			}
		}

		//敵キャラが画面外に出た際にもう一度画面に戻るようにローテションするように設定。
		enemy[0][7] = temp;

		if(up & Switch_On){
			up &= ~Switch_On;
			switch(sw_now){	//押されたスイッチの状態によって、表示を変える

			    //SW1,SW2スイッチが両方押されたら弾を発射する（Bom_Flagに1を立てる）
				case SW1|SW2:
				Bom_Flag=1;

				melody_address=melody[0];
				play_music(melody_address);

				break;

				//SW1スイッチが押されたら上に動く
				case SW1:
				UpMatrix(invader,bom, 0);
				//BuzzerOn(rate,b_time);
				break;

				//SW2スイッチが押されたら下に動く
				case SW2:
				DownMatrix(invader,bom, 0);
			}
		}


		//壁がすべて壊されたかどうかの確認。
		if(second_flag==0){
			for(int i = 0; i<ROW_SIZE; i++){
				enemy_number |= kabe[0][i] ;
			}

            //壁がすべて破壊されたら、Second_Stageへ移行する
			if(enemy_number==0){
				Second_Stage();
				second_flag =1;
				enemy_number =1;
					for(int i = 0; i<ROW_SIZE; i++){
						enemy[0][i] = enemy_stock[0][i];
					}
			}else{
				enemy_number =0;
			}

		//敵が全滅したかどうかの確認
		}else if(second_flag==1){
			for(int i = 0; i<ROW_SIZE; i++){
				enemy_number |= enemy[0][i] ;
			}

			//敵が全滅した場合にはFinish()を呼び出し、初期設定（壁の復元）に移行
			if(enemy_number==0){
				Finish();
				second_flag =0;
					for(int i = 0; i<ROW_SIZE; i++){
						kabe[0][i] = kabe_stock[0][i];
					}

			}else{
				enemy_number =0;
			}


		}
	}
}




//電源を入れた直後に動作する
void Opening(){

	for(pattern_num=0; pattern_num<(sizeof start)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[2];
		play_music(melody_address);
		font_pattern_data=start[pattern_num];	//表示させたいフォントデータの先頭アドレスを取得。これはSysTick_Handlerに渡される。
		Wait_Time(300);	//待ち
		melody_address=melody[3];
		play_music(melody_address);
	}

}

//壁をすべて壊した場合に動作する
void Second_Stage(){

	for(pattern_num=0; pattern_num<(sizeof second_stage)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[10];
		play_music(melody_address);
		font_pattern_data=second_stage[pattern_num];	//表示させたいフォントデータの先頭アドレスを取得。これはSysTick_Handlerに渡される。
		Wait_Time(300);	//待ち
		melody_address=melody[11];
		play_music(melody_address);
	}

}

//敵をすべて倒した場合にどうする
void Finish(){

	for(pattern_num=0; pattern_num<(sizeof finish)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[stage_clear_music];
		play_music(melody_address);
		font_pattern_data=finish[pattern_num];	//表示させたいフォントデータの先頭アドレスを取得。これはSysTick_Handlerに渡される。
		Wait_Time(500);	//待ち
		stage_clear_music++;
	}

	stage_clear_music = 4;

}


//SW1スイッチが押された場合に動作、上に動く
void UpMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row) {

	uint8_t temp;
	uint8_t temp1;

    temp= matrix[0][0];
    temp1= bom[0][0];

    for (int j = 0; j < 7; ++j) {
    	matrix[0][j] = matrix[0][j + 1];  // 右にシフト
    	bom[0][j] = bom[0][j + 1];

    }
    matrix[0][7] = temp;
    bom[0][7] = temp1;
}

//SW2スイッチが押された場合に動作、上に動く
void DownMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row) {

	uint8_t temp;
	uint8_t temp1;

    temp= matrix[0][7];
    temp1= bom[0][7];

    for (int j = 7; j > 0; --j) {
    	matrix[0][j] = matrix[0][j - 1];  // 右にシフト
    	bom[0][j] = bom[0][j - 1];

    }
    matrix[0][0] = temp;
    bom[0][0] = temp1;
}


//SW1スイッチとSW2スイッチが両方押された場合に動作、弾を発射する
void Bom(uint8_t bom[][8]){


	      // 3ビット目が1だった場合の特定の処理
	for (int i = 0; i < ROW_SIZE; ++i) {
		if(bom[0][i] == 0x1){
			bom[0][i] = 0x80;
		}else if(bom[0][i] == 0x2){
			Judge();
			bom[0][i] >>= 1;
		}else{
			bom[0][i] >>= 1;  // 右シフト
		}
	}


}



//弾が壁、敵に当たった際の判定
void Judge(){

	for(int i = 0; i < ROW_SIZE; ++i) {
		judge[0][i] = bom[0][i] | enemy[0][i] | kabe[0][i] ;
		if(judge[0][i]==0x3){
			enemy[0][i]=0x0;
			kabe[0][i]=0x0;
			melody_address=melody[1];
			play_music(melody_address);
		}
		judge[0][i] = 0x0 ;
	}

}


//ハードウェアの初期化
void my_init_system(void)
{


	/* クロック情報取得／設定の為の構造体定義 */
	RCC_ClocksTypeDef RCC_Clocks;

	/* システムのクロック情報の取得 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick タイマの設定 */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick タイマ割込みの間隔を 1ms に設定

	//GPIO　A,B,Cを有効化
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN|RCC_APB2ENR_IOPCEN;	// GPIOA のクロックを有効にする
	RCC->APB1ENR = RCC_APB1ENR_TIM3EN;

	GPIOB->CRL=0x4444444B;
	//ポートAの入出力設定

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;


    // PWMモードを選択し、PWM信号をピンに出力する
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // プリスケーラを設定する
    TIM3->PSC = 720-1;


    // タイマを有効にするコントロールレジスタ。　タイマのON・OFFなど設定
    TIM3->CR1 |= TIM_CR1_CEN;



}


//スイッチを読み込む
unsigned int Get_SW(void){

	unsigned int sw;
	sw= GPIOC ->IDR;
	sw = ~sw;
	sw =sw & (SW1|SW2);

	return sw;
}



void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//カウンタの値が0になるまで待つ。カウンタはSysTick_Handlerで減算されている。


}

void BuzzerOn(int rate, int time){
	int i;
	for(i =0;i<rate;i++){
		GPIOB->ODR =1;
		Wait_Time((time/rate)/2);
		GPIOB->ODR =0;
		Wait_Time((time/rate)/2);
	}

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

void play_music(Note data[]){
	int i=0;
//	while(data[i].pitch!=0){
		play_tone(data[i].pitch);
		Wait_Time(data[i].duration);
//		i++;
//		time_data=i;
		stop_tone();
//	}

}


void SysTick_Handler(void)
{


	static int swc  = 0;	// 今回読み込んだスイッチの値
	static int swp1 = 0;	// 前回読み込んだスイッチの値
	static int swp2 = 0;	// 前々回に読み込んだスイッチの値

	static int sw_last = 0;	// 前回のスイッチの確定値
	static int chat_count = 0;


	if(chat_count == 9){
		chat_count =0;

	swp2 = swp1;				// 前々回の値を保存する
	swp1 = swc;					// 前回の値を保存する
	swc  = Get_SW();			// 新たに今回のスイッチの値を読み込む

	if ((swp2 == swp1) && (swp1 == swc)) {	// 今回、前回、前々回の値が全て等しい
		sw_now = swc;							// 場合、今回の値を現在の確定値とする
	}

	if (sw_now != sw_last) {	// 前回のスイッチの確定値と異なる場合だけ以下の処理を行う
		if(sw_now & ~sw_last){
			up |= Switch_On;
		}
//		up = sw_now & ~sw_last;	// スイッチの立上り（何れかのスイッチが押されたか）を検出
		sw_last = sw_now;		// 現在のスイッチの確定値を sw_last に保存する
	}

	}else{

		chat_count++;
	}

	//
	//待ち時間関数　Wait_Time()用の変数の処理
	if(TimeInt>0) TimeInt--;

	//シリアル・パラレル変換回路へデータを送出（LEDマトリクスが表示される）

	Send_Word_To_SR(Setup_Serial_Data_2(font_pattern_data));


}








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



