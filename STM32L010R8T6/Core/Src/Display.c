/*
 * Display.c
 *
 *  Created on: 2 lug 2023
 *      Author: Nicola Vaccaro
 *      LinkedIn: https://www.linkedin.com/in/nicola-vaccaro/
 */


/******************     Includes           *****************/

#include "Display.h"

#include <qrcode.h>
#include "qrcodegen.h"
#include "Io.h"
#include "main.h"
#include "Comm.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

/******************     Private  datatypes *****************/
typedef struct NewBadgeRequest
{
  bool ValidId;
  bool NewBadgeCoupled;
  uint16_t NewBadgeId;
  uint8_t NewBadgeValue;
}NewBadgeRequest_t;

/******************     Private  defines   *****************/

/* Define for Timing */
#define DISPLAY_TICK                (20u)
#define DISPLAY_TIME(X)             (X/DISPLAY_TICK)

#define DISPLAY_FONT_WELCOME        Font_11x18
#define DISPLAY_FONT_TITLE          Font_11x18
#define DISPLAY_FONT_DATA           Font_7x10
#define DISPLAY_NEW_LINE            (DISPLAY_FONT_DATA.FontHeight + 7u)
#define DISPALY_TITLE_TO_DATA_SPACE (7u)
#define DISPLAY_YELL_START_ROW      (0u)
#define DISPLAY_BLUE_START_ROW      (20u)
#define DISPLAY_DATA_START_ROW      (DISPLAY_BLUE_START_ROW)
#define DISPLAY_WELCOME_MSG_1       ("WELCOME TO")
#define DISPLAY_WELCOME_MSG_2       ("NO HAT !")
#define DISPLAY_WELCOME_ON_TIME     (2000u)
#define DISPLAY_SHOW_NEW_BADGE_TIME (2500u)

/* Define for Display pages */
#define DISPLAY_PAGES_NUM     (4u)
#define DISPLAY_PAGE_MY_INFO  (0u)
#define DISPLAY_PAGE_MY_DATA  (1u)
#define DISPLAY_PAGE_QR_CODE  (2u)
#define DISPLAY_PAGE_CREDITS  (3u)
/* Define for extra pages */
#define DISPLAY_PAGE_NEW_BADGE      (DISPLAY_PAGES_NUM)
#define DISPLAY_PAGE_INVALID_BADGE  (DISPLAY_PAGES_NUM + 1u)
#define DISPLAY_PAGE_WELCOME        (DISPLAY_PAGES_NUM + 2u)

/* Define for QR Code */
#define DISPLAY_QR_VERSION  (1u)
#define DISPLAY_QR_SIZE     ((4u*DISPLAY_QR_VERSION) + 17)

/******************     Global  variables  *****************/

NewBadgeRequest_t NewBadgeRqst;
static char encoding_table[] = 
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};
static int mod_table[] = {0, 2, 1};
char* Encoded64Data;

/******************     Private functions prototypes *****************/

void Display_NewLine(void);
void Display_SetCursorToHorCenter(uint8_t StringLenght, FontDef* Font);
void Display_SetCursorToVerCenter(uint8_t StringLenght, FontDef* Font);
void Display_ApplyPage(uint8_t Page);
void Display_EncryptDecrypt(char *input, char *output);
char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length);

/******************     Public  functions  *****************/
void Display_Init(void)
{
  // Init lcd
  ssd1306_Init();
  // Show Welcome Page
  Display_ApplyPage(DISPLAY_PAGE_WELCOME);
  // Wait 3 sec
  HAL_Delay(DISPLAY_WELCOME_ON_TIME);
  // Show Init Page
  Display_ApplyPage(DISPLAY_PAGE_MY_INFO);
}

void Display_Mng(void)
{
  static uint8_t ShowNewBadgeTimer = 0u;
  static uint8_t LastApplPage = DISPLAY_PAGE_MY_INFO;
  static uint8_t CurrentPageOld = DISPLAY_PAGE_MY_INFO;
  uint8_t CurrentPage;
  static GPIO_PinState Btn_ChangePageOld = GPIO_PIN_RESET;
  GPIO_PinState Btn_ChangePage;

  // Assign old value by default
  CurrentPage = CurrentPageOld;

  // Is there a new Badge coupled ?
  if (true == NewBadgeRqst.NewBadgeCoupled)
  {
    // Save last Appl page
    LastApplPage = CurrentPageOld;
    // Reset New Badge coupled request
    NewBadgeRqst.NewBadgeCoupled = false;
    // Set Timer
    ShowNewBadgeTimer = DISPLAY_TIME(DISPLAY_SHOW_NEW_BADGE_TIME);
    // Is Valid ID ?
    if (true == NewBadgeRqst.ValidId)
    {
      // Set New Badge Page
      CurrentPage = DISPLAY_PAGE_NEW_BADGE;
    }
    else
    {
      // Set Invalid Badge Page
      CurrentPage = DISPLAY_PAGE_INVALID_BADGE;
    }
  }

  // Is NewBadgeTimer not active ?
  if (ShowNewBadgeTimer == 0u)
  {
    // Retrieve last Appl page
    CurrentPage = LastApplPage;
    // Read IO_IN_BTN_NEXT_PAGE
    Btn_ChangePage = Io_GetInpVal(IO_IN_BTN_NEXT_PAGE);
    // Has Btn_ChangePage a new value ?
    if (Btn_ChangePageOld != Btn_ChangePage)
    {
      // Update old value
      Btn_ChangePageOld = Btn_ChangePage; 
      // Is there a ChangePage request ?
      if (GPIO_PIN_SET == Btn_ChangePage)
      {
        // Request Next Page animation
        Io_SetAnimation(IO_ANIMATION_NEXT_PAGE);
        // Set next page
        CurrentPage = (LastApplPage + 1u) % DISPLAY_PAGES_NUM;
        // Save last Appl page
        LastApplPage = CurrentPage; 
      }
    }
  }
  else
  {
    // Decrement NewBadgeTimer
    ShowNewBadgeTimer--; 
  }

  /* Is there a new Current page */
  if (CurrentPage != CurrentPageOld)
  {
    // Update old value
    CurrentPageOld = CurrentPage;
    // Apply new page
    Display_ApplyPage(CurrentPage);
  }
}

void Display_SetNewBadgeRequest(bool ValidId, uint16_t BadgeId, uint8_t BadgeVal)
{
  // Set New Badge coupling request
  NewBadgeRqst.NewBadgeCoupled = true;
  NewBadgeRqst.NewBadgeId = BadgeId;
  NewBadgeRqst.ValidId = ValidId;
  NewBadgeRqst.NewBadgeValue = BadgeVal;
 
}

/******************     Private functions  *****************/

void Display_NewLine(void)
{
  uint16_t CurrentX, CurrentY;
  // Get Display current cursor
  ssd1306_GetCursor(&CurrentX, &CurrentY);
  // Set Cursor to position (0,Actual+FontHeight)
  ssd1306_SetCursor(0,CurrentY + DISPLAY_NEW_LINE);
}

void Display_SetCursorToHorCenter(uint8_t StringLenght, FontDef* Font)
{
  volatile uint8_t CenterPos;
  uint16_t CurrentX, CurrentY;
  uint8_t FontWidth;

  /* Is text to print ? */
  if (Font == NULL)
  {
    /* Assign unitary Width because pixel will be printed */
    FontWidth = 1u;
  }
  else
  {
    /* Assign real Font Width */
    FontWidth = Font->FontWidth;
  }

  // Compute center position basing on string lenght
  CenterPos = (uint8_t)((SSD1306_WIDTH - (StringLenght*FontWidth)) / 2u);
  // Get current cursor
  ssd1306_GetCursor(&CurrentX, &CurrentY);
  // Set cursor to center
  ssd1306_SetCursor(CenterPos,CurrentY);
}

void Display_SetCursorToVerCenter(uint8_t StringLenght, FontDef* Font)
{
  volatile uint8_t CenterPos;
  uint16_t CurrentX, CurrentY;
  uint8_t FontHeight;

  /* Is text to print ? */
  if (Font == NULL)
  {
    /* Assign unitary Height because pixel will be printed */
    FontHeight = 1u;
  }
  else
  {
    /* Assign real Font Height */
    FontHeight = Font->FontHeight;
  }

  // Compute center position basing on string lenght and skip yellow space
  CenterPos = (uint8_t)((SSD1306_HEIGHT + DISPLAY_BLUE_START_ROW - (StringLenght*FontHeight)) / 2u);
  // Get current cursor
  ssd1306_GetCursor(&CurrentX, &CurrentY);
  // Set cursor to center
  ssd1306_SetCursor(CurrentX,CenterPos);
}

void Display_ApplyPage(uint8_t Page)
{
  char url[43] = "";
  enum qrcodegen_Ecc errCorLvl;
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN];
  // Must try the best len in order to reduce RAM occupation
  uint8_t qrcode[qrcodegen_BUFFER_LEN];
  char PageTitle[SSD1306_WIDTH] = {0};
  char PageData[SSD1306_WIDTH] = {0};
  uint8_t y,x;
  uint16_t CurrentX, CurrentY;
  char IdStr[6];
  char ScoreStr[6];
  char ToEncrypt[12] = "";
  size_t OutputLenghtTemp;
  
  // Clear screen
  ssd1306_Fill(0);
  // Set cursor to (0,0)
  ssd1306_SetCursor(0,0);
  
  /* Switch Page to apply */
  switch (Page)
  {
    /************* PAGE_MY_INFO ***************/
    case DISPLAY_PAGE_MY_INFO :
      // Set Title
      sprintf(PageTitle,"MY INFO");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW);
      // Set data to write
      sprintf(PageData,"My Id = %u",MyId);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to new line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"My Value = %u",MyValue);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to new line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"My Score = %u",MyScore);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
    break;
    /************* PAGE_MY_DATA ***************/
    case DISPLAY_PAGE_MY_DATA :
      // Set Title
      sprintf(PageTitle,"MY DATA");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW+6);
      // Set data to write
      sprintf(PageData,"%s",BADGE_ROLE);
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_DATA);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to new line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"%s",BADGE_OWNER);
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageData),&DISPLAY_FONT_DATA);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);    
    break;
    // /************* PAGE_MY_SCORE **************/
    // case DISPLAY_PAGE_MY_SCORE :
    //   // Set Title
    //   sprintf(PageTitle,"MY SCORE");
    //   // Set cursor to center
    //   Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
    //   // Write Page Title
    //   ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
    //   // Set cursor to Normal font begin
    //   ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW);
    //   // Set data to write
    //   sprintf(PageData,"My Score = %u",MyScore);
    //   // Set cursor to vertical center
    //   Display_SetCursorToVerCenter(1,&DISPLAY_FONT_DATA);
    //   // Write data
    //   ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
    // break;
    /************* PAGE_QR_CODE ***************/
    case DISPLAY_PAGE_QR_CODE :
      // Set Title
      sprintf(PageTitle,"SCAN ME");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_BLUE_START_ROW);
      // Prepare data
      sprintf(IdStr,"%05d",MyId);
      sprintf(ScoreStr,"%05d",MyScore);
      strcat(ToEncrypt,IdStr);
      strcat(ToEncrypt,"-");
      strcat(ToEncrypt,ScoreStr);
      // Encrypt data XOR
      Display_EncryptDecrypt(ToEncrypt,ToEncrypt);
      // Encode data 64
      Encoded64Data = base64_encode(ToEncrypt,strlen(ToEncrypt),&OutputLenghtTemp);
      Encoded64Data[OutputLenghtTemp] = '\0';
      // Generate QR Code
      strcat(url,"http://nohat.it/sb.html?v=");
      strcat(url,Encoded64Data);
      errCorLvl = qrcodegen_Ecc_LOW;
      // Create the QR code
      qrcodegen_encodeText(url, tempBuffer, qrcode, errCorLvl,qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX , qrcodegen_Mask_0, true);
      // Write QR Code
      Display_SetCursorToVerCenter(qrcodegen_getSize(qrcode),(FontDef*)NULL);
      Display_SetCursorToHorCenter(qrcodegen_getSize(qrcode),(FontDef*)NULL);
      // Get Display current cursor
      ssd1306_GetCursor(&CurrentX, &CurrentY);
      /* For each QR Code column */
      for (y = 0u; y < qrcodegen_getSize(qrcode); y++)
      {
        /* For each QR Code row */
        for (x = 0u; x < qrcodegen_getSize(qrcode); x++)
        {
          /* Is pixel to turn on ?*/
          if (qrcodegen_getModule(qrcode, x, y)==true)
          {
            // Turn on pixel
            ssd1306_DrawPixel(x+CurrentX, y+CurrentY, White);
          }
        }
      }
    break;
    /************* CREDITS ********************/
    case DISPLAY_PAGE_CREDITS :
      // Set Title
      sprintf(PageTitle,"NO HAT <3");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW);
      // Set data to write
      sprintf(PageData,"HW : Marcello F.");
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to New Line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"SW : Nicola V.");
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to New Line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"PM : Jacopo F.");
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
    break;
    /************* PAGE_NEW_BADGE *************/
    case DISPLAY_PAGE_NEW_BADGE :
      // Set Title
      sprintf(PageTitle,"NEW BADGE !");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW + DISPALY_TITLE_TO_DATA_SPACE);
      // Set data to write
      sprintf(PageData,"Badge Id = %u",NewBadgeRqst.NewBadgeId);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to new line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"Badge Pts = %u",NewBadgeRqst.NewBadgeValue);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
    break;
    /************* PAGE_INVALID_BADGE *************/
    case DISPLAY_PAGE_INVALID_BADGE :
      // Set Title
      sprintf(PageTitle,"WARNING !");
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_TITLE);
      // Write Page Title
      ssd1306_WriteString(PageTitle,DISPLAY_FONT_TITLE,White);
      // Set cursor to Normal font begin
      ssd1306_SetCursor(0,DISPLAY_DATA_START_ROW + DISPALY_TITLE_TO_DATA_SPACE);
      // Set data to write
      sprintf(PageData,"Badge %u is",NewBadgeRqst.NewBadgeId);
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
      // Go to new line
      Display_NewLine();
      // Set data to write
      sprintf(PageData,"already coupled !");
      // Write data
      ssd1306_WriteString(PageData,DISPLAY_FONT_DATA,White);
    break;
    /************* PAGE_WELCOME ***************/
    case DISPLAY_PAGE_WELCOME :
      // Set Title 1
      sprintf(PageTitle,DISPLAY_WELCOME_MSG_1);
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_WELCOME);
      // Write page Title
      ssd1306_WriteString(PageTitle, DISPLAY_FONT_WELCOME, White);
      // Set Title 2
      sprintf(PageTitle,DISPLAY_WELCOME_MSG_2);
      // Leave some space
      Display_SetCursorToVerCenter(1,&DISPLAY_FONT_WELCOME);
      // Set cursor to center
      Display_SetCursorToHorCenter(strlen(PageTitle),&DISPLAY_FONT_WELCOME);
      // Write page Title
      ssd1306_WriteString(PageTitle, DISPLAY_FONT_WELCOME, White);
    break;
  }
  // Update screen
  ssd1306_UpdateScreen();
}

void Display_EncryptDecrypt(char *input, char *output)
{
  char key[] = "nohatnohatA"; //Can be any chars, and any size array
  int i;

  for(i = 0; i < strlen(input); i++)
  {
    output[i] = input[i] ^ key[i % (sizeof(key)/sizeof(char))];
  }
}

char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}
