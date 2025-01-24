/*
 * BuzzerTest01.c
 *
 *  Created on: 2023/09/18
 *      Author: x-lib
 */

/*
 *�@buzzer test�@���̂Q
	SW1�ASW2�̉����󋵂ɂ���āA�Ȃ�ς���B
	TIM3 PWM���g�p�B

 * PB0(TIM3-Ch3) Buzzer
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

 *�@SW1		PA7		CN10-15
 *�@SW2		PA8		CN10-23

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

/* �}�N����`�@*/
#define ROW_SIZE 8	//�s��


//          0bFEDCBA9876543210
#define SW1 0b0000000010000000	//Sw1(PA7)�p�̃}�X�N�l
#define SW2 0b0000000100000000	//Sw1(PA8)�p�̃}�X�N�l

#define	STOP	0	//��~���
#define RUN		1	//�J�E���g���


//TIM�ɂ��u�U�[���K�f�[�^
#define C4 523 //��
#define D4 587 //��
#define E4 659 //��
#define F4 698 //�ӂ�
#define G4 783 //��
#define A4 880 //��
#define B4 987 //��
#define C5 1046 //��
#define D5 1174 //��

#define LEN1	500


//�����f�[�^�̍\����
typedef struct{
	int pitch;
	int duration;
} Note;


//�O���[�o���ϐ�
int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int pattern_num;//�\����������8x8LED�}�g���N�X�̃t�H���g�f�[�^�̔ԍ�

int time_data = 0;	//���ԃf�[�^


int State_ID=STOP;



//�y�Ȃ̉����f�[�^
Note melody[][20]={
		{{B4,LEN1},{G4,LEN1},{D4,LEN1},{G4,LEN1},{A4,LEN1},{D5,LEN1*2},
				{D4,LEN1},{A4,LEN1},{B4,LEN1},{A4,LEN1},{D4,LEN1},{G4,LEN1*2},{0,0}},
		{{C4,LEN1},{C4,LEN1},{G4,LEN1},{G4,LEN1},{A4,LEN1},{A4,LEN1},{G4,LEN1*2},
				{F4,LEN1},{F4,LEN1},{E4,LEN1},{E4,LEN1},{D4,LEN1},{D4,LEN1},{C4,LEN1*2},{0,0}},
				{{C4,LEN1},{D4,LEN1},{E4,LEN1},{F4,LEN1},{G4,LEN1},{A4,LEN1},{B4,LEN1},{C5,LEN1},{D5,LEN1*2}}
		};

Note* melody_address;	//���t������Ȃ̊y���f�[�^�̐擪�A�h���X



//8x8 LED�}�g���N�X�̃t�H���g�f�[�^
//����0~10�܂ł̃t�H���g�f�[�^
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

//���鎞�_��LED�}�g���N�X�ɕ\�����������p�^�[���̃t�H���g�f�[�^�̐擪�A�h���X
uint8_t *font_pattern_data;


//�X�C�b�`�֌W�̕ϐ�
int up=0;		//�X�C�b�`�̗����オ�茟�o�t���O�A0�ŏ�����
int sw_now=0;	//���݂̃X�C�b�`������ԁA0�ŏ�����

//�֐��̃v���g�^�C�v�錾
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handler���g���҂����ԏ���


//�u�U�[�o�͊֌W
void play_tone(unsigned int frequency);
void stop_tone();
void play_music(Note data[]);


//send_data_to_sr.c�ɋL�q���ꂽ�֐��Q
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

extern void Flash_SR(void);


//
//���C���֐�
//
int main(void)
{

	my_init_system();//�n�[�h�E�F�A�̏�����


	while(1){

		while(!up);	//�X�C�b�`�������������܂ő҂B
		up=0;		//�����ꂽ��A�t���O���N���A����B

		switch(sw_now){	//�����ꂽ�X�C�b�`�̏�Ԃɂ���āA���y��ς���

			case SW1:	//�t�@�~���[�}�[�g

				melody_address=melody[0];
				break;

			case SW2:	//�L���̔ޕ������������o������
				melody_address=melody[1];
				break;

			case SW1|SW2:	//���̃��W�b�N�ł́A���̕����͎��s����Ȃ��͂��B
				melody_address=melody[2];
				break;
		}

		play_music(melody_address);
		time_data=0;


	}
}




//
//�n�[�h�E�F�A�̏�����
//

void my_init_system(void)
{

	/* �N���b�N���擾�^�ݒ�ׂ̈̍\���̒�` */
	RCC_ClocksTypeDef RCC_Clocks;

	/* �V�X�e���̃N���b�N���̎擾 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick �^�C�}�̐ݒ� */
	/*SysTick �^�C�}�����݂̊Ԋu�� 1ms �ɐݒ� */
	// ���荞�݊Ԋu��SysTick_Config()�̈�����RCC_Clocks.HCLK_Frequency / 1000�@�Ƃ��āA1msec
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//

	//GPIO�@A��B��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPBEN;	// GPIOA,B�̃N���b�N��L���ɂ���

	//TIM3 �L�������āAPB0��TIM3�Ƃ���
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;	//TIM3�̗L����

	//�|�[�gA�̓��o�͐ݒ�
	//			 76543210
	GPIOA->CRL=0x41114444;
	//			 FEDCBA98
	GPIOA->CRH=0x44444444;

	//�|�[�gB�̓��o�͐ݒ�@GPIOB->CRL�ݒ�́A�|�[�gB�̓��o�͐ݒ�@pb0��TIM3�Ƃ���
	//PB0�@CNF=10�AMODE=11�́@�I���^�l�[�g�o�́E�v�b�V���v��
	//			 76543210
	GPIOB->CRL=0x4444444B;

    // PWM���[�h��I�����APWM�M�����s���ɏo�͂���
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
    TIM3->CCER |= TIM_CCER_CC3E;

    // �v���X�P�[����ݒ肷��
    TIM3->PSC = 720-1;

    // �^�C�}��L���ɂ���R���g���[�����W�X�^�B�@�^�C�}��ON�EOFF�Ȃǐݒ�
    TIM3->CR1 |= TIM_CR1_CEN;
}










//
//�V���A���E�p�������ϊ���H�ɑ���o���f�[�^�𐶐�����֐�
// 2023/09/12 �����搶
//
//SysTick_Handler���ŌĂяo�����
//

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


/*
 * �҂����Ԋ֐��@�iSysTick_Handler�ƘA�g�j
 */

void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B


}





/*
**	get_sw --- �P���Ɍ��݂̃X�C�b�`�̒l�𐳘_���i�A�N�e�B�u�n�C�j�Ŏ擾����֐�
*/

int get_sw(void)
{
	int sw;

	sw = GPIOA->IDR;		// ���݂̃X�C�b�`�̒l��ǂݍ���
	sw = ~sw;				// ���_���Ȃ̂Ńr�b�g�𔽓]����
	sw = sw & (SW1|SW2);	// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

	return sw;
}

/*
**    read_sw --- �`���^�����O�h�~�@�\�t���̃X�C�b�`�̊m��l��ǂݍ��ފ֐�
*/

int read_sw(void)
{
	static int swc  = 0;	// ����ǂݍ��񂾃X�C�b�`�̒l
	static int swp1 = 0;	// �O��ǂݍ��񂾃X�C�b�`�̒l
	static int swp2 = 0;	// �O�X��ɓǂݍ��񂾃X�C�b�`�̒l
	static int sw   = 0;	// ���݂̃X�C�b�`�̊m��l

	swp2 = swp1;				// �O�X��̒l��ۑ�����
	swp1 = swc;					// �O��̒l��ۑ�����
	swc  = get_sw();			// �V���ɍ���̃X�C�b�`�̒l��ǂݍ���

	if ((swp2 == swp1) && (swp1 == swc)) {	// ����A�O��A�O�X��̒l���S�ē�����
		sw = swc;							// �ꍇ�A����̒l�����݂̊m��l�Ƃ���
	}

	return sw;		// ���݂̃X�C�b�`�̊m��l��Ԃ�
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
	while(data[i].pitch!=0){
		play_tone(data[i].pitch);
		Wait_Time(data[i].duration);
		i++;
		time_data=i;
		stop_tone();
	}

}












/*
 * SysTick_Handler�֐�
*/

void SysTick_Handler(void)
{

	static int count_1000ms=0;	//1�b����邽�߂�1000��̌Ăяo�����J�E���g����ϐ��@�i1msec*1000��1�b�j

	static int sw_last = 0;	// �O��̃X�C�b�`�̊m��l

	//
	//�X�C�b�`�֌W�̏���
	//
	sw_now = read_sw();			// �X�C�b�`�̊m��l��ǂݍ���

	if (sw_now != sw_last) {	// �O��̃X�C�b�`�̊m��l�ƈقȂ�ꍇ�����ȉ��̏������s��
		up = sw_now & ~sw_last;	// �X�C�b�`�̗����i���ꂩ�̃X�C�b�`�������ꂽ���j�����o
		sw_last = sw_now;		// ���݂̃X�C�b�`�̊m��l�� sw_last �ɕۑ�����
	}




	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	//
	if(TimeInt>0) TimeInt--;


	//1�b�̃J�E���g�A�b�v����
	if(State_ID==RUN){
		if(count_1000ms<1000){
			count_1000ms++;	//SysTick_Handler��1000��Ăяo����邲�ƂɃJ�E���g��1���₷
		}else{
			time_data++;	//1000��ڂ̌Ăяo���ŁA1�b�o�߂��J�E���g�A�b�v����B
			count_1000ms=0;	//1000��ڂŁA�J�E���^�����Z�b�g����B
		}
	}

	//���ԏ��̕\��
	font_pattern_data=number_data[time_data%10];
	Send_Word_To_SR(Setup_Serial_Data_2(font_pattern_data));




}
