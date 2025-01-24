
/*LED�}�g���b�N�X�V���[�e�B���O�Q�[��
 *
 * �d������ꂽ�����Opening���N��
 * Opening���I��������A�퓬���[�h��
 * SW1�������Ə�ցASW2�������Ɖ��ֈړ�����B�g�O�ɏo���ꍇ�̓��[�e�[�V��������i���ւ͂ݏo��Əォ��o�Ă���j
 * SW1��SW2�𗼕������ɉ����ƁA�e�����˂���
 *
 * �EFirst_Stage
 * �ǂ����ׂĔj�󂷂�ƁASecond_Stage��
 * �ESecond_Stage
 * �����G�����ׂĔj�󂷂�ƁAFinish
 *�@����First_Stage�֖߂�
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* �}�N����`�@*/
#define ROW_SIZE 8	//�s��
#define SW1 0x1
#define SW2 0x2
#define Switch_On 0x8

#define Delay_Count 800
#define Basic_Time 200

#define C4 523 //��
#define D4 587 //��
#define E4 659 //��
#define F4 698 //�ӂ�
#define G4 783 //��
#define A4 880 //��
#define B4 987 //��
#define C5 1046 //��
#define D5 1174 //��
#define bom_tune 3000
#define destory 100
#define LEN1	50


//�O���[�o���ϐ�



int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int pattern_num = 0;//�\����������8x8LED�}�g���N�X�̃t�H���g�f�[�^�̔ԍ�
int count_flag =0;
int set_time =10; //���[���b�g�̉�]�X�s�[�h

int up=0;		//�X�C�b�`�̗����オ�茟�o�t���O�A0�ŏ�����
int sw_now=0;	//���݂̃X�C�b�`������ԁA0�ŏ�����
int Bom_Flag =0; //SW1,SW2�X�C�b�`�����������ꂽ�ꍇ�ɁA�e�𔭎˂���t���O�ϐ�
int Bom_Tune_Flag=0;
int Bom_Count =0; //�e�������ɃV�t�g���[�e�[�V�������Ȃ��悤�ɒe�̈ړ����J�E���g����ϐ�
int enemy_move =0; //�G�L�����̃V�t�g���[�e�[�V�����𐧌䂷��ϐ�
int enemy_number =0; //�ǁA�G��|�������ǂ����m�F����ϐ�
int second_flag =0; //First.Second�X�e�[�W�𐧌䂷��ϐ�
int rate_count=0;
int time_count=0;
int time_data =0;

int stage_clear_music = 4; //melody�z��̃X�^�[�g�ʒu
//�����f�[�^�̍\����
typedef struct{
	int pitch;
	int duration;
} Note;

Note* melody_address;	//���t������Ȃ̊y���f�[�^�̐擪�A�h���X

//�y�Ȃ̉����f�[�^
Note melody[][20]={
		{{bom_tune,LEN1}}, //�{���̔���
		{{destory,LEN1}}, //�G�̔j��
		{{C4,LEN1}},{{E4,LEN1}}, //�I�[�v�j���O
		{{D4,LEN1}},{{A4,LEN1}},{{B4,LEN1}},{{A4,LEN1}},{{D4,LEN1}},{{G4,LEN1*2}}, //�N���A
		{{C5,LEN1}},{{D5,LEN1}}, //�G�̏o���i�Z�J���h�X�e�[�W�j

};


//8x8 LED�}�g���N�X�̃t�H���g�f�[�^

//Opening�̍ۂɋN������`�ʂ��i�[����z��
uint8_t start[][ROW_SIZE]={
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
};

uint8_t invader[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x80,0xC0,0x80},
};//��s�@

uint8_t bom[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x00},
};//�{��


uint8_t kabe[][ROW_SIZE]={
		{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};//��

uint8_t kabe_stock[][ROW_SIZE]={
		{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
};//�󂳂ꂽ�ǂ��[����z��

uint8_t enemy[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
};//�G

uint8_t enemy_stock[][ROW_SIZE]={
		{0x1,0x1,0x0,0x0,0x1,0x1,0x0,0x0},
};//�|���ꂽ�G���[����z��

uint8_t second_stage[][ROW_SIZE]={
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},
		{0x42,0x24,0x3C,0x5A,0xFF,0xFF,0xA5,0x3C},
		{0x42,0x24,0xBD,0xDB,0xFF,0x7E,0x24,0x42},

};//SecondStage�ֈڍs����ۂɓ����L�����`��

uint8_t finish[][ROW_SIZE]={
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
		{0x00,0x0,0x0,0x0,0x0,0x0,0x0,0x0},


};//�G��S�ł������ꍇ�ɏo��`��


uint8_t judge[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x00,0x0},
};//����


uint8_t Result[][ROW_SIZE]={
		{0x0,0x0,0x0,0x0,0x0,0x0,0x00,0x0},
};//�S��

//���鎞�_��LED�}�g���N�X�ɕ\�����������p�^�[���̃t�H���g�f�[�^�̐擪�A�h���X
uint8_t *font_pattern_data;


//�֐��̃v���g�^�C�v�錾

void my_init_system(void); //�n�[�h�E�F�A�̏����ݒ�
void Opening(); //�d�����͒���̋������L��
void Second_Stage(); //�ǂ����ׂĔj�󂵂���̋������L��
void Finish(); //Second_Stage�̓G�����ׂĔj�󂵂���̋������L��
void UpMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row); //SW1�X�C�b�`�������Ə�ɓ���
void DownMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row); //SW2�X�C�b�`�������Ə�ɓ���
void Bom(uint8_t bom[][8]); ////SW1�X�C�b�`�ASW2�X�C�b�`�����ɉ����ƒe�����˂���鋓�����L��
void Judge(); //�e���ǁA�G�ɓ������čۂ̔�����L��
unsigned int Get_SW(void); //�X�C�b�`�������ꂽ�ۂ̔�����L��
void play_music(Note data[]);
void play_tone(unsigned int frequency);

//send_data_to_sr.c�ɋL�q���ꂽ�֐��Q
void Wait_Time(int time);	//SysTick_Handler���g���҂����ԏ���
void Send_Byte_To_SR(uint8_t bytes);
void Send_Word_To_SR(uint16_t words);
void Send_Byte_To_SR_From_MSB(uint8_t bytes);
void Send_Word_To_SR_From_MSB(uint16_t words);
void Flash_SR(void);
uint16_t Setup_Serial_Data_2(uint8_t font_data[]);

//void shiftRowRight(uint8_t arr[][8], int row) ;


//���C���֐�

int main(void)
{


	my_init_system();//�n�[�h�E�F�A�̏�����

	Opening(); //�d�����͒���̋������L��


	int temp;


	//font_pattern_data�ɕ\�����������t�H���g�f�[�^�̈�̐擪�A�h���X���Z�b�g����ƁA
	//LED�}�g���N�X��ɂ��̃p�^�[�����\�������B
	//Wait_Time�Ŏw�肳��鎞�ԊԊu�ŁALED�}�g���N�X�\�����X�V����A���̓��삪�J��Ԃ����B
	//

	while(1){

		// �r�b�g��`�悷��ׂ̏����B��s�@�A�e�A�G�Ȃǂ̃p�[�c��g�ݍ��킹��Result�ɂ��ׂẴp�[�c�𔽉f�B
		for(int i = 0; i < ROW_SIZE; ++i) {
			Result[0][i] = 0x0; //���Z�b�g������B���ꂪ�Ȃ��ƒe���ǂɓ������Ă�������`�ʂ��`���Ȃ��B
			Result[0][i] = invader[0][i] | bom[0][i] | enemy[0][i] | kabe[0][i];
		}


		//�G�L��������ʊO�ɏo���ۂɂ�����x��ʂɖ߂�悤�Ƀ��[�e�V��������悤�ɐݒ�B
		temp= enemy[0][0];

		//�z��̗�̃T�C�Y�i8�j���J��Ԃ��`�悷��B
		for(pattern_num=0; pattern_num<(sizeof invader)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){

			if(Bom_Flag==1){

				if(Bom_Count==8){
					Bom_Count=0;
					Bom_Flag=0;
					break;

				}else{
					Bom(bom);
					font_pattern_data=Result[pattern_num];	//�\�����������t�H���g�f�[�^�̐擪�A�h���X���擾�B�����SysTick_Handler�ɓn�����B
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
			Wait_Time(100);	//�҂�

			}
		}

		//�G�L��������ʊO�ɏo���ۂɂ�����x��ʂɖ߂�悤�Ƀ��[�e�V��������悤�ɐݒ�B
		enemy[0][7] = temp;

		if(up & Switch_On){
			up &= ~Switch_On;
			switch(sw_now){	//�����ꂽ�X�C�b�`�̏�Ԃɂ���āA�\����ς���

			    //SW1,SW2�X�C�b�`�����������ꂽ��e�𔭎˂���iBom_Flag��1�𗧂Ă�j
				case SW1|SW2:
				Bom_Flag=1;

				melody_address=melody[0];
				play_music(melody_address);

				break;

				//SW1�X�C�b�`�������ꂽ���ɓ���
				case SW1:
				UpMatrix(invader,bom, 0);
				//BuzzerOn(rate,b_time);
				break;

				//SW2�X�C�b�`�������ꂽ�牺�ɓ���
				case SW2:
				DownMatrix(invader,bom, 0);
			}
		}


		//�ǂ����ׂĉ󂳂ꂽ���ǂ����̊m�F�B
		if(second_flag==0){
			for(int i = 0; i<ROW_SIZE; i++){
				enemy_number |= kabe[0][i] ;
			}

            //�ǂ����ׂĔj�󂳂ꂽ��ASecond_Stage�ֈڍs����
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

		//�G���S�ł������ǂ����̊m�F
		}else if(second_flag==1){
			for(int i = 0; i<ROW_SIZE; i++){
				enemy_number |= enemy[0][i] ;
			}

			//�G���S�ł����ꍇ�ɂ�Finish()���Ăяo���A�����ݒ�i�ǂ̕����j�Ɉڍs
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




//�d������ꂽ����ɓ��삷��
void Opening(){

	for(pattern_num=0; pattern_num<(sizeof start)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[2];
		play_music(melody_address);
		font_pattern_data=start[pattern_num];	//�\�����������t�H���g�f�[�^�̐擪�A�h���X���擾�B�����SysTick_Handler�ɓn�����B
		Wait_Time(300);	//�҂�
		melody_address=melody[3];
		play_music(melody_address);
	}

}

//�ǂ����ׂĉ󂵂��ꍇ�ɓ��삷��
void Second_Stage(){

	for(pattern_num=0; pattern_num<(sizeof second_stage)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[10];
		play_music(melody_address);
		font_pattern_data=second_stage[pattern_num];	//�\�����������t�H���g�f�[�^�̐擪�A�h���X���擾�B�����SysTick_Handler�ɓn�����B
		Wait_Time(300);	//�҂�
		melody_address=melody[11];
		play_music(melody_address);
	}

}

//�G�����ׂē|�����ꍇ�ɂǂ�����
void Finish(){

	for(pattern_num=0; pattern_num<(sizeof finish)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
		melody_address=melody[stage_clear_music];
		play_music(melody_address);
		font_pattern_data=finish[pattern_num];	//�\�����������t�H���g�f�[�^�̐擪�A�h���X���擾�B�����SysTick_Handler�ɓn�����B
		Wait_Time(500);	//�҂�
		stage_clear_music++;
	}

	stage_clear_music = 4;

}


//SW1�X�C�b�`�������ꂽ�ꍇ�ɓ���A��ɓ���
void UpMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row) {

	uint8_t temp;
	uint8_t temp1;

    temp= matrix[0][0];
    temp1= bom[0][0];

    for (int j = 0; j < 7; ++j) {
    	matrix[0][j] = matrix[0][j + 1];  // �E�ɃV�t�g
    	bom[0][j] = bom[0][j + 1];

    }
    matrix[0][7] = temp;
    bom[0][7] = temp1;
}

//SW2�X�C�b�`�������ꂽ�ꍇ�ɓ���A��ɓ���
void DownMatrix(uint8_t matrix[][8],uint8_t bom[][8],int row) {

	uint8_t temp;
	uint8_t temp1;

    temp= matrix[0][7];
    temp1= bom[0][7];

    for (int j = 7; j > 0; --j) {
    	matrix[0][j] = matrix[0][j - 1];  // �E�ɃV�t�g
    	bom[0][j] = bom[0][j - 1];

    }
    matrix[0][0] = temp;
    bom[0][0] = temp1;
}


//SW1�X�C�b�`��SW2�X�C�b�`�����������ꂽ�ꍇ�ɓ���A�e�𔭎˂���
void Bom(uint8_t bom[][8]){


	      // 3�r�b�g�ڂ�1�������ꍇ�̓���̏���
	for (int i = 0; i < ROW_SIZE; ++i) {
		if(bom[0][i] == 0x1){
			bom[0][i] = 0x80;
		}else if(bom[0][i] == 0x2){
			Judge();
			bom[0][i] >>= 1;
		}else{
			bom[0][i] >>= 1;  // �E�V�t�g
		}
	}


}



//�e���ǁA�G�ɓ��������ۂ̔���
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


//�n�[�h�E�F�A�̏�����
void my_init_system(void)
{


	/* �N���b�N���擾�^�ݒ�ׂ̈̍\���̒�` */
	RCC_ClocksTypeDef RCC_Clocks;

	/* �V�X�e���̃N���b�N���̎擾 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick �^�C�}�̐ݒ� */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick �^�C�}�����݂̊Ԋu�� 1ms �ɐݒ�

	//GPIO�@A,B,C��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN|RCC_APB2ENR_IOPCEN;	// GPIOA �̃N���b�N��L���ɂ���
	RCC->APB1ENR = RCC_APB1ENR_TIM3EN;

	GPIOB->CRL=0x4444444B;
	//�|�[�gA�̓��o�͐ݒ�

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;


    // PWM���[�h��I�����APWM�M�����s���ɏo�͂���
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // �v���X�P�[����ݒ肷��
    TIM3->PSC = 720-1;


    // �^�C�}��L���ɂ���R���g���[�����W�X�^�B�@�^�C�}��ON�EOFF�Ȃǐݒ�
    TIM3->CR1 |= TIM_CR1_CEN;



}


//�X�C�b�`��ǂݍ���
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
	while(TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B


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
    // ���������[�h���W�X�^�Ɣ�r���W�X�^�����g���ɉ����Đݒ肷��
    TIM3->ARR = (72000000 / frequency / 720) - 1;
    TIM3->EGR = TIM_EGR_UG;  // �ǉ�: ���W�X�^�̕ύX�������ɔ��f����
    TIM3->CCR3 = (TIM3->ARR + 1) / 2;  // 50% �f���[�e�B�T�C�N��
}

void stop_tone() {
    // PWM�M�����~����
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


	static int swc  = 0;	// ����ǂݍ��񂾃X�C�b�`�̒l
	static int swp1 = 0;	// �O��ǂݍ��񂾃X�C�b�`�̒l
	static int swp2 = 0;	// �O�X��ɓǂݍ��񂾃X�C�b�`�̒l

	static int sw_last = 0;	// �O��̃X�C�b�`�̊m��l
	static int chat_count = 0;


	if(chat_count == 9){
		chat_count =0;

	swp2 = swp1;				// �O�X��̒l��ۑ�����
	swp1 = swc;					// �O��̒l��ۑ�����
	swc  = Get_SW();			// �V���ɍ���̃X�C�b�`�̒l��ǂݍ���

	if ((swp2 == swp1) && (swp1 == swc)) {	// ����A�O��A�O�X��̒l���S�ē�����
		sw_now = swc;							// �ꍇ�A����̒l�����݂̊m��l�Ƃ���
	}

	if (sw_now != sw_last) {	// �O��̃X�C�b�`�̊m��l�ƈقȂ�ꍇ�����ȉ��̏������s��
		if(sw_now & ~sw_last){
			up |= Switch_On;
		}
//		up = sw_now & ~sw_last;	// �X�C�b�`�̗����i���ꂩ�̃X�C�b�`�������ꂽ���j�����o
		sw_last = sw_now;		// ���݂̃X�C�b�`�̊m��l�� sw_last �ɕۑ�����
	}

	}else{

		chat_count++;
	}

	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	if(TimeInt>0) TimeInt--;

	//�V���A���E�p�������ϊ���H�փf�[�^�𑗏o�iLED�}�g���N�X���\�������j

	Send_Word_To_SR(Setup_Serial_Data_2(font_pattern_data));


}








//8bit,1byte��LSB���瑗�o����
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



//8bit,1byte��MSB���瑗�o����
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



//16bit,1word��LSB���瑗�o����

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


//16bit,1word��MSB���瑗�o����

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
			serial_data=0;	//�\��OFF�̏ꍇ�̃t�H���g�f�[�^
	}else{
		col_data=(0x8000 >> col_id);	//�\�������������̐���
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



