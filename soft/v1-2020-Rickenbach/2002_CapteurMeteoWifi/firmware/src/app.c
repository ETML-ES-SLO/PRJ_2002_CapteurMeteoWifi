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
#include "MenuGen.h"

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

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            LED_G_On();
            
            //Init de l'état du menu au démarrage
            MenuStatus = Enter_Menu_Set;
            
            //Initialisation des fifo SW
            FifoComm_Init();
            
            //Initialisation des Modules
            BME280_Init();
            ESP8266_Init();
            
            //Initialisation des Timers
            DRV_TMR1_Start();
            
            LED_G_Off();
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        
        case APP_STATE_WAIT:
        {
            //do nothing            
            break;
        }
        
        case APP_STATE_DO_MEASURE:
        {
            static uint16_t countForecast = 0;
            
            //Lecture des données du BME280
            BME280.Temperature = BME280_readTemperature();
            BME280.Humidite = BME280_readHumidity();
            BME280.Pression = BME280_readPressure();
            
            BME280_HumidityStatus(BME280.Humidite); 
            
            //test si ça fait 20 minutes
            if(countForecast >= (20/((ESP8266.Interval/20)/60))-1)
            {       
                countForecast = 0;
                BME280_Forecast(BME280.Pression);
            }
            else
            {
                countForecast++;
            }
            
            APP_UpdateState(APP_STATE_TRANSMIT_DATA);           
            break;
        }
        
        case APP_STATE_TRANSMIT_DATA:
        {
            //Envoie des données à Domoticz
            SendDomoticz(BME280.Index, BME280.Temperature, BME280.Humidite, BME280.Hum_Status, BME280.Pression, BME280.Pression_Forecast);
            
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        
        case APP_STATE_MENU:
        {
            //gestion du menu
            GestMenu();
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }

        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
// fonction pour changer d'état 
void APP_UpdateState(APP_STATES NewState)
{
    appData.state = NewState;
}

// Fonction permettant de venir tester la présence des modules et d'allumer les
//   LED en concéquence
void LED_Signal(void)
{
    // test la présence du BME280
    if(I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60)
        BME280.Status = 0;
    
    // test la présence du module Wifi
    if(ESP8266_isStarted() != OK)
        ESP8266.Status = 0;

    // si les deux modules sont présents
    if(BME280.Status && ESP8266.Status)
    {
        LED_G_Toggle();
    }
    // si le BME280 est présent mais pas le module Wifi
    else if(BME280.Status == 1 && ESP8266.Status == 0)
    {
        LED_B_Toggle();
        LED_R_Toggle();
    }
    // si le module Wifi est présent mais pas le BME280
    else if(BME280.Status == 0 && ESP8266.Status == 1)
    {
        LED_G_Toggle();
        LED_R_Toggle();
    }
    //si aucun des modules n'est présent
    else
    {
        LED_R_On();
    } 
}
/*******************************************************************************
 End of File
 */
