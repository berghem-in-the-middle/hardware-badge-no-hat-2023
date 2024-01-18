/*
 * Display.h
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

/******************     Includes           *****************/
#include "main.h"

/******************     Public API         *****************/
void Display_Init(void);
void Display_Mng(void);
void Display_SetNewBadgeRequest(bool ValidId, uint16_t BadgeId, uint8_t BadgeVal);

#endif /* SRC_DISPLAY_H_ */
