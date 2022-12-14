/**
 ******************************************************************************
 * @file    shell.c
 * @brief   This file provides code for motor control
 ******************************************************************************
 *  Created on: Nov 7, 2022
 *      Author: nicolas
 *
 ******************************************************************************
 */

#include "motor.h"
#include "tim.h"
#include "shell.h"

//Private variable begin

int speed_value=5311/2;
//Private variable end

/**
 * @brief  Switch on the motor driver
 * @retval None
 */
void motorPowerOn(void){
	motorSetAlpha(50);

	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_SET); // just for test, you can delete it
	for(int i=0;i<30;i++){}
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_RESET);

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_2);
}

/**
 * @brief  Switch off the motor driver
 * @retval None
 */
void motorPowerOff(void){
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_SET); // just for test, you can delete it
	for(int i=0;i<30;i++){}
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_RESET);

	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_2);
}

/**
 * @brief  Set the motor speed
 * @param  speed : target speed of the motor
 * @retval None
 */
void motorSetSpeed(int speed){

}

/** Fonction qui verfie dans un premier temps que la valeur de alpha est valable puis entre dans les registres CRR1 et CRR2 des CH 1 et 2**/
void motorSetAlpha(int alpha){
	int pulse_value=0;
	if (alpha>100 || alpha<0) {
		shellCmdNotFound();
		}
		else {
			pulse_value=(int) 5311*alpha/100;
			TIM1->CCR1 =pulse_value;
			TIM1->CCR2 =5311-pulse_value;
		}
}



