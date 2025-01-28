/*
 * test-8LEDs-main-01.c
 *
 *  Created on: 2023/06/26
 *      Author: x-lib
 */

/*
 *
 * �V���A���p�������ϊ���H�̃e�X�g�v���O����
 * �V�S�T�X�T�i�V���A���E�p�������ϊ�IC�j�ɁA�}�C�R������V���A���Ńf�[�^���������ރT���v���B
 *
 * ����́ALED-SW����8��LED��2�i���I�ɓ_��������B
 *
 *
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* S/P��H          NUCLEO-F103RB
 * SER		PA4	CN7-32
 * SRCLK	PA5	CN10-11
 * RCLK		PA6	CN10-13
 *
 *�@3.3�@�@�@�@�@3V3 CN7-16
 *�@GND�@�@�@�@�@GND CN7-32�Ȃ�


 * S/P��H�@�@�@LED-SW���
 *�@Q0�@�@�@�@�@�@��ԉE����LED�i�ԁj
 *�@Q1�@�@�@�@�@�@���̍��ׂ�LED�i�ԁj
 *�@�E�@�@�@�@�@�@�@�@�E
 *�@�E�@�@�@�@�@�@�@�@�E
 *�@Q8�@�@�@�@�@�@��ԍ�����LED�i�΁j
 *
 *�@3.3�s���@�@�@3.3
 *�@GND�s���@�@�@GND
 *
 *
 *
 */

//�֐��̃v���g�^�C�v�錾
void my_init_system(void);

void wait_delay(int delay);


//send_data_to_sr.c�Œ�`����Ă���֐�
extern void Send_Byte_To_SR(uint8_t bytes);


//
//���C���֐�
//
int main(void)
{

	uint16_t test_data;

	my_init_system();

	for(test_data=0x00; test_data<0x0fff; test_data++){

		Send_Byte_To_SR(~test_data);
		wait_delay(99999);
	}
	while(1);
}


//
//�n�[�h�E�F�A�̏�����
//
void my_init_system(void)
{
	//GPIO�@A��L����
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;	// GPIOA �̃N���b�N��L���ɂ���

//�|�[�gA�̓��o�͐ݒ�

//               76543210
	GPIOA->CRL=0x41114444;

}


void wait_delay(int delay){

	for(int i=0; i<delay; i++);

}




void SysTick_Handler(void)
{

}
