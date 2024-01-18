/*
 * Io.c
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */


/******************     Includes           *****************/
#include "Io.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/******************     Private  datatypes *****************/
typedef struct Inputs
{
  GPIO_TypeDef* Port;
  uint16_t Pin;
  GPIO_PinState InstVal;
  GPIO_PinState ApplVal;
  uint8_t DebounceCnt;
  uint8_t DebounceTime;
}Inputs_t;

typedef struct Outputs
{
  GPIO_TypeDef* Port;
  uint16_t Pin;
  uint8_t  ApplVal;
}Outputs_t;

typedef struct AnimationStep
{
  uint32_t OutputMask;
  uint8_t Duration;
  bool    BuzzerStatus;
}AnimationStep_t;

typedef struct AnimationDriver
{
  uint8_t AnimationIndex;
  uint8_t StepIndex;
  uint8_t DurationCount;
}AnimationDriver_t;

/******************     Private  defines   *****************/

/* Define for Timing */
#define IO_TIME(TIME)     (TIME/IO_READ_WRITE_TICK_MS)    // TIME must be milliseconds
#define IO_DURATION_INF   (0xFFu)
#define IO_DURATION_END   (0u)

/* Define for Animations */
#define IO_MIN_DURATION             (10u)
#define IO_MAX_DURATION             (100u)
#define IO_MASK_LED(LED)            (uint32_t)(1u << LED)
#define IO_MASK_LED_GROUP_2(x,y)    (IO_MASK_LED(x) | IO_MASK_LED(y) )
#define IO_MASK_LED_GROUP_3(x,y,z)  (IO_MASK_LED(x) | IO_MASK_LED(y) | IO_MASK_LED(z))
#define IO_MASK_ALL                 (0x000FFFFFu)
#define IO_MASK_NONE                (0x00000000u)

#define IO_GET_BIT_VAL(ByteVal,Index)  ( (ByteVal & (1u << Index)) >> Index )

/******************     Global  variables  *****************/
Inputs_t Io_Inputs[IO_INPUTS_NUM] =
{
  //         Port                  Pin               InstVal         ApplVal      DebounceCnt   DebounceTime
  { In_Btn_NextPage_GPIO_Port,   In_Btn_NextPage_Pin,   GPIO_PIN_RESET, GPIO_PIN_RESET, 0u,     IO_TIME(20) }, /* IO_IN_BTN_NEXT_PAGE  (PC13) */
  { In_Btn_PartyMode_GPIO_Port,  In_Btn_PartyMode_Pin,  GPIO_PIN_RESET, GPIO_PIN_RESET, 0u,     IO_TIME(20) }, /* IO_IN_BTN_PARTY_MODE (PC1) */
  { In_CommStrobe_GPIO_Port,     In_CommStrobe_Pin,     GPIO_PIN_RESET, GPIO_PIN_RESET, 0u,     IO_TIME(20) } /*  IO_IN_COMM_STROBE    (PC2) */
};

Outputs_t Io_Outputs[IO_OUTPUTS_NUM] =
{
  // {GreenLed_GPIO_Port, GreenLed_Pin, GPIO_PIN_RESET }     /* GreenLed (PA5) */
  {Out_LED1_GPIO_Port, Out_LED1_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_1 */
  {Out_LED2_GPIO_Port, Out_LED2_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_2 */
  {Out_LED3_GPIO_Port, Out_LED3_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_3 */
  {Out_LED4_GPIO_Port, Out_LED4_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_4 */
  {Out_LED5_GPIO_Port, Out_LED5_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_5 */
  {Out_LED6_GPIO_Port, Out_LED6_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_6 */
  {Out_LED7_GPIO_Port, Out_LED7_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_7 */
  {Out_LED8_GPIO_Port, Out_LED8_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_8 */
  {Out_LED9_GPIO_Port, Out_LED9_Pin, GPIO_PIN_RESET },     /* IO_OUT_LED_9 */
  {Out_LED10_GPIO_Port, Out_LED10_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_10 */
  {Out_LED11_GPIO_Port, Out_LED11_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_11 */
  {Out_LED12_GPIO_Port, Out_LED12_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_12 */
  {Out_LED13_GPIO_Port, Out_LED13_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_13 */
  {Out_LED14_GPIO_Port, Out_LED14_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_14 */
  {Out_LED15_GPIO_Port, Out_LED15_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_15 */
  {Out_LED16_GPIO_Port, Out_LED16_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_16 */
  {Out_LED17_GPIO_Port, Out_LED17_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_17 */
  {Out_LED18_GPIO_Port, Out_LED18_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_18 */
  {Out_LED19_GPIO_Port, Out_LED19_Pin, GPIO_PIN_RESET },   /* IO_OUT_LED_19 */
  {Out_LED20_GPIO_Port, Out_LED20_Pin, GPIO_PIN_RESET }    /* IO_OUT_LED_20 */
};

/* Animaton for New Badge coupled */
static AnimationStep_t IO_AnimationCoupled[] =
{
//      OutputMask                    Duration       BuzzerStatus
  { IO_MASK_LED_GROUP_3(0,1,2),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(3,4,5),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(6,7,8),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(9,10,11),   IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(12,13,14),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(15,16,17),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_2(18,19),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(0,1,2),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(3,4,5),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(6,7,8),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(9,10,11),   IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(12,13,14),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(15,16,17),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_2(18,19),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(0,1,2),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(3,4,5),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(6,7,8),     IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(9,10,11),   IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(12,13,14),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_3(15,16,17),  IO_TIME(20),        true  },
  // { IO_MASK_NONE,                   IO_TIME(20),        true  },
  { IO_MASK_LED_GROUP_2(18,19),     IO_TIME(20),        true  },
  { IO_MASK_NONE,                   IO_DURATION_END,    false }
};

/* Animaton for New Badge coupled */
static AnimationStep_t IO_AnimationParty[] =
{
//      OutputMask                    Duration    BuzzerStatus
  { IO_MASK_LED_GROUP_3(0,1,2),     IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(3,4,5),     IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(6,7,8),     IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_NONE,                   IO_TIME(50),    true },
  { IO_MASK_LED_GROUP_3(9,10,11),   IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(12,13,14),  IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(15,16,17),  IO_TIME(10),      false },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_2(18,19),     IO_TIME(10),      false },
  { IO_MASK_NONE,                   IO_TIME(50),    true },
  { IO_MASK_NONE,                   IO_TIME(250),    false },
  { IO_MASK_NONE,                   IO_DURATION_INF,  false }
};

/* Animaton for Next page */
static AnimationStep_t IO_AnimationNextPage[] =
{
//      OutputMask                    Duration    BuzzerStatus
  { IO_MASK_LED_GROUP_3(0,1,2),     IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(3,4,5),     IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(6,7,8),     IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(9,10,11),   IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(12,13,14),  IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_3(15,16,17),  IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(20),      true },
  { IO_MASK_LED_GROUP_2(18,19),     IO_TIME(5),      true },
  // { IO_MASK_NONE,                   IO_TIME(250),    false },
  { IO_MASK_NONE,                   IO_DURATION_END,  false }
};

static AnimationStep_t* IO_AnimationList[] =
{
  IO_AnimationCoupled,
  IO_AnimationParty,
  IO_AnimationNextPage
};

AnimationDriver_t IO_AnimationDriver = { .AnimationIndex = IO_NO_ANIMATION};
bool Io_BuzzerStatus = false;

/******************     Private functions prototypes  *****************/

static void Io_RandomAnimation(uint16_t Duration);
static void Io_UpdateOutputs(void);
static void Io_ReadInputs(void);
static void Io_PlayAnimation(void);
static void Io_DriveBuzzer();
static AnimationStep_t* IO_GetAnimationStep(uint8_t StepIndex);
static void IO_ApplyOutMask(uint32_t OutputMask);
static void Io_SetBuzzer(bool Status);

/******************     Public  functions  *****************/
void Io_Init(void)
{
  // Generate random seed for rand() function
  srand(BADGE_TYPE);
  uint16_t Duration = 1000u;
  // Turn on/off random lights
  Io_RandomAnimation(Duration);
}

void Io_Mng(void)
{
  static uint8_t Io_ReadWriteTimer = IO_READ_WRITE_TIME;

  /* Drive Buzzer */
  Io_DriveBuzzer();
  // Decrement ReadWrite timer
  Io_ReadWriteTimer--;
  // Is ReadWrite timer expired ?
  if (Io_ReadWriteTimer == 0u)
  {
    // Reset ReadWrite timer
	  Io_ReadWriteTimer = IO_READ_WRITE_TIME;
    /* Read inputs */
    Io_ReadInputs();
    /* Play Animation */
    Io_PlayAnimation();
    /* Update outputs */
    Io_UpdateOutputs();
  }
}

GPIO_PinState Io_GetInpVal(uint8_t Index)
{
  return Io_Inputs[Index].ApplVal;
}

void Io_SetOutVal(uint8_t Index, GPIO_PinState Value)
{
  /* Update global outputs value */
  Io_Outputs[Index].ApplVal = Value;
}

void Io_ToggleOut(uint8_t Index)
{
  /* Toggle output value */
  Io_Outputs[Index].ApplVal = !(Io_Outputs[Index].ApplVal);
}

void Io_SetAllOutVal(GPIO_PinState Value)
{
  uint8_t Index;

  for(Index=0u; Index < IO_OUTPUTS_NUM; Index++)
  {
    Io_SetOutVal(Index, Value);
  }
}

/******************     Private functions  *****************/
static void Io_PlayAnimation(void)
{
  AnimationStep_t* AnimationStep;

  /* Is there any animation to play ? */
  if (IO_AnimationDriver.AnimationIndex != IO_NO_ANIMATION)
  {
    /* Is Step ongoing ? */
    if (IO_AnimationDriver.DurationCount > 0)
    {
      // Keep playing Step
      IO_AnimationDriver.DurationCount -= 1u;
    }
    /* Go to next Step */
    else
    {
      // Increment Step index
      IO_AnimationDriver.StepIndex += 1u;
      AnimationStep = IO_GetAnimationStep(IO_AnimationDriver.StepIndex);
      /* Infinite animation ? */
      if (AnimationStep->Duration == IO_DURATION_INF)
      {
        // Reset Step pointer
        IO_AnimationDriver.StepIndex = 0u;
        AnimationStep = IO_GetAnimationStep(IO_AnimationDriver.StepIndex);
      }
      /* Animation finished ? */
      if (AnimationStep->Duration == IO_DURATION_END)
      {
        // Reset LedAnimationDriver to stop playing
        IO_AnimationDriver.AnimationIndex = IO_NO_ANIMATION;
      }
      else
      {
        // Load Step duration
        IO_AnimationDriver.DurationCount = AnimationStep->Duration;
        IO_AnimationDriver.DurationCount -= 1u;
      }
      // Set Buzzer status for Step
      Io_SetBuzzer(AnimationStep->BuzzerStatus);
      // Reproduce Step
      IO_ApplyOutMask(AnimationStep->OutputMask);
    }
  }
}

static void Io_DriveBuzzer()
{
  // Is Buzzer active ?
  if (Io_BuzzerStatus == true)
  {
    // Toggle buzzer
    HAL_GPIO_TogglePin(Out_TempBuzzer_GPIO_Port, Out_TempBuzzer_Pin);
  }
  else
  {
    // Set buzzer low
    HAL_GPIO_WritePin(Out_TempBuzzer_GPIO_Port, Out_TempBuzzer_Pin, GPIO_PIN_RESET);
  }
}

static AnimationStep_t* IO_GetAnimationStep(uint8_t StepIndex)
{
  AnimationStep_t* pAnimation;

  // Addres the correct AnimationStep
  pAnimation = (AnimationStep_t*)&IO_AnimationList[IO_AnimationDriver.AnimationIndex][StepIndex];

  // Return the AnimationStep pointer
  return pAnimation;
}

static void IO_ApplyOutMask(uint32_t OutputMask)
{
  uint8_t Index;
  uint8_t OutVal;

  // For each Output
  for(Index = 0u; Index < IO_OUTPUTS_LED_NUM; Index++)
  {
    // Mask the desired output value
    OutVal = IO_GET_BIT_VAL(OutputMask,Index);
    // Set Output
    Io_SetOutVal(Index + IO_OUTPUTS_LED_IDX_MIN, (GPIO_PinState)OutVal);
  }
}

void Io_SetAnimation(uint8_t AnimationIndex)
{
  uint8_t Index;

  /* Set AnimationIndex to play */
  IO_AnimationDriver.AnimationIndex = AnimationIndex;
  IO_AnimationDriver.StepIndex = 0xFFu;
  IO_AnimationDriver.DurationCount = 0u;

  // Is no animation requested ?
  if (AnimationIndex == IO_NO_ANIMATION)
  {
    // Deactivate Buzzer
    Io_SetBuzzer(false);

    // No animation requested, turn off LEDs
    for(Index = 0u; Index < IO_OUTPUTS_LED_NUM; Index++)
    {
      // Turn off LED
      Io_SetOutVal(Index + IO_OUTPUTS_LED_IDX_MIN, GPIO_PIN_RESET);
    }
  }
}

static void Io_RandomAnimation(uint16_t Duration)
{
  uint8_t IoIndex;
  uint16_t IoDuration;
  bool BuzzerStatus = false;

  // Keep going until Animation duration goes to 0
  while (Duration > 0u)
  {
    // Pick random output
    IoIndex = rand() % IO_OUTPUTS_LED_NUM;
    // Pick random duration (ms)
    IoDuration = (rand() % (IO_MAX_DURATION - IO_MIN_DURATION + 1u)) + IO_MIN_DURATION;
    printf(" IoIndex = %d, IoDuration = %d\r\n", IoIndex, IoDuration);
    // Is Duration higher than random ?
    if (Duration > IoDuration)
    {
      // Decreas Animation duration
      Duration -= IoDuration;
    }
    else
    {
      // End Animation
      Duration = 0u;
    }
    // Toggle LED
    Io_ToggleOut(IoIndex);
    // Toggle Buzzer
    Io_SetBuzzer(!BuzzerStatus);
    // Wait for random duration
    HAL_Delay(IoDuration);
  }

  // At end of Animation switch Buzzer off
  Io_SetBuzzer(false);
  // At end of Animation turn lights off
  Io_SetAllOutVal(GPIO_PIN_RESET);
}

static void Io_UpdateOutputs(void)
{
  uint8_t Index;
  // Apply Application value to all outputs
  for (Index = 0; Index < IO_OUTPUTS_NUM; Index++)
  {
    HAL_GPIO_WritePin(Io_Outputs[Index].Port, Io_Outputs[Index].Pin, Io_Outputs[Index].ApplVal);
  }
}

static void Io_ReadInputs(void)
{
  uint8_t Index;

  // Read all inputs and update global Inputs value
  for (Index = 0u; Index < IO_INPUTS_NUM; Index++)
  {
    /* Read Instant value */
    Io_Inputs[Index].InstVal = HAL_GPIO_ReadPin(Io_Inputs[Index].Port, Io_Inputs[Index].Pin);
    /* Is instant value different from Applicative value ? */
    if (Io_Inputs[Index].InstVal != Io_Inputs[Index].ApplVal)
    {
      /* Update Debounce counter */
      Io_Inputs[Index].DebounceCnt += 1u;
      /* Is Debounce counter elapsed ? */
      if(Io_Inputs[Index].DebounceCnt == Io_Inputs[Index].DebounceTime)
      {
        /* Update Applicative Value */
        Io_Inputs[Index].ApplVal = Io_Inputs[Index].InstVal;
        /* Reset Debounce counter */
        Io_Inputs[Index].DebounceCnt = 0u;
      }
    }
    /* Instant value is equal to Applicative value */
    else
    {
      /* Reset Debounce counter */
      Io_Inputs[Index].DebounceCnt = 0u;
    }
  }
}

static void Io_SetBuzzer(bool Status)
{
  // Set Buzzer status
  Io_BuzzerStatus = Status;
}
