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

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "BME280.h"
#include "ESP8266.h"
#include "Mc32gestI2cSensor.h"
#include "Mc32gest_RS232.h"
#include "menu.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;


//prototypes fonctions internes
void APP_UpdateState (APP_STATES NewState);
void GestLed(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    appData.flagInt = 0;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    static uint32_t cnt = 0;
    
    // timer écoulé ?
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_TIMER_2);
    if (appData.flagInt > 0)
    {
        appData.flagInt--;
        appData.state = APP_STATE_SERVICE_TASKS;
    }
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_TIMER_2);
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {                      
            //Initialisation des fifo SW
            FifoComm_Init();
            
            MENU_Init();    //Init de l'état du menu au démarrage
            
            //rouge pour indiquer init
            LED_R_Off();
            LED_G_On();
            LED_B_On();            
            //Initialisation des Modules           
            BME280_Init();
            ESP8266_Init();
                        
            // boot terminé : éteint tout
            LED_R_On();
            LED_G_On();
            LED_B_On();    
            
            //starte timer pour app
            DRV_TMR1_Start();
      
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        
        case APP_STATE_WAIT:
        {
            //do nothing            
            break;
        }
        
        case APP_STATE_SERVICE_TASKS:
        {
            //gestion led RGB pour signalisation état
            GestLed();
            
            //gestion du menu
            MENU_Mgmt();
            
            //mesure selon intervalle [s] souhaité
            if (cnt < BME280.Period * FREQ_SERVICE_TASKS)  
            {
                cnt++;
            }
            else    //si intervalle de mesure atteint
            {
                cnt = 0;
                
                // mesure
                static uint16_t countForecast = 0;
            
                //Lecture des données du BME280
                BME280.Temperature = BME280_readTemperature();
                BME280.Humidite = BME280_readHumidity();
                BME280.Pression = BME280_readPressure();

                BME280_HumidityStatus(BME280.Humidite); 

                //todo contrôler comptabilisation minutes.
                //test si ça fait 20 minutes
                if(countForecast >= (20/((BME280.Period/20)/60))-1)
                {       
                    countForecast = 0;
                    BME280_Forecast(BME280.Pression);
                }
                else
                {
                    countForecast++;
                }

                //Envoie des données à Domoticz
                SendDomoticz(/*BME280.Index, */BME280.Temperature, BME280.Humidite, BME280.Hum_Status, BME280.Pression, BME280.Pression_Forecast);   
            }   
            
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        
//        case APP_STATE_DO_MEASURE:
//        {
//            static uint16_t countForecast = 0;
//            
//            //Lecture des données du BME280
//            BME280.Temperature = BME280_readTemperature();
//            BME280.Humidite = BME280_readHumidity();
//            BME280.Pression = BME280_readPressure();
//            
//            BME280_HumidityStatus(BME280.Humidite); 
//            
//            //test si ça fait 20 minutes
//            if(countForecast >= (20/((ESP8266.Interval/20)/60))-1)
//            {       
//                countForecast = 0;
//                BME280_Forecast(BME280.Pression);
//            }
//            else
//            {
//                countForecast++;
//            }
//            
//            APP_UpdateState(APP_STATE_TRANSMIT_DATA);           
//            break;
//        }
//        
//        case APP_STATE_TRANSMIT_DATA:
//        {
//            //Envoie des données à Domoticz
//            SendDomoticz(BME280.Index, BME280.Temperature, BME280.Humidite, BME280.Hum_Status, BME280.Pression, BME280.Pression_Forecast);
//            
//            APP_UpdateState(APP_STATE_WAIT);
//            break;
//        }
//        
//        case APP_STATE_MENU:
//        {
//            //gestion du menu
//            GestMenu();
//            APP_UpdateState(APP_STATE_WAIT);
//            break;
//        }

        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

void APP_UpdateFlagInt(void)
{
    appData.flagInt++;
}

// fonction pour changer d'état 
void APP_UpdateState(APP_STATES NewState)
{
    appData.state = NewState;
}


// Fonction permettant de signaler l'état sur la led RGB
// Appel toutes les 10 ms
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
        case 100:   // à t = 1s, clignote vert si BME ok
            if (BME280.Status)
                LED_G_Off();
            break;
        case 150:
            LED_G_On();
            break;
        case 200:   // à t = 2s, clignote bleu si wifi ok
            if (ESP8266.Status)
                LED_B_Off();  
            break;
        case 250:
            LED_B_On();
            break;          
    }
        
    // test la présence du BME280
//    if(I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60)
//        BME280.Status = 0;
//    
//    // test la présence du module Wifi
//    if(ESP8266_isStarted() != OK)
//        ESP8266.Status = 0;

//    // si les deux modules sont présents
//    if(BME280.Status && ESP8266.Status)
//    {
//        LED_G_Toggle();
//    }
//    // si le BME280 est présent mais pas le module Wifi
//    else if(BME280.Status == 1 && ESP8266.Status == 0)
//    {
//        LED_B_Toggle();
//        LED_R_Toggle();
//    }
//    // si le module Wifi est présent mais pas le BME280
//    else if(BME280.Status == 0 && ESP8266.Status == 1)
//    {
//        LED_G_Toggle();
//        LED_R_Toggle();
//    }
//    //si aucun des modules n'est présent
//    else
//    {
//        LED_R_Off();
//        LED_G_On();
//        LED_B_On();
//        
//    } 
}
/*******************************************************************************
 End of File
 */
