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

/**
 * @brief  Switch on the motor driver
 * @retval None
 */
void motorPowerOn(void){
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
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // just for test, you can delete it
}

/**
 * @brief  Set the motor speed
 * @param  speed : target speed of the motor
 * @retval None
 */
void motorSetSpeed(int speed){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // just for test, you can delete it
}
