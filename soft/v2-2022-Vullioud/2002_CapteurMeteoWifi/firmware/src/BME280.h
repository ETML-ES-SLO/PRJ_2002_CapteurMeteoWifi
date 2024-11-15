#ifndef Mc32GestSensor_H
#define Mc32GestSensor_H
/*--------------------------------------------------------*/
// Mc32GestSensor.h
/*--------------------------------------------------------*/
//	Description :	Gestion des capteurs
//	Auteur 		: 	D. Rickenbach
//	Version		:	V0.5    18.08.2020
//	Compilateur	:	XC32 V5.15 & Harmony 2_06
//
/*--------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

// *****************************************************************************
// *****************************************************************************
// Section: Defines
// *****************************************************************************
// *****************************************************************************
// Définition pour le capteur de température, humidité et pression
#define BME280_ADDRESS                  0x77

#define BME280_REGISTER_DIG_T1          0x88
#define BME280_REGISTER_DIG_T2          0x8A
#define BME280_REGISTER_DIG_T3          0x8C

#define BME280_REGISTER_DIG_P1          0x8E
#define BME280_REGISTER_DIG_P2          0x90
#define BME280_REGISTER_DIG_P3          0x92
#define BME280_REGISTER_DIG_P4          0x94
#define BME280_REGISTER_DIG_P5          0x96
#define BME280_REGISTER_DIG_P6          0x98
#define BME280_REGISTER_DIG_P7          0x9A
#define BME280_REGISTER_DIG_P8          0x9C
#define BME280_REGISTER_DIG_P9          0x9E

#define BME280_REGISTER_DIG_H1          0xA1
#define BME280_REGISTER_DIG_H2          0xE1
#define BME280_REGISTER_DIG_H3          0xE3
#define BME280_REGISTER_DIG_H4          0xE4
#define BME280_REGISTER_DIG_H5          0xE5
#define BME280_REGISTER_DIG_H6          0xE7

#define BME280_REGISTER_CHIPID          0xD0
#define BME280_REGISTER_VERSION         0xD1
#define BME280_REGISTER_SOFTRESET       0xE0

#define BME280_REGISTER_CAL26           0xE1// R calibration stored in 0xE1-0xF0

#define BME280_REGISTER_CONTROLHUMID    0xF2
#define BME280_REGISTER_STATUS          0XF3
#define BME280_REGISTER_CONTROL         0xF4
#define BME280_REGISTER_CONFIG          0xF5
#define BME280_REGISTER_PRESSUREDATA    0xF7
#define BME280_REGISTER_TEMPDATA        0xFA
#define BME280_REGISTER_HUMIDDATA       0xFD
// *****************************************************************************

typedef struct BME280 {
    uint8_t Index;
    float Temperature;   // Adress IP de l'ESP8266
    float Humidite;   // pointeur sur début du fifo
    uint8_t Hum_Status;
    float Pression;
    uint8_t Pression_Forecast;
    uint16_t Altitude;
    uint32_t Magic;
    bool Status;
} S_BME280;
S_BME280 BME280;

// prototypes des fonctions
bool BME280_Init(void);

float BME280_readTemperature(void);
float BME280_readPressure(void) ;
float BME280_readHumidity(void);

void BME280_HumidityStatus(float Hum);
void BME280_Forecast(float Press);

#endif
