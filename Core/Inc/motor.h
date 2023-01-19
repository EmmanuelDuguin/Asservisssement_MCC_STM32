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

//---------saturration/antiwindup rapport cyclique (alpha)-------
uint16_t verif_alpha(uint16_t alpha);
float verif_alpha_float(float alpha);

//---------saturration/antiwindup courrant-------
uint16_t verif_current(uint16_t current);
float verif_current_float(float current);

#endif /* INC_MOTOR_H_ */
