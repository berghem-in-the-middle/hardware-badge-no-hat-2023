/*
 * Io.h
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */

#ifndef SRC_IO_H_
#define SRC_IO_H_

/******************     Includes           *****************/
#include "main.h"

/******************     Public Define      *****************/

/* Define for Timing */
#define IO_TICK                 (500e-6)    // Desired tick for Io_Mng()
#define IO_READ_WRITE_TICK      (1e-3)      // Desired tick for Io_ReadInputs(), Io_PlayAnimation(), Io_UpdateOutputs()
#define IO_READ_WRITE_TIME      (IO_READ_WRITE_TICK / IO_TICK)
#define IO_READ_WRITE_TICK_MS   (IO_READ_WRITE_TICK * 1000u)
#define IO_TIMER_SOURCE_FREQ    (2.097e6)   // Defined in CubeMX
#define IO_TIMER_CLK_DIVIDER    (2u)        // Defined in CubeMX
#define IO_TIMER_FREQ           (IO_TIMER_SOURCE_FREQ / IO_TIMER_CLK_DIVIDER)
#define IO_TIMER_TIMEOUT        ((IO_TICK*IO_TIMER_FREQ))

/* Configuration Parameters */
#define IO_INPUTS_NUM   (3u)
#define IO_OUTPUTS_NUM  (20u)
#define IO_OUTPUTS_LED_NUM     (20u)
#define IO_OUTPUTS_LED_IDX_MIN (0u)
#define IO_OUTPUTS_LED_IDX_MAX (19u)

/* Define for Inputs addressing */
#define IO_IN_BTN_NEXT_PAGE   (0u)
#define IO_IN_BTN_PARTY_MODE  (1u)
#define IO_IN_COMM_STROBE     (2u)

/* Define for Outputs addressing */
#define IO_OUT_LED_1   (0u)
#define IO_OUT_LED_2   (1u)
#define IO_OUT_LED_3   (2u)
#define IO_OUT_LED_4   (3u)
#define IO_OUT_LED_5   (4u)
#define IO_OUT_LED_6   (5u)
#define IO_OUT_LED_7   (6u)
#define IO_OUT_LED_8   (7u)
#define IO_OUT_LED_9   (8u)
#define IO_OUT_LED_10  (9u)
#define IO_OUT_LED_11  (10u)
#define IO_OUT_LED_12  (11u)
#define IO_OUT_LED_13  (12u)
#define IO_OUT_LED_14  (13u)
#define IO_OUT_LED_15  (14u)
#define IO_OUT_LED_16  (15u)
#define IO_OUT_LED_17  (16u)
#define IO_OUT_LED_18  (17u)
#define IO_OUT_LED_19  (18u)
#define IO_OUT_LED_20  (19u)

/* Define for Animation addressing */
#define IO_ANIMATION_COUPLED    (0u)
#define IO_ANIMATION_PARTY      (1u)
#define IO_ANIMATION_NEXT_PAGE  (2u)
#define IO_NO_ANIMATION         (0xFFu)

/******************     Public API         *****************/

void Io_Init(void);
void Io_Mng(void);
GPIO_PinState Io_GetInpVal(uint8_t Index);
void Io_SetOutVal(uint8_t Index, GPIO_PinState Value);
void Io_ToggleOut(uint8_t Index);
void Io_SetAllOutVal(GPIO_PinState Value);
void Io_SetAnimation(uint8_t AnimationIndex);

#endif /* SRC_IO_H_ */
