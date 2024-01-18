/*
 * Appl.c
 *
 *  Created on: Aug 14, 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */


/******************     Includes           *****************/
#include "Appl.h"
#include "Display.h"
#include "main.h"
#include "Io.h"

/******************     Private  defines   *****************/

/******************     Global  variables  *****************/

/******************     Private functions prototypes *****************/

/******************     Public  functions  *****************/

void Appl_Init(void)
{
#ifdef DEBUG
  volatile static uint32_t BootMode;
  BootMode = HAL_SYSCFG_GetBootMode();
#endif
  // Run Display Init
  Display_Init();
}

void Appl_Mng(void)
{
  GPIO_PinState Btn_PartyMode;
  static GPIO_PinState Btn_PartyModeOld = GPIO_PIN_RESET;
  static bool PartyModeSts = false;

  // Run Display Manager
  Display_Mng();
  // Read Read IO_IN_BTN_PARTY_MODE
  Btn_PartyMode = Io_GetInpVal(IO_IN_BTN_PARTY_MODE);
  // Has Btn_PartyMode a new value ?
  if (Btn_PartyModeOld != Btn_PartyMode)
  {
    // Update old value
    Btn_PartyModeOld = Btn_PartyMode; 
    // Is there a PartyMode request ?
    if (GPIO_PIN_SET == Btn_PartyMode)
    {
      // Toggle PartyModeSts
      PartyModeSts = !PartyModeSts;
      // Is PartyMode requested ?
      if (PartyModeSts == true)
      {
        // Request Party Mode animation
        Io_SetAnimation(IO_ANIMATION_PARTY);
      }
      else
      {
        // Turn PARTY_MODE off
        Io_SetAnimation(IO_NO_ANIMATION);
      }
    }
  }
}

/******************     Private functions    *****************/