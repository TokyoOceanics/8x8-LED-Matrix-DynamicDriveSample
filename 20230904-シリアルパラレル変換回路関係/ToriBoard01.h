/*
 * ToriBoard01.h
 *
 *  Created on: 2023/06/29
 *      Author: x-lib
 */

#ifndef TORIBOARD01_H_
#define TORIBOARD01_H_



//�VsegLED�̕\���f�[�^�A���_��
//int seg7[]={0xc0, 0xf9, 0xa4,0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};

//���_���L�q�̏ꍇ
int seg7[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

int seven_segment_led[5];


//7segLED�̕\�����I���A���_��
uint16_t disp_digit[]={
	//	  FEDCBA9876543210
		0b1101111111111111,//1���ڂ̕\��
		0b1110111111111111,//2���ڂ̕\��
		0b1111011111111111,//3���ڂ̕\��
		0b1111101111111111,//4���ڂ̕\��
		0b1111101111111111};//  �S����   <-- BUG? miss? 2023/08/25 ito
//



#endif /* TORIBOARD01_H_ */
