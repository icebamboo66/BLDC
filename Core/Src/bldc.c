#include "bldc.h"
#include "stdlib.h"
#include "tim.h"

BldcMotor_t mymotor;


// ���ڵõ�HALL������λ��
uint8_t GetBldcHall(void)
{
	uint8_t ret = 0;
	
	ret = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15);
	ret <<=1;
	ret |= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
	ret <<=1;
	ret |= HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);
	
	return ret;
}


// 120 ��HAL,�����ĵ��ֱ����������������ʣ����������Ҫ����
// ʵ�ʵ�������ʵ��ĵ�����Ӧ��ϵ�����ڸñ�����Ͻ����ʵ��޸ļ���
// �� ת
// ����#1 ����#2 ����#3		A+ 		A- 		 B+ 		B- 		C+ 		C- 		����
// 1 				0 		1 		�ر� 		�ر� 	�ر� 	��ͨ 	��ͨ 		�ر� 		��
// 0 				0 		1 		�ر� 		��ͨ 	�ر� 	�ر� 	��ͨ 		�ر� 		��
// 0 				1 		1 		�ر� 		��ͨ 	��ͨ 	�ر� 	�ر� 		�ر� 		��
// 0 				1 		0 		�ر� 		�ر� 	��ͨ 	�ر� 	�ر� 		��ͨ 		��
// 1 				1 		0 		��ͨ 		�ر� 	�ر� 	�ر� 	�ر� 		��ͨ 		��
// 1 				0 		0 		��ͨ 		�ر� 	�ر� 	��ͨ 	�ر� 		�ر� 		��

// �� ת
// ����#1 ����#2 ����#3 	A+ 		A- 			B+ 		B- 		C+ 		C- 		����
// 1 				0 		1 		�ر� 		�ر� 	��ͨ 	�ر� 	�ر� 		��ͨ 		��
// 0 				0 		1 		��ͨ 		�ر� 	�ر� 	�ر� 	�ر� 		��ͨ 		��
// 0 				1 		1 		��ͨ 		�ر� 	�ر� 	��ͨ 	�ر� 		�ر� 		��
// 0 				1 		0 		�ر� 		�ر� 	�ر� 	��ͨ 	��ͨ 		�ر� 		��
// 1 				1 		0 		�ر� 		��ͨ 	�ر� 	�ر� 	��ͨ 		�ر� 		��
// 1 				0 		0 		�ر� 		��ͨ 	��ͨ 	�ر� 	�ر� 		�ر� 		��


//  �������
void BldcMove( BldcMotor_t *motor )
{
	static uint8_t prehall = 0;
	motor->Hall = GetBldcHall();
	uint16_t Speed;
	
	motor->Dir = motor->PwmRef > 0 ? 1 : 0;
	
	Speed = abs(motor->PwmRef);
	if( Speed > 1199 ) motor->PwmRef = 1199;
	
	if( motor->EN )
	{
		TIM1->CCR1 = Speed;
		TIM1->CCR2 = Speed;
		TIM1->CCR3 = Speed;
	}
	else
	{
		TIM1->CCR1 = 0;
		TIM1->CCR2 = 0;
		TIM1->CCR3 = 0;
	}
	
	if( prehall != motor->Hall )
	{
		
		GPIOB->ODR &= ~(7<<13);
		
		if( motor->Dir )
		{
			switch( motor->Hall )
			{
				case 2:
					TIM1->CCER = 0x010;
					GPIOB->ODR |= 6<<10;
					break;

				case 6:
					TIM1->CCER = 0x001;
					GPIOB->ODR |= 5<<10;
					break;

				case 4:
					TIM1->CCER = 0x001;
					GPIOB->ODR |= 3<<10;
					break;

				case 5:
					TIM1->CCER = 0x100;
					GPIOB->ODR |= 6<<10;
					break;

				case 1:
					TIM1->CCER = 0x100;
					GPIOB->ODR |= 5<<10;
					break;

				case 3:
					TIM1->CCER = 0x010;
					GPIOB->ODR |= 3<<10;
					break;
				
				default:break;
			}
		}
		else
		{
			switch( motor->Hall )
			{
				case 5:
					TIM1->CCER = 0x010;
					GPIOB->ODR |= 6<<10;
					break;

				case 1:
					TIM1->CCER = 0x001;
					GPIOB->ODR |= 5<<10;
					break;

				case 3:
					TIM1->CCER = 0x001;
					GPIOB->ODR |= 3<<10;
					break;

				case 2:
					TIM1->CCER = 0x100;
					GPIOB->ODR |= 6<<10;
					break;

				case 6:
					TIM1->CCER = 0x100;
					GPIOB->ODR |= 5<<10;
					break;

				case 4:
					TIM1->CCER = 0x010;
					GPIOB->ODR |= 3<<10;
					break;

				default:break;
			}
		}
		prehall = motor->Hall;
	}
	
}

// 

// Commutation steps forwards
// steps  hall_a hall_b hall_c		A+ 		 A- 		 B+ 		B- 		C+ 		 C-
//   1      0 			0 		1       1  	   1       0      1     1      0
//   2      1 			0 		1       1      0       0      1     1      1
//   3      1 			0 		0       1      0       1      1     0      1
//   4      1 			1 		0       1      1       1      0     0      1
//   5      0 			1 		0       0      1       1      0     1      1
//   6      0 			1 		1       0      1       1      1     1      0

// Commutation steps reverse (switch phase 2 and 3)
// steps  hall_a hall_b hall_c		A+ 		 A- 		 B+ 		B- 		C+ 		 C-
//   1      0 			0 		1       1      1       1      0     0      1
//   2      1 			0 		1       0      1       1      0     1      1
//   3      1 			0 		0       0      1       1      1     1      0
//   4      1 			1 		0       1      1       0      1     1      0
//   5      0 			1 		0       1      0       0      1     1      1
//   6      0 			1 		1       1      0       1      1     0      1


//motor six-step control
//void BldcMove(uint8_t Dir)
//{
//  static uint8_t prehall = 0;
//  uint8_t hall = GetBldcHall();

//  if (prehall != hall)
//  {

//    GPIOB->ODR &= ~(7 << 13);
//    GPIOA->ODR &= ~(7 << 8);

//    if (Dir)
//    {
//      switch (hall)
//      {
//      case 1:
//        GPIOA->ODR |= 5 << 8; // 101
//        GPIOB->ODR |= 6 << 13; // 110
//        break;

//      case 5:
//        GPIOA->ODR |= 5 << 8; // 101
//        GPIOB->ODR |= 3 << 13; // 011
//        break;

//      case 4:
//        GPIOA->ODR |= 6 << 8; // 110
//        GPIOB->ODR |= 3 << 13; // 011
//        break;

//      case 6:
//        GPIOA->ODR |= 6 << 8; // 110
//        GPIOB->ODR |= 5 << 13; // 101
//        break;

//      case 2:
//        GPIOA->ODR |= 3 << 8; // 011
//        GPIOB->ODR |= 5 << 13; // 101
//        break;

//      case 3:
//        GPIOA->ODR |= 3 << 8; // 011
//        GPIOB->ODR |= 6 << 13; // 110
//        break;

//      default:
//        break;
//      }
//    }
//    else
//    {
//      switch (hall)
//      {
//      case 1:
//        GPIOA->ODR |= 6 << 8; // 110
//        GPIOB->ODR |= 5 << 13; // 101
//        break;

//      case 5:
//        GPIOA->ODR |= 3 << 8; // 011
//        GPIOB->ODR |= 5 << 13; // 101
//        break;

//      case 4:
//        GPIOA->ODR |= 3 << 8; // 011
//        GPIOB->ODR |= 6 << 13; // 110
//        break;

//      case 6:
//        GPIOA->ODR |= 5 << 8; // 101
//        GPIOB->ODR |= 6 << 13; // 110
//        break;

//      case 2:
//        GPIOA->ODR |= 5 << 8; // 101
//        GPIOB->ODR |= 3 << 13; // 011
//        break;

//      case 3:
//        GPIOA->ODR |= 6 << 8; // 110
//        GPIOB->ODR |= 3 << 13; // 011
//        break;

//      default:
//        break;
//      }
//    }
//    prehall = hall;
//  }
//}


//  �������
void BldcStart( BldcMotor_t *motor,int16_t speed )
{
	TIM_SlaveConfigTypeDef SlaveConfig;
	SlaveConfig.SlaveMode =  TIM_SLAVEMODE_DISABLE;
	SlaveConfig.InputTrigger = TIM_TS_TI1F_ED;
	SlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_BOTHEDGE;
	HAL_TIM_SlaveConfigSynchronization(&htim3, &SlaveConfig);
	 
  HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
	__HAL_TIM_ENABLE(&htim3);
	
	
	motor->PwmRef = speed;
	motor->EN = 1;
	BldcMove( motor );
	
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CR1  |= TIM_CR1_CEN;
}


//  �������
void BldcStop( BldcMotor_t *motor )
{
	motor->PwmRef = 0;
	motor->EN = 0;
	BldcMove( motor );
}



//  ����PWMֵ
void BldcUpdataPwm( int16_t pwm )
{
	uint16_t speed = abs(pwm);
	mymotor.PwmRef = pwm;
	mymotor.Dir = mymotor.PwmRef > 0 ? 1 : 0;

	TIM1->CCR1 = speed;
	TIM1->CCR2 = speed;
	TIM1->CCR3 = speed;
}
