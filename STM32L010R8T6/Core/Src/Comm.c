/*
 * Comm.c
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */


/******************     Includes           *****************/
#include "Comm.h"
#include "Display.h"
#include "main.h"
#include "Io.h"
#include "string.h"
#include "eeprom.h"

/******************     Private  defines   *****************/

/* Maximum data lentgh (bytes) */
#define COMM_DATA_LEN  (3u)
/* Timeout to receive a Coupling request (ms) */
#define COMM_CPL_TIMEOUT  (5u)
/* Timeout to receive data from other boards (ms) */
#define COMM_DATA_TIMEOUT (5u)
/* Value for Coupling Request */
#define COMM_CPLRQST      (0xFFFFF0u)
/* Value for Coupling Acknowledge */
#define COMM_ACK          (0xFFFFF1u)

/* Comm modes */
#define COMM_SLAVE  (0u)
#define COMM_MASTER (1u)

/* Define for EEPROM */
#define COMM_EE_FOUND      (0u)

/******************     Global  variables  *****************/

/* 
  This table represents a sort of "LABELS" to retrieve data in NVM.
  Data are saved in the format {uint16_t Data - uint16_t VirtAddr}.
  For this application Data = Pts and VirtAddr = BadgeId.
  i.e. Badge Id = 1, VirtAddr[1] = 1, Data = Pts of Badge 1
                                |
                                |
                                |
  i.e. Badge Id = 512, VirtAddr[512] = 512, Data = Pts of Badge 512
  VirtAddr[0] = 0xFFFF, Data = MyScore
*/
uint16_t VirtAddVarTab[NB_OF_VAR];
/* How many points I will give to others */
uint8_t MyValue;
/* Unique Id of the Badge */
const uint16_t MyId = BADGE_ID;
/* How many points I have */
uint16_t MyScore = 0u;

/******************     Private functions prototypes *****************/
static uint32_t Comm_PackData(uint8_t* Buffer);
static bool Comm_CheckId(uint16_t Id);
static void Comm_UpdateIdList(uint16_t Id, uint8_t Pts);
static void Comm_UpdateMyScore(uint16_t MyScore);
/******************     Public  functions  *****************/
void Comm_Init(void)
{
  uint16_t Index;

  /* Set MyValue basing on Badge Type */
  if(BADGE_TYPE == BADGE_TYPE_SPEAKER)
  {
    // Set Speaker value
    MyValue = BADGE_VALUE_SPEAKER;
  }
  else if (BADGE_TYPE == BADGE_TYPE_SPONSOR)
  {
    // Set Sponsor value
    MyValue = BADGE_VALUE_SPONSOR;
  }
  else
  {
    // Set User value
    MyValue = BADGE_VALUE_USER;
  }
  /* Set Virtual Address for Badge score */
  VirtAddVarTab[0] = 0xFFFFu;
  /* Init Virtual Address Table */
  for (Index = 1u; Index <= BADGE_TOT_NUMBER; Index++)
  {
    /* Set Virtual Address (from 1 to 512) */
    VirtAddVarTab[Index] = Index;
  }
  // Read MyScore from NVM, if present
  EE_ReadVariable(0xFFFF,&MyScore);
}

// #define PROVA_FLASH
void Comm_Mng(void)
{
  static uint32_t Count=0;
  uint32_t Data;
  uint8_t Buffer[COMM_DATA_LEN];
#ifdef PROVA_FLASH
  uint8_t Mode = COMM_MASTER;
  static uint16_t Id = 0;
#else
  uint8_t Mode = 0xFFu;
  uint16_t Id;
#endif
  uint8_t Pts;
  bool ValidId;
  HAL_StatusTypeDef CommSts;

  // Look for coupling request
  CommSts = HAL_UART_Receive(&hlpuart1,Buffer,COMM_DATA_LEN,COMM_CPL_TIMEOUT);
  // Pack received data
  Data = Comm_PackData(Buffer);
  // Coupling request received ?
  if ((CommSts == HAL_OK) && (Data == COMM_CPLRQST))
  {
    // Send Acknowledge
    Data = COMM_ACK;
    memcpy(Buffer,&Data,COMM_DATA_LEN);
    HAL_UART_Transmit(&hlpuart1,Buffer,COMM_DATA_LEN,HAL_MAX_DELAY);
    // Set Slave Mode
    Mode = COMM_SLAVE;
  }
  else
  {
    // Send coupling request
    Data = COMM_CPLRQST;
    memcpy(Buffer,&Data,COMM_DATA_LEN);
    HAL_UART_Transmit(&hlpuart1,Buffer,COMM_DATA_LEN,HAL_MAX_DELAY);
    // Look for ACK
    CommSts = HAL_UART_Receive(&hlpuart1,Buffer,COMM_DATA_LEN,COMM_CPL_TIMEOUT);
    // Pack received data
    Data = Comm_PackData(Buffer);
    if ((CommSts == HAL_OK) && (Data == COMM_ACK))
    {
      Mode = COMM_MASTER;
    }
  }
  // Switch Comm Mode
  switch (Mode)
  {
    /******************     Slave Mode   *****************/
    case COMM_SLAVE:
      printf("Sono lo Slave %d\r\n",Count);
      Count++;
      // Look for data from the Master (ID and Pts)
      CommSts = HAL_UART_Receive(&hlpuart1,Buffer,COMM_DATA_LEN,COMM_DATA_TIMEOUT);
      // Is data received ?
      if (CommSts == HAL_OK)
      {
        // Set received data
        Id = (Buffer[1] << 8u) |Buffer[0];
        Pts = Buffer[2];
        ValidId = Comm_CheckId(Id);
        if (true == ValidId)
        {
          // Update list of coupled Id
          Comm_UpdateIdList(Id,Pts);
          // Send MyId + MyValue to the Master
          Data = (MyValue << 16u) | MyId;
          memcpy(Buffer,&Data,COMM_DATA_LEN);
          HAL_UART_Transmit(&hlpuart1,Buffer,COMM_DATA_LEN,HAL_MAX_DELAY);
          // Update Score adding the received points
          MyScore += Pts;
          // Update Badge score in NVM
          Comm_UpdateMyScore(MyScore);
          printf("Ricevuti %d punti dal badge n° %d\r\n",Pts,Id);
          // Set New Badge coupling request
          Display_SetNewBadgeRequest(true,Id,Pts);
          /* Play animation for New Badge coupled */
          Io_SetAnimation(IO_ANIMATION_COUPLED);
        }
        else
        {
       	  // Set Invalid Badge coupling request
          Display_SetNewBadgeRequest(false,Id,Pts);
          printf("Badge n° %d non valido o già accoppiato\r\n",Id);
        }
      }
      else
      {

        printf("Errore nella ricezione dei dati\r\n");
      }
    break;
    /******************     Master Mode  *****************/
    case COMM_MASTER:
      printf("Sono il Master %d\r\n",Count);
      Count++;
      // Send MyId + MyValue to the Slave
      Data = (MyValue << 16u) | MyId;
      memcpy(Buffer,&Data,COMM_DATA_LEN);
      HAL_UART_Transmit(&hlpuart1,Buffer,COMM_DATA_LEN,HAL_MAX_DELAY);
      // Look for data from the Slave (ID and Pts)
      CommSts = HAL_UART_Receive(&hlpuart1,Buffer,COMM_DATA_LEN,COMM_DATA_TIMEOUT);
#ifdef PROVA_FLASH
      CommSts = HAL_OK;
#endif
      // Is data received ?
      if (CommSts == HAL_OK)
      {
#ifdef PROVA_FLASH
        Id++;
        Pts = 1;
#else
        // Set received data
        Id = (Buffer[1] << 8u) |Buffer[0];
        Pts = Buffer[2];
#endif
        ValidId = Comm_CheckId(Id);
        if (true == ValidId)
        {
          // Update list of coupled Id
          Comm_UpdateIdList(Id,Pts);
          // Update Score adding the received points
          MyScore += Pts;
          // Update Badge score in NVM
          Comm_UpdateMyScore(MyScore);
          printf("Ricevuti %d punti dal badge n° %d\r\n",Pts,Id);
          // Set New Badge coupling request
          Display_SetNewBadgeRequest(true,Id,Pts);
          /* Play animation for New Badge coupled */
          Io_SetAnimation(IO_ANIMATION_COUPLED);
        }
        else
        {
          // Set New Badge coupling request
          Display_SetNewBadgeRequest(false,Id,Pts);
          printf("Badge n° %d non valido o già accoppiato\r\n",Id);
        }
      }
      else
      {
        printf("Errore nella ricezione dei dati\r\n");
      }
    break;
    /******************     No Mode      *****************/
    default:
    break;
  }
}

/******************     Private functions    *****************/
uint32_t Comm_PackData(uint8_t* Buffer)
{
  uint32_t Data = 0u;
  uint8_t Index;

  /* Is valid data ? */
  if (Buffer != NULL)
  {
    /* For each byte within given Buffer */
    for(Index = 0u; Index < COMM_DATA_LEN; Index++)
    {
      /* Pack data knowing that MSBs are in highest indexes */
      Data |= Buffer[Index] << (Index*8u);
    }
  }
  /* Return Decoded Data */
  return Data;
}

bool Comm_CheckId(uint16_t Id)
{
  bool ValidId = true;
  uint16_t Found;
  uint16_t Dummy;

  /* Is Id in valid range ? */
  if ((Id >= 1u) && (Id <= 512u))
  {
    /* Search Id into NVM */
    Found = EE_ReadVariable(Id,&Dummy);
    /* Is Id already coupled ? */
    if (COMM_EE_FOUND == Found)
    {
      /* Set Id as not valid */
      ValidId = false;
    }
  }
  else
  {
    /* Set Id as not valid */
    ValidId = false;
  }

  return ValidId;
}

static void Comm_UpdateIdList(uint16_t Id, uint8_t Pts)
{
  /* Save Id in NVM */
  EE_WriteVariable(Id,Pts);
}

static void Comm_UpdateMyScore(uint16_t MyScore)
{
  /* Save MyScore in NVM */
  EE_WriteVariable(0xFFFFu,MyScore);
}
