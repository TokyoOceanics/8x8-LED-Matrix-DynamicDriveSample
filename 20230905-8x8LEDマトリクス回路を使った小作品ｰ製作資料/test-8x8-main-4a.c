/*
 * test-8x8-main-4a.c
 *
 *  Created on: 2023/09/06
 *      Author: x-lib
 */

/*
 *
 * SW1,SW2�̓��͂ɉ����āALED�}�g���N�X��̕\����ς���B
 *
 * �N������́A�C���x�[�_��\���B
 * SW1�������ƔL�ASW2�������ƃ����S�ASW1��SW2�𓯎��ɉ����ƃC���x�[�_���A
 * ���ꂼ��\�������B
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






/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* �}�N����`�@*/
#define ROW_SIZE 8	//�s��


//          0bFEDCBA9876543210
#define SW1 0b0000000010000000	//Sw1(PA7)�p�̃}�X�N�l
#define SW2 0b0000000100000000	//Sw1(PA8)�p�̃}�X�N�l




//�O���[�o���ϐ�
int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int pattern_num;//�\����������8x8LED�}�g���N�X�̃t�H���g�f�[�^�̔ԍ�


//8x8 LED�}�g���N�X�̃t�H���g�f�[�^

uint8_t face[]={0x0,0x22,0x77,0x22,0x0,0x8,0x22,0x1C};		//��
uint8_t invader[]={0x81,0x42,0x3C,0x7E,0xDB,0x7E,0x42,0xC3};//�C���x�[�_
uint8_t cat[]={0x0,0x42,0x66,0xFF,0x99,0xBB,0xFF,0x7E};		//�L
uint8_t apple[]={0xC,0x8,0x7E,0xFF,0xBF,0xBF,0xFF,0x7E};	//�����S


//���鎞�_��LED�}�g���N�X�ɕ\�����������p�^�[���̃t�H���g�f�[�^�̐擪�A�h���X
uint8_t *font_pattern_data;



//�X�C�b�`�֌W�̕ϐ�
int up=0;		//�X�C�b�`�̗����オ�茟�o�t���O�A0�ŏ�����
int sw_now=0;	//���݂̃X�C�b�`������ԁA0�ŏ�����




//�֐��̃v���g�^�C�v�錾
void my_init_system(void);
void Wait_Time(int time);	//SysTick_Handler���g���҂����ԏ���


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






	//�������
	font_pattern_data=invader;


	while(1){



		while(!up);	//�X�C�b�`�������������܂ő҂B
		up=0;		//�����ꂽ��A�t���O���N���A����B

		switch(sw_now){	//�����ꂽ�X�C�b�`�̏�Ԃɂ���āA�\����ς���

			case SW1|SW2:
				font_pattern_data=invader;
				break;

			case SW1:
				font_pattern_data=cat;
				break;

			case SW2:
				font_pattern_data=apple;


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
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA �̃N���b�N��L���ɂ���

	//�|�[�gA�̓��o�͐ݒ�

//               76543210
	GPIOA->CRL=0x41114444;

//				 FEDCBA98
	GPIOA->CRH=0x44444444;
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














//
// ���荞�݊Ԋu��SysTick_Config()�̈�����
// RCC_Clocks.HCLK_Frequency / 1000�@�Ƃ��āA1msec
//

void SysTick_Handler(void)
{




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



	//
	//�V���A���E�p�������ϊ���H�փf�[�^�𑗏o�iLED�}�g���N�X���\�������j

	Send_Word_To_SR_From_MSB(Setup_Serial_Data(font_pattern_data));

	//Send_Word_To_SR(Setup_Serial_Data(font_pattern_data));













}
