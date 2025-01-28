/*
 * test-8LEDs-main-5.c
 *
 *  Created on: 2023/07/11
 *      Author: x-lib
 */

/*
 * �V���A���������݂̕]��
 * ���C�搶�{�[�h��LED�_��
 * �_�C�i�~�b�N�_���@���荞�ݔ�
 *
 * �v���O�����N����ɁA�o�ߎ��Ԃ�mm:ss�`���ŕ\������B
 *
 *2023/08/24 �R�����g�ǉ�
 *          �J�E���g����̌�����
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
 * Q0		SEG-A	��
 * Q1		SEG-B	��
 * Q2		SEG-C	��
 * Q3		SEG-D	��
 * Q4		SEG-E	��
 * Q5		SEG-F	��
 * Q6		SEG-G	��
 * Q7		SEG-DP	�D
 * Q8		N.C.
 * Q9		N.C.
 * QA		AN3(Digit4)		��
 * QB		AN2(Digit3)		�D
 * QC		AN1(Digit2)		��
 * QD		AN0(Digit1)		��
 *
 * Vdd	Vcc		��
 * GND	GND		��
 *
 */



//�O���[�o���ϐ�
int TimeInt=0;	//delay�����iWait����Ԋu���w�肷�邽�߁j�̃O���[�o���ϐ��iSysTick�Ō��Z�j


int time_count;	//�������@�b
int minutes_data[2];	//7�Z�O���2���ɕ\�����鐔�l�i���j
int seconds_data[2];	//7�Z�O����2���ɕ\�����鐔�l�i�b�j


//�֐��̃v���g�^�C�v�錾
void my_init_system(void);

void wait_delay(int delay);
void Wait_Time(int time);	//SysTick_Handler���g���҂����ԏ���

uint16_t make_digit_data(int i);
void display_number_on_7SegLED(int num, int digit_position);


//send_data_so_sr.c�Œ�`����Ă���֐��Q
extern void Send_Byte_To_SR(uint8_t bytes);
extern void Send_Word_To_SR(uint16_t words);

extern void Send_Byte_To_SR_From_MSB(uint8_t bytes);
extern void Send_Word_To_SR_From_MSB(uint16_t words);

void Flash_SR(void);




//
//���C���֐�
//
int main(void)
{

	time_count=0;	//�b����0�ŏ���������B

	my_init_system();




	while(1){

		Wait_Time(100);	//100msec�҂�,1000�Ƃ���1000msec�҂�
		if(time_count==(60*60-1))
			time_count=0;
		else
			time_count++;
	}


}

//�������iMM:SS)���V�Z�OLED�\�����邽�߂̊e���ւ̕\���f�[�^����
void  sec_to_mm_and_ss(void){

	int min, sec;


	min=time_count/60;
	sec=time_count%60;

	minutes_data[1]=min/10;	//���@10�̈�
	minutes_data[0]=min%10;	//���@1�̈�

	seconds_data[1]=sec/10;	//�b�@10�̈�
	seconds_data[0]=sec%10;	//�b�@1�̈�


}



//���l1����7�Z�O�\���p�f�[�^�ɕϊ�����
uint16_t make_digit_data(int num)
{
	uint16_t output_data;
	output_data=(0xff00|(~seg7[num]));
	return output_data;
}

//�w��̌��ɐ��l1����\������
void display_number_on_7SegLED(int num, int digit_position)
{
	uint16_t output_data;
	output_data=make_digit_data(num)&disp_digit[digit_position];
	Send_Word_To_SR_From_MSB(output_data);
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
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	// SysTick �^�C�}�����݂̊Ԋu�� 1ms �ɐݒ�



	//GPIO�@A��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA �̃N���b�N��L���ɂ���

//�|�[�gA�̓��o�͐ݒ�

//               76543210
	GPIOA->CRL=0x41114444;

}






void wait_delay(int delay){

	for(int i=0; i<delay; i++);


}



void Wait_Time(int time)
{
	TimeInt=time;
	while(TimeInt>0);	//�J�E���^�̒l��0�ɂȂ�܂ő҂B�J�E���^��SysTick_Handler�Ō��Z����Ă���B


}



void SysTick_Handler(void)
{

	int segment_digit;	//7�Z�O�\�����ʒu�ԍ��@0��1���ځA1��2���ځA2��3���ځA3��4���ځA4�Ŗ��I��(����)
	static int lighting_state=0;//7�Z�O�\�����̓_������p�̏�ԕϐ��A�ڍׂ͉��Lswitch-case�Q��


	//
	//�҂����Ԋ֐��@Wait_Time()�p�̕ϐ��̏���
	//���荞�݊Ԋu�P�����A1��̃_�E���J�E���g�ɂP����
	if(TimeInt>0) TimeInt--;


	//7�Z�OLED�\���f�[�^����
	sec_to_mm_and_ss();


	//
	//7�Z�OLED�\������
	//

	//�e���̕\���ؑւƁA�\���ؑւ̏u�Ԃɂ�������S���\��OFF�̃u�����N�����鏈��
	switch(lighting_state){

		case 0:
			lighting_state=1;
			segment_digit=0;	//1����
			display_number_on_7SegLED(seconds_data[0], segment_digit);
			break;
		case 1:
			lighting_state=2;
			segment_digit=1;	//2����
			display_number_on_7SegLED(seconds_data[1], segment_digit);
			break;
		case 2:
			lighting_state=3;
			segment_digit=2;	//3����
			display_number_on_7SegLED(minutes_data[0], segment_digit);
			break;
		case 3:
			lighting_state=4;
			segment_digit=3;	//4����
			display_number_on_7SegLED(minutes_data[1], segment_digit);
			break;
		case 4:
			lighting_state=0;
			segment_digit=4;	//�\���u�����N
			break;
	}



}
