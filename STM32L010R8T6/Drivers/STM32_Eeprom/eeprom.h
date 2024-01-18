/**
  ******************************************************************************
  * @file    EEPROM/EEPROM_Emulation/inc/eeprom.h 
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "main.h"

/* Exported constants --------------------------------------------------------*/
/* EEPROM emulation firmware error codes */
#define EE_OK      (uint32_t)HAL_OK
#define EE_ERROR   (uint32_t)HAL_ERROR
#define EE_BUSY    (uint32_t)HAL_BUSY
#define EE_TIMEOUT (uint32_t)HAL_TIMEOUT

/* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)0x800  /* Page size = 2048 bytes (2 kB) */
#define VIRT_TO_REAL_PAGE       (uint32_t)16     /* 1 virtual page is 15 real pages */

/* Device voltage range supposed to be [2.7V to 3.6V], the operation will 
   be done by word  */
#define VOLTAGE_RANGE           (uint8_t)VOLTAGE_RANGE_3

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  ((uint32_t)0x0800F000) /* EEPROM emulation start address:
                                                        sector15 Page 480 */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS))    // Pages 480 - 495
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))  // 0x0800F7FF
#define PAGE0_ID               FLASH_SECTOR_2

#define PAGE1_BASE_ADDRESS    ((uint32_t)(PAGE0_BASE_ADDRESS + PAGE_SIZE))    // Pages 496 - 511
#define PAGE1_END_ADDRESS     ((uint32_t)(PAGE0_BASE_ADDRESS + (2 * PAGE_SIZE - 1)))  // 0x800FFFF
#define PAGE1_ID               FLASH_SECTOR_3

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)0x0001) /* Page nb between PAGE0_BASE_ADDRESS & PAGE1_BASE_ADDRESS*/

/* No valid page define */
#define NO_VALID_PAGE         ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                ((uint32_t)0x00000000)     /* Page is empty */
#define RECEIVE_DATA          ((uint32_t)0xEEEEEEEE)     /* Page is marked to receive data */
#define VALID_PAGE            ((uint32_t)0xFFFFFFFF)     /* Page containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL             ((uint8_t)0x80)

/* Variables' number */
#define NB_OF_VAR             ((uint16_t)BADGE_TOT_NUMBER + 1u)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
HAL_StatusTypeDef EE_Format(void);
uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif /* __EEPROM_H */