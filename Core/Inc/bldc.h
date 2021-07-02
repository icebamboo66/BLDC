#ifndef __BLDC_H_
#define __BLDC_H_



/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


typedef struct
{
	uint8_t EN;     		// bldcʹ��
	uint8_t Dir;     		// bldc����
	uint8_t Hall;   		// bldc HALL ״̬
	int16_t PwmRef;  	  // bldc�����ٶȲο�
	
	int16_t SpeedRef;  	// bldc�ջ��ٶ�
	int16_t SpeedBck;  	// bldc�ٶȷ���
	uint8_t Status; 		// bldc״̬

}BldcMotor_t;


extern BldcMotor_t mymotor;



// ���ڵõ�HALL������λ��
uint8_t GetBldcHall(void);

//  �������
void BldcStart( BldcMotor_t *motor,int16_t speed );
void BldcStop( BldcMotor_t *motor );
void BldcMove( BldcMotor_t *motor );
void BldcUpdataPwm( int16_t pwm );


#endif
