/**
 ******************************************************************************
 * @file    shell.c
 * @brief   This file provides code for motor control
 ******************************************************************************
 *  Created on: Nov 7, 2022
 *      Author: nicolas and Emmanuel DUGUIN
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

/**
 * @brief  Set the cyclic ratio
 * @note   This function verify if the input is inside the cyclic ratio range and set the timer registers for PWM modification
 * @param  cyclic ratio : between 0 and 100
 * @retval None
 */
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


/**
 * @brief  Allow the saturation of the input parameter
 * @note   This function verify if the cyclic ratio range is respected. Used for uint_16_t input
 * @param  alpha : cyclic ratio
 * @retval uint16_t alpha : into the range [0;100]
 */
uint16_t verif_alpha(uint16_t alpha){
	if (alpha>=100)
		return 100;
	else if (alpha<=0)
		return 0;
	else
		return alpha;
}

/**
 * @brief  Allow the saturation of the input parameter
 * @note   This function verify if the cyclic ratio range is respected. Used for float input
 * @param  alpha : cyclic ratio
 * @retval float alpha : into the range [0;100]
 */
float verif_alpha_float(float alpha){
	if (alpha>=100)
		return 100;
	else if (alpha<=0)
		return 0;
	else
		return alpha;
}

/**
 * @brief  Allow the saturation of the input parameter
 * @note   This function verify if the required current is into the acceptable range.Used for uint_16_t input
 * @param  uint16_t current
 * @retval uint16_t current:  into the range [-8;8]
 */
uint16_t verif_current(uint16_t current){
	if (current>=8)
		return 8;
	else if (current<=-8)
		return -8;
	else
		return current;
}
/**
 * @brief  Allow the saturation of the input parameter
 * @note   This function verify if the cyclic ratio range is respected. Used for float input
 * @param  float current
 * @retval float current:  into the range [-8;8]
 */
float verif_current_float(float current){
	if (current>=8)
		return 8;
	else if (current<=-8)
		return -8;
	else
		return current;
}

