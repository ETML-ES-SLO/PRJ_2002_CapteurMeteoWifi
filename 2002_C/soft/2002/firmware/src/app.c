/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/



// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"

#include "Mc32gest_RS232.h"
#include "Mc32gest_I2C.h"
#include "Mc32NVMUtil.h"
#include "Mc32DriverAdc.h"

#include "MCP74911.h"
#include "ESP32.h"
#include "BME280.h"
#include "ENS160.h"

#include "Gest_Menu.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


//
APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************


void APP_Tasks ( void )
{
    static uint32_t cnt = 0;
//    uint8_t ValBrute_Sec =0;
//    uint8_t ValBrute_Min =0;
//    uint8_t ValBrute_Heu =0;

    switch ( appData.state )
    {
        
        case APP_STATE_INIT:
        {
            //Initialisation des fifo SW
            FifoComm_Init();
            
            // Initialisation de l'état du menu au démarrage
            Menu_init();
            
            // Alumer la lumiere rouge de la LED RGB
            LED_RGB_ROff();
            LED_RGB_BOn();
            LED_RGB_GOn();  
            
            I2C_Init();
            // Initialisation de l'ESP32
            ESP32_Init();
            // Initialisation du capteur BM280
            BME280_Init();
            // Initialisation du capteur ENS160
            ENS160_Init();
            
//            MCP79411_RTC_Init();
//            MCP79411_RTC_Init_Horloge();
//            MCP79411_RTC_Init_Alarm2(RTC.Periode);
            
            //Init_MCP79411RTC();
            // Eteindre la LED RGB
            LED_RGB_ROn();
            LED_RGB_BOn();
            LED_RGB_GOn(); 
            
            // Demarer le timer 0
            DRV_TMR0_Start();
            
            BSP_InitADC10();
            // State prend l'etat WAIT
            APP_UpdateState(APP_STATE_SERVICE_WAIT);
            break;
        }
        
        case APP_STATE_SERVICE_WAIT:
        {          
            break;
        }
        
        case APP_STATE_SERVICE_TASKS:
        {
            BME280.Period = 60;

            // Gestion du menu pour PC-Systeme
            State_Menu();
            // Gestion de la led RGB
            GestLed();
//            MCP79411_RTC_Read_Horloge(&ValBrute_Sec,&ValBrute_Min,&ValBrute_Heu);
//
//            //traitemannt des valeur brute
//            //Seconde
//            RTC.Seconde = bcdToDec(ValBrute_Sec&0x7F);
//            //Minute
//            RTC.Minute = bcdToDec(ValBrute_Min&0x7F);
//            //Heure
//            RTC.Heure = bcdToDec(ValBrute_Heu&0x3F);
//  
//            //Control flag interupt alarm et mis a jour de l'alarme
//            MCP79411_RTC_Check_and_Update_Alarm(RTC.Periode);
            
            // Si temps pas arrive en continu de compter
            if (cnt < BME280.Period * FREQ_SERVICE_TASKS) //changer par RTC.Periode si temps 
            {
                // incremente le counter
                cnt++;
            }
            else
            {
                // Remet a zero le counter
                cnt = 0; 
                //**** Lecture capteur BM280
                // Lecture temperature
                BME280.Temperature = BME280_readTemperature();
                // Lecture humidite
                BME280.Humidite = BME280_readHumidity();
                BME280_HumidityStatus(BME280.Humidite);
                // Lecture pression
                BME280.Pression = BME280_readPressure();
                BME280_Forecast(BME280.Pression);
                
                //**** Lecture capteur ENS160
                // Lecture qualite d'air
                ENS160.Indice_Qualite_Air = ENS160_Read_Data_Aqi();
                ENS160.Concentration_Ppb = ENS160_Read_Data_Tvoc();
                ENS160.Concentration_Co2 = ENS160_Read_Data_Eco2();
                //**** Traitement d'envoie de donné par ESP32
                // publie les data au serveur mosquitto
                SetupMQTTAndPublish(BME280.Temperature, BME280.Humidite, 
                                    BME280.Pression, ENS160.Indice_Qualite_Air, 
                                    ENS160.Concentration_Co2); 
                
                //Envoie des données en TCP
                //ESP32_SendTCP();  
                
            }
            
           APP_UpdateState(APP_STATE_SERVICE_WAIT); 
            break;
        }

        default:
        {
           
            break;
        }
    }
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : APP_UpdateState                                         *                                    
* Entrée : APP_STATES newState - le nouvel état à appliquer à l'application    *                                                                      
* Sortie : Aucun                                                               *
* Description : Cette fonction met à jour l'état actuel de l'application en    *
*               assignant la nouvelle valeur d'état passée en paramètre.       *
*                                                                              * 
*******************************************************************************/
 void APP_UpdateState(APP_STATES newState)
{
    appData.state = newState;
}
 
/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : GestLed                                                 *                                    
* Entrée : Aucun                                                               *                                                                      
* Sortie : Aucun                                                               *
* Description : Cette fonction gère le clignotement des LEDs en fonction des   *
*               états des capteurs BME280 et ENS160, de l'ESP32 ainsi que      *
*               le niveau inferieur a 30% des piles.                           *
*                                                                              * 
*******************************************************************************/ 
void GestLed(void)
{
    static uint16_t cnt = 0;
    
    //compteur sur 5 sec pour clignotement
    if (cnt<499)
        cnt++;
    else
        cnt = 0;
    
    switch(cnt)
    {
        case 100:   // etat pile
            if (BME280.Status)
                LED_RGB_ROff();
            break;
        case 150:
            LED_RGB_ROn();
            break;
        case 200:   // etat mesure
            if (ESP32.Status)
                LED_RGB_GOff();
            break;
        case 250:
            LED_RGB_GOn();
            break;
//        case 300:   // esp32
//            if (BME280.Status)
//                LED_RGB_ROff();
//            break;
//        case 350:
//            LED_RGB_ROn();
//            break;
//        case 400:   // Save
//            if (ESP32.Status)
//                LED_RGB_BOff();
//            break;
//        case 450:
//            LED_RGB_BOn();
//            break;       
    }
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : GestAlim                                                *                                    
* Entrée : Aucun                                                               *                                                                      
* Sortie : Aucun                                                               *
* Description : Cette fonction gère la gestion d'alimention, detction des type *
*               de pile ainsi que le controle du niveaux de charge.            *
*               Si niveaux trop bas arret com ESP ainsi que uC endormire       *
*                                                                              * 
*******************************************************************************/ 
uint8_t GestAlim(void)
{
    uint8_t statut = 0;
//    uint16_t Cel1 = 0;
//    uint16_t Cel2 = 0;
    uint16_t Cel3 = 0;
    uint16_t Cel4 = 0;
    // Lecture ADC des 4 pile
    appData.ValAdc = BSP_ReadAllADC();
    Cel4 = appData.ValAdc.Chan0*(OutMax/ValAdcMax);
    Cel3 = appData.ValAdc.Chan1*(OutMax/ValAdcMax);
//    Cel2 = appData.ValAdc.Chan2*(OutMax/ValAdcMax);
//    Cel1 = appData.ValAdc.Chan3*(OutMax/ValAdcMax);
    // Gestion RTC
    
    // Detection type de pile
    if((Cel4 > 1350) && (Cel3 > 2700))
    {
        // Deconecter les pile 
           
    }      
  
    // Controle niveaux des pile
    if((Cel4 < 1200) && (Cel3 > 2400))
    {
        // Satut pile a 1
        statut = 1;
        // Areter communication wifi
        
        // Endormir uC   
    }

    return statut;
}

/*******************************************************************************
 End of File
 */
