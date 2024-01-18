/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
int _write(int file, char *ptr, int len);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define In_Btn_NextPage_Pin GPIO_PIN_0
#define In_Btn_NextPage_GPIO_Port GPIOC
#define In_Btn_PartyMode_Pin GPIO_PIN_1
#define In_Btn_PartyMode_GPIO_Port GPIOC
#define In_CommStrobe_Pin GPIO_PIN_2
#define In_CommStrobe_GPIO_Port GPIOC
#define Out_Buzzer_Pin GPIO_PIN_0
#define Out_Buzzer_GPIO_Port GPIOA
#define Debug_UART2_TX_Pin GPIO_PIN_2
#define Debug_UART2_TX_GPIO_Port GPIOA
#define Debug_UART2_RX_Pin GPIO_PIN_3
#define Debug_UART2_RX_GPIO_Port GPIOA
#define Display_SPI1_SCK_Pin GPIO_PIN_5
#define Display_SPI1_SCK_GPIO_Port GPIOA
#define Display_SPI1_CS_Pin GPIO_PIN_6
#define Display_SPI1_CS_GPIO_Port GPIOA
#define Display_SPI1_MOSI_Pin GPIO_PIN_7
#define Display_SPI1_MOSI_GPIO_Port GPIOA
#define Display_RST_Pin GPIO_PIN_4
#define Display_RST_GPIO_Port GPIOC
#define Display_DC_Pin GPIO_PIN_5
#define Display_DC_GPIO_Port GPIOC
#define Data_UART1_TX_Pin GPIO_PIN_10
#define Data_UART1_TX_GPIO_Port GPIOB
#define Data_UART1_RX_Pin GPIO_PIN_11
#define Data_UART1_RX_GPIO_Port GPIOB
#define Out_LED12_Pin GPIO_PIN_12
#define Out_LED12_GPIO_Port GPIOB
#define Out_LED11_Pin GPIO_PIN_13
#define Out_LED11_GPIO_Port GPIOB
#define Out_LED10_Pin GPIO_PIN_14
#define Out_LED10_GPIO_Port GPIOB
#define Out_LED9_Pin GPIO_PIN_15
#define Out_LED9_GPIO_Port GPIOB
#define Out_LED8_Pin GPIO_PIN_6
#define Out_LED8_GPIO_Port GPIOC
#define Out_LED7_Pin GPIO_PIN_7
#define Out_LED7_GPIO_Port GPIOC
#define Out_LED6_Pin GPIO_PIN_8
#define Out_LED6_GPIO_Port GPIOC
#define Out_LED5_Pin GPIO_PIN_9
#define Out_LED5_GPIO_Port GPIOC
#define Out_LED4_Pin GPIO_PIN_8
#define Out_LED4_GPIO_Port GPIOA
#define Out_LED3_Pin GPIO_PIN_9
#define Out_LED3_GPIO_Port GPIOA
#define Out_LED2_Pin GPIO_PIN_10
#define Out_LED2_GPIO_Port GPIOA
#define Out_LED1_Pin GPIO_PIN_11
#define Out_LED1_GPIO_Port GPIOA
#define Out_TempBuzzer_Pin GPIO_PIN_12
#define Out_TempBuzzer_GPIO_Port GPIOA
#define Out_LED20_Pin GPIO_PIN_10
#define Out_LED20_GPIO_Port GPIOC
#define Out_LED19_Pin GPIO_PIN_11
#define Out_LED19_GPIO_Port GPIOC
#define Out_LED18_Pin GPIO_PIN_12
#define Out_LED18_GPIO_Port GPIOC
#define Out_LED17_Pin GPIO_PIN_2
#define Out_LED17_GPIO_Port GPIOD
#define Out_LED16_Pin GPIO_PIN_3
#define Out_LED16_GPIO_Port GPIOB
#define Out_LED15_Pin GPIO_PIN_4
#define Out_LED15_GPIO_Port GPIOB
#define Out_LED14_Pin GPIO_PIN_5
#define Out_LED14_GPIO_Port GPIOB
#define Out_LED13_Pin GPIO_PIN_6
#define Out_LED13_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* Total number of Badges */
#define BADGE_TOT_NUMBER    (512)     /* From 1 to 512 */
/* Type of Badge */
#define BADGE_TYPE_SPEAKER  (0u)
#define BADGE_TYPE_SPONSOR  (1u)
#define BADGE_TYPE_USER     (2u)

/* Value for Badge types */
#define BADGE_VALUE_SPEAKER  (30u)
#define BADGE_VALUE_SPONSOR  (10u)
#define BADGE_VALUE_USER     (3u)

/* Badge properties */
#define BADGE_TYPE  (BADGE_TYPE_SPEAKER)
#define BADGE_ID    (1u)  /* Valid range is 1 <= BADGE_ID <= 512 */
#if BADGE_TYPE == BADGE_TYPE_SPEAKER
  #define BADGE_ROLE  ("SPEAKER")
#elif BADGE_TYPE == BADGE_TYPE_SPONSOR
  #define BADGE_ROLE  ("SPONSOR")
#elif BADGE_TYPE == BADGE_TYPE_USER
  #define BADGE_ROLE  ("PARTICIPANT")
#endif
#define BADGE_OWNER ("Alessandro")

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
