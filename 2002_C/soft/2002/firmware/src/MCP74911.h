/******************************************************************************* 
*                                                                              * 
* Auteur : Joao Marques                                                        *                     
* Fichier : MCP74911.h                                                         *
*                                                                              *
*    Ce fichier d'en-tête définit les structures et les fonctions utilisées    *
*    pour configurer et interagir avec la RTC MCP74911.                        *
*                                                                              *
*******************************************************************************/

#ifndef MCP74911_H
#define MCP74911_H

/******************************************************************************/
/* Section : Fichiers Inclus                                                  */
/******************************************************************************/

#include <stdint.h>
#include <time.h>

/******************************************************************************/
/* Section : Defines                                                          */
/******************************************************************************/

#define RTC_WRITE_ADDRESSE              0xDE
#define RTC_READ_ADDRESSE               0xDF

#define RTC_ADDRESSE_REGISTRE_CONTROL   0x07

#define RTC_ADDRESSE_HORLOGES_SEC       0x00
#define RTC_ADDRESSE_HORLOGES_MIN       0x01         
#define RTC_ADDRESSE_HORLOGES_HEURES    0x02         
#define RTC_ADDRESSE_ALARM1_SEC         0x11
#define RTC_ADDRESSE_ALARM1_MIN         0x12
#define RTC_ADDRESSE_ALARM1_HEU         0x13
#define RTC_ADDRESSE_ALARM1_WEEK        0x14
#define RTC_ADDRESSE_ALARM1_MOUNT       0x15
#define RTC_ADDRESSE_ALARM1_YEAR        0x16

//******************************************************************************
// Section : Données
//******************************************************************************

//structure avec les differente valeurs pour l'heure
typedef struct
{    
    uint8_t Seconde;   /* The application's current state */
    uint8_t Minute;
    uint8_t Heure;
    uint32_t Periode;
} ParamHorloge;

ParamHorloge RTC;

// *****************************************************************************
// Section : Fonctions
// *****************************************************************************

uint8_t decToBcd(uint8_t decimal);

uint8_t bcdToDec(uint8_t bcd);

void MCP79411_RTC_Write (void *SrcData, uint32_t RTCAddr, uint16_t NbBytes);

void MCP79411_RTC_Read(void *DstData, uint32_t RTCAddr, uint16_t NbBytes);

void MCP79411_RTC_Init(void);

void MCP79411_RTC_Init_Horloge (void);

void MCP79411_RTC_Init_Alarm2(uint32_t totalSeconds); 

void MCP79411_RTC_Read_Horloge(uint8_t *Val_Sec,uint8_t *Val_Min,uint8_t *Val_Heu);

bool MCP79411_RTC_Check_and_Update_Alarm(uint8_t time);






#endif
