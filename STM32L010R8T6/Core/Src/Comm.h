/*
 * Comm.h
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */

#ifndef SRC_COMM_H_
#define SRC_COMM_H_

/******************     Includes           *****************/
#include "main.h"

/******************     Public API         *****************/
/* How many points I will give to others */
extern uint8_t MyValue;
/* Unique Id of the Badge */
extern const uint16_t MyId;
/* How many points I have */
extern uint16_t MyScore;

void Comm_Init(void);
void Comm_Mng(void);

#endif /* SRC_COMM_H_ */
