/*
 * test-8x8-main-3b.c
 *
 *  Created on: 2023/09/03
 *      Author: x-lib
 */

/*
 * �V���A���������݂̕]��
 * 8x8LED�}�g���N�X��H�̓���m�F
 * �_�C�i�~�b�N�_���@���荞�ݔ�
 *
 * �ϐ����ύX�A�R�����g���������
 * main�Ń}�g���N�X�f�[�^�̃t�H���g���w�肷��悤�ɉ���
 *
 * SW1,SW2�̓��͂ɉ����āALED�}�g���N�X��̕\����ς���\�肾���A���̔łł͖������B
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



//�O���[�o���ϐ�
int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j

int pattern_num;//�\����������8x8LED�}�g���N�X�̃t�H���g�f�[�^�̔ԍ�

//8x8 LED�}�g���N�X�̃t�H���g�f�[�^
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

//���鎞�_��LED�}�g���N�X�ɕ\�����������p�^�[���̃t�H���g�f�[�^�̐擪�A�h���X
uint8_t *font_pattern_data;














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
int main(void)
{

	my_init_system();//�n�[�h�E�F�A�̏�����



	//font_pattern_data�ɕ\�����������t�H���g�f�[�^�̈�̐擪�A�h���X���Z�b�g����ƁA
	//LED�}�g���N�X��ɂ��̃p�^�[�����\�������B
	//
	//Wait_Time�Ŏw�肳��鎞�ԊԊu�ŁALED�}�g���N�X�\�����X�V����A���̓��삪�J��Ԃ����B
	//
	while(1){

		for(pattern_num=0; pattern_num<(sizeof myfont)/ROW_SIZE/(sizeof (uint8_t)); pattern_num++){
			font_pattern_data=myfont[pattern_num];	//�\�����������t�H���g�f�[�^�̐擪�A�h���X���擾�B�����SysTick_Handler�ɓn�����B
			Wait_Time(100);	//�҂�
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

//
// ���荞�݊Ԋu��SysTick_Config()�̈�����
// RCC_Clocks.HCLK_Frequency / 1000�@�Ƃ��āA1msec
//

void SysTick_Handler(void)
{

	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	if(TimeInt>0) TimeInt--;



	//
	//�V���A���E�p�������ϊ���H�փf�[�^�𑗏o�iLED�}�g���N�X���\�������j

	Send_Word_To_SR_From_MSB(Setup_Serial_Data(font_pattern_data));

	//Send_Word_To_SR(Setup_Serial_Data(font_pattern_data));



}
