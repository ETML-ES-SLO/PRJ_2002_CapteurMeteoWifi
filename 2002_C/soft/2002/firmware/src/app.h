/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "ESP32.h"
#include "MCP74911.h"
#include "Mc32DriverAdc.h"

#define FREQ_SERVICE_TASKS 100  // [Hz]



// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define OutMax 3300 // tension de sotire max mesurable
#define ValAdcMax 1024 // valeur max de l'adc(10bit)


typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,
    APP_STATE_SERVICE_WAIT,
	APP_STATE_SERVICE_TASKS,

	/* TODO: Define states used by the application state machine. */

} APP_STATES;

typedef struct
{
    APP_STATES state;
    uint8_t flagInt;
    S_ADCResults ValAdc;
} APP_DATA;

//Structure de sauvegarde
typedef struct {
    uint32_t test;
    uint32_t test4;
    uint32_t test3;
    uint32_t test2;
    uint32_t test1;
    uint32_t Magic;
} S_SAVE;
S_SAVE Struct_save;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************


void APP_Initialize ( void );

void APP_Tasks( void );

void APP_UpdateState(APP_STATES newState);

void GestLed(void);

uint8_t GestAlim(void);

#endif /* _APP_H */
/*******************************************************************************
 End of File
 */

