/*
 * motor.h
 *
 *  Created on: Nov 8, 2022
 *      Author: nicolas
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

void motorPowerOn(void);
void motorPowerOff(void);
void motorSetSpeed(int speed);
void motorSetAlpha(int alpha);
uint16_t verif_alpha(uint16_t alpha);
float verif_alpha_float(float alpha);

#endif /* INC_MOTOR_H_ */
