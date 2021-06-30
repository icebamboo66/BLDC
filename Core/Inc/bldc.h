#ifndef __BLDC_H_
#define __BLDC_H_



/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


typedef struct
{
	uint8_t EN;     		// bldc使能
	uint8_t Dir;     		// bldc方向
	uint8_t Hall;   		// bldc HALL 状态
	int16_t PwmRef;  	  // bldc开环速度参考
	
	int16_t SpeedRef;  	// bldc闭环速度
	int16_t SpeedBck;  	// bldc速度反馈
	uint8_t Status; 		// bldc状态

}BldcMotor_t;


extern BldcMotor_t mymotor;



// 用于得到HALL传感器位置
uint8_t GetBldcHall(void);

//  电机驱动
void BldcStart( BldcMotor_t *motor,int16_t speed );
void BldcStop( BldcMotor_t *motor );
void BldcMove( BldcMotor_t *motor );
void BldcUpdataPwm( int16_t pwm );


#endif
