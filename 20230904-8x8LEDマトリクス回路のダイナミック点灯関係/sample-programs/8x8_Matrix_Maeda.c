/*
 * test-8x8-main-5a.c
 *
 *  Created on: 2023/09/12
 *      Author: x-lib
 */

/*
 *
 * SW0,SW1�̓��͂ɉ����āALED�}�g���N�X��̕\����ς���B
 *
 * �N������́A�C���x�[�_��\���B
 * SW0�������ƔL�ASW1�������ƃ����S�ASW0��SW1�𓯎��ɉ����ƃC���x�[�_���A
 * ���ꂼ��\�������B
 *
 * �t�H���g�f�[�^�����̕��@���C��
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

 *�@SW0		PA7		CN10-15
 *�@SW1		PA8		CN10-23

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

/* �}�N����`�@*/
#define ROW_SIZE 8	//�s��


#define Display_Enable  0x4		// �_�C�i�~�b�N�_���ׂ̈̕\���t���O
#define LP_Count_Enable 0x8 	// �X�C�b�`�������L���������t���O

#define SW0 0b0000000010000000	//SW0(PA7)�p�̃}�X�N�l
#define SW1 0b0000000100000000	//SW0(PA8)�p�̃}�X�N�l
#define SW_OFF 0x0
//#define start_flg 0b10000

#define Divide_Count 1000		// SysTick �^�C�}�[�̊����݊Ԋu�̌W���i1 ms �p�j
#define LP_Interval  1000		// �X�C�b�`�����������J�n�܂ł̎��ԊԊu�i1 ms �P�ʁj


//TIM�ɂ��u�U�[���K�f�[�^
#define C2 134 //��
#define D2 151 //��
#define E2 170 //��
#define F2 179 //�ӂ�
#define G2 201 //��
#define A2 226 //��
#define B2 254 //��
#define C3 267 //��
#define D3 300 //��
#define E3 336 //��
#define F3 353 //�ӂ�
#define G3 396 //��
#define A3 444 //��
#define B3 498 //��
#define C4 523 //��
#define D4 587 //��
#define E4 659 //��
#define F4 698 //�ӂ�
#define G4 783 //��
#define A4 880 //��
#define B4 987 //��
#define C5 1046 //��
#define D5 1174 //��
#define E5 1315 //��
#define F5 1392 //�ӂ�
#define G5 1562 //��
#define A6 1751 //��
#define B6 1964 //��
#define C6 2063 //��
#define D6 2318 //��
#define E6 2596 //��
#define F6 2727 //�ӂ�
#define G6 3059 //��

#define S4 1015 //��#
#define P4 732 //�ӂ�#
#define O6 2985

#define LEN1	500

#define LEN2	250 // LEN1��1/2�{
#define LEN3	750 // LEN1��1.5�{
#define LEN4	125 // LEN1��1/4�{
#define LEN5	100 // LEN1��1/5�{
#define LEN6	63  // LEN1��1/6�{


//�����f�[�^�̍\����
typedef struct{
	int pitch;
	int duration;
} Note;

//=======================< �O���[�o���E�f�[�^��` >============================

unsigned int Up   = 0;	// �\���^�C�~���O�y�уX�C�b�`�����������t���O�ϐ�
int Switch_ID     = 0;	// ���ݒ��������̃X�C�b�`�i0:�Ȃ��A1:�X�C�b�`�O�A2:�X�C�b�`�P�j
int LP_Count_Flag = 0;	// �X�C�b�`���������̃J�E���g�A�b�v�^�C�~���O�������t���O�ϐ�




//�O���[�o���ϐ�
int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int m_TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j


int pattern_num;//�\����������8x8LED�}�g���N�X�̃t�H���g�f�[�^�̔ԍ�

int music_play = 0; //1��systick���ŉ���炷
int m_time = 0;
int melody_number = 0;
int m_i = 0;
//unsigned int act_flg = 1;----���܂����삵�Ȃ�
/*-----act_flg-----*/
/*  0: �����Ȃ�              */
/*  1: ptr,PTR������  */
/*  2: bee,BEE������  */
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

int time_data = 0;	//���ԃf�[�^

unsigned int cnt_flg   = 0;

unsigned int cnt_flg2  = 0;

unsigned int cnt_flg3  = 0;

unsigned int cnt_flg4  = 0;

unsigned int cnt_flg5  = 0;

unsigned int cnt_flg6  = 0;

unsigned int cnt_flg_s = 0; //�������̎��̏�Ԃ�ۑ�����

unsigned int cnt_flg_p = 0; //���ʂ̎��̏�Ԃ�ۑ�����

unsigned int wait_flg  = 0;


//8x8 LED�}�g���N�X�̃t�H���g�f�[�^

uint8_t face[]={0x0,0x22,0x77,0x22,0x0,0x8,0x22,0x1C};		//��
uint8_t invader[]={0x81,0x42,0x3C,0x7E,0xDB,0x7E,0x42,0xC3};//�C���x�[�_
uint8_t cat[]={0x22,0x66,0xFF,0x99,0xBB,0xFF,0x7E,0x00};		//�L
uint8_t apple[]={0xC,0x8,0x7E,0xDF,0xBF,0xBF,0xDF,0x7E};	//�����S
uint8_t bee[]={0x40,0xB1,0x3F,0xF,0x6,0x3E,0x7C,0x38};      //�I
uint8_t BEE[]={0x40,0xB0,0x3E,0xE,0x7,0x3F,0x7D,0x38};      //�I2
uint8_t mantis[]={0x0,0xC0,0xB0,0x3E,0x4F,0xC7,0x9B,0x91};  //��
uint8_t coconutcrab[]={0x24,0x24,0x5A,0xDB,0x3C,0x7E,0xBD,0xBD};  //���V�K�j
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

uint8_t test_bee[]={0x41,0xB3,0x3F,0xE,0xC,0x7C,0xF8,0x70};      //test�I
uint8_t test_BEE[]={0x40,0xB0,0x3C,0xC,0xE,0x7F,0xFB,0x71};      //test�I2

//�y�Ȃ̉����f�[�^
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
				{{C2,LEN1},{D2,LEN1},{E2,LEN1},{F2,LEN1},{G2,LEN1},{A2,LEN1},{B2,LEN1},{0,0}} //�Ⴂ���̃e�X�g
		};

Note* melody_address;	//���t������Ȃ̊y���f�[�^�̐擪�A�h���X

//Note m_data[];




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
int play_music(Note data[]);


//send_data_to_sr.c�ɋL�q���ꂽ�֐��Q
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

extern void Flash_SR(void);





//
//���C���֐�
//



//main()����while���[�v���ŁA
//font_pattern_data�ɕ\�����������t�H���g�f�[�^�̈�̐擪�A�h���X���Z�b�g����ƁA
//LED�}�g���N�X��ɂ��̃p�^�[�����\�������B
//
//Wait_Time�Ŏw�肳��鎞�ԊԊu�ŁALED�}�g���N�X�\�����X�V����A���̓��삪�J��Ԃ����B
//
//�G�����X�C�b�`�̏�Ԃɉ����ĕω�������B




int main(void)
{

	my_init_system();//�n�[�h�E�F�A�̏�����

	//Up = Up | start_flg;



/*
	//�������
	switch (cnt_flg) {
	case 0:
		font_pattern_data=bee;
		break;
	case 1:
		font_pattern_data=BEE;
	}
*/
/* ����炷�e�X�g  */
//melody_number = 4;
//music_play = 1;
//while(1);
	while(1) {

		//�������
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

	if (Up & LP_Count_Enable) {			// �������������L���ł���Ԃ�
		if (LP_Count_Flag) {			// ���������̃J�E���g�Ԋu�t���O
			LP_Count_Flag = 0;			// ����������A�܂����Z�b�g����
			//act_flg = 0;
			switch (Switch_ID) {		// �X�C�b�`�ɉ����ď������s��
			case 1:							// �X�C�b�`�O�̒������Ȃ��
				//act_flg = 1;
				start_flg = 0;
				break;
			case 2:							// �X�C�b�`�P�̒������Ȃ��
				//act_flg = 2;
				Bee_flg = 1;
			}
		}
	} else {
			switch(Up){
			case SW0:
				Up &= ~SW0;					// �X�C�b�`�O�̉����t���O�����Z�b�g
				//act_flg = 0;
				start_flg  = 1;
				Bee_flg    = 0;
				ptr_flg = 1;
				break;
			case SW1:
				Up &= ~SW1;					// �X�C�b�`�P�̉����t���O�����Z�b�g
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
//�n�[�h�E�F�A�̏�����
//
void my_init_system(void)
{


	/* �N���b�N���擾�^�ݒ�ׂ̈̍\���̒�` */
	RCC_ClocksTypeDef RCC_Clocks;

	/* �V�X�e���̃N���b�N���̎擾 */
	RCC_GetClocksFreq(&RCC_Clocks);

	/* SysTick �^�C�}�̐ݒ� */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick �^�C�}�����݂̊Ԋu�� 10ms �ɐݒ�

	//GPIO�@A��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN|RCC_APB2ENR_IOPBEN;	// GPIOA �̃N���b�N��L���ɂ���

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	//�|�[�gA�̓��o�͐ݒ�

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;

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
//
//SysTick_Handler���ŌĂяo�����
//
uint16_t Setup_Serial_Data(uint8_t font_data[]){

	static int row_index=0;
	static int col_pos=0x8000; //LED�}�g���N�X�̃J����Column���w�肷��ϐ��BMSB��COL0�ɐڑ�
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
//�V���A���E�p�������ϊ���H�ɑ���o���f�[�^�𐶐�����֐�
//
//SysTick_Handler���ŌĂяo�����
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




void Wait_Time(int time)
{
	TimeInt=time;
	while(!wait_flg && TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B


}





/*
**	get_sw --- �P���Ɍ��݂̃X�C�b�`�̒l�𐳘_���i�A�N�e�B�u�n�C�j�Ŏ擾����֐�
*/

int get_sw(void)
{
	int sw;

	sw = GPIOA->IDR;		// ���݂̃X�C�b�`�̒l��ǂݍ���
	sw = ~sw;				// ���_���Ȃ̂Ńr�b�g�𔽓]����
	sw = sw & (SW0|SW1);	// �X�C�b�`�ȊO�̃r�b�g���}�X�N����

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
// ���荞�݊Ԋu��SysTick_Config()�̈�����
// RCC_Clocks.HCLK_Frequency / 1000�@�Ƃ��āA1msec
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



	static unsigned int swc  = 0;	// ����ǂݍ��񂾃X�C�b�`�̒l
	static unsigned int swp1 = 0;	// �O��ǂݍ��񂾃X�C�b�`�̒l
	static unsigned int swp2 = 0;	// �O�X��ɓǂݍ��񂾃X�C�b�`�̒l
	static unsigned int now  = 0;	// ����i���݁j�̃X�C�b�`�̊m��l
	static unsigned int last = 0;	// �O��̃X�C�b�`�̊m��l

	static int chat_count    = 0;	// �`���^�����O�����p�̃J�E���^�ϐ�

	static int lp_start   = 0;	// �X�C�b�`�����������J�n�t���O
	static int lp_counter = 0;	// �X�C�b�`�������L�����܂ł̎��ԁi1 �b�j�𑪂�J�E���^
	static int lp_int_cnt = 0;	// �X�C�b�`���������̃^�C�~���O�i0.1 �b�j�����p�J�E���^


/* �`���^�����O�����y�ї����i�X�C�b�`�̉����j���o */

	if (chat_count == 9) {

		chat_count = 0;

		swp2 = swp1;		// �O�X��̃X�C�b�`�̓ǂݎ��l��ۑ�����
		swp1 = swc;			// �O��̃X�C�b�`�̓ǂݎ��l��ۑ�����
		swc  = get_sw();	// �V���ɍ���̃X�C�b�`�̒l��ǂݍ���

		if ((swp2 == swp1) && (swp1 == swc)) {	// ����A�O��A�O�X��̒l���S�ē�����
			now = swc;							// �ꍇ�A����̒l�����݂̊m��l�Ƃ���
		}

		if (now != last) {			// �O��̃X�C�b�`�̒l�ƈقȂ�ꍇ�����ȉ��̏������s��
			if (now & ~last) {		// �����i�����ꂩ�̃X�C�b�`�̉����j�����o������
				Up |= now & ~last;	// �X�C�b�`�i�O�܂��͂P�j�����̃t���O�𗧂Ă�

			/* �X�C�b�`�������̊J�n���� */

				if (Up & SW0) {			// �X�C�b�`�O�������ꂽ��A
					if (!Switch_ID) {	// �������X�C�b�`�h�c���m�F���A�O�ł���΁A
						Switch_ID = 1;	// �������X�C�b�`�h�c���P�ɐݒ肷��
					}
				}
				if (Up & SW1) {			// �X�C�b�`�P�������ꂽ��A
					if (!Switch_ID) {	// �������X�C�b�`�h�c���m�F���A�O�ł���΁A
						Switch_ID = 2;	// �������X�C�b�`�h�c���Q�ɐݒ肷��
					}
				}
				lp_start   = 1;		// �X�C�b�`�̒��������Ԃ̃J�E���g��L���ɂ���
				lp_counter = 0;		// �X�C�b�`�̒��������ԃJ�E���^���O�Ƀ��Z�b�g����
				lp_int_cnt = 0;		// ���������̃J�E���g�E�^�C�~���O�p�J�E���^���O��
									// ���Z�b�g����
			}
			last = now;				// ���݂̃X�C�b�`�̒l�� last �ɕۑ�����
		}


	/* �X�C�b�`�������̏I������ */

		if (Switch_ID) {			// �������X�C�b�`���m�肵�Ă���ꍇ
			switch (Switch_ID) {	// ���̃X�C�b�`�ɉ����ĉ������s��
			case 1:						// �X�C�b�`�O���������X�C�b�`�̏ꍇ��
				if (!(now & SW0)) {		// �X�C�b�`�O�������ꂽ�i�I�t�ɂȂ����j��
					lp_start = 0;		// �X�C�b�`�̒��������ԃJ�E���g�̃t���O���I�t��
					Up &= ~LP_Count_Enable;	// �����������L���̃t���O���I�t�ɂ�
					Switch_ID = 0;			// �������X�C�b�`�h�c���O�Ƀ��Z�b�g����
				}
				break;
			case 2:						// �X�C�b�`�P���������X�C�b�`�̏ꍇ��
				if (!(now & SW1)) {		// �X�C�b�`�P�������ꂽ�i�I�t�ɂȂ����j��
					lp_start = 0;		// �X�C�b�`�̒��������ԃJ�E���g�̃t���O���I�t��
					Up &= ~LP_Count_Enable;	// �����������L���̃t���O���I�t�ɂ�
					Switch_ID = 0;			// �������X�C�b�`�h�c���O�Ƀ��Z�b�g����
				}
			}
		}

	} else {

		chat_count++;

	}

/* �X�C�b�`�������̗L�����`�F�b�N */

	if (lp_start) {						// �X�C�b�`�̒��������ԃJ�E���g�̃t���O���L���ȏꍇ��
		if (lp_counter < LP_Interval) {	// �w�肳�ꂽ���ԁi�P�b�j�܂ŃJ�E���g�𑱂�
			lp_counter++;
		} else {						// �w�肳�ꂽ���ԁi�P�b�j������������
			Up |= LP_Count_Enable;		// �����������L���̃t���O�𗧂Ă�
		}
	}

/* �X�C�b�`�������L�����̃J�E���g�A�b�v�̃^�C�~���O���� */

	if (Up & LP_Count_Enable) {		// �������������L���ȊԂ�
		if (lp_int_cnt < 99) {		// �w�肳�ꂽ�Ԋu�i0.1�b�j
			lp_int_cnt++;			// �܂ŃJ�E���g�A�b�v����
		} else {					// ���ԁi0.1�b�j������������
			lp_int_cnt = 0;			// �J�E���^���O�Ƀ��Z�b�g����
			LP_Count_Flag = 1;		// ���������̃J�E���g�Ԋu��
		}							// �^�C�~���O�̃t���O���Z�b�g����
	}





	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	//
	if(TimeInt>0) {
		if(Up) {
			wait_flg = 1;
		}else
			TimeInt--;
	}

	if(m_TimeInt>0) {//���y�p�҂����ԃJ�E���g
		if(Up) {
			wait_flg = 1;
		}else
			m_TimeInt--;
	}


	//
	//�V���A���E�p�������ϊ���H�փf�[�^�𑗏o�iLED�}�g���N�X���\�������j

	//Send_Word_To_SR_From_MSB(Setup_Serial_Data(font_pattern_data));

	//Send_Word_To_SR(Setup_Serial_Data(font_pattern_data));

	//2023/09/12 ����
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
		if(music_play == 1)  //���t��
		    if (( m_TimeInt==0)||(wait_flg==1)){
				if (melody[melody_number][m_i].pitch!=0){
					stop_tone();
					play_tone(melody[melody_number][m_i].pitch);
					//Wait_Time(data[i].duration);
					m_time = melody[melody_number][m_i].duration;
					m_TimeInt=m_time;
					//while(!wait_flg && TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B
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
