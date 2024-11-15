#ifndef Mc32GestSensor_H
#define Mc32GestSensor_H
/*--------------------------------------------------------*/
// Mc32GestSensor.h
/*--------------------------------------------------------*/
//	Description :	Gestion des capteurs
//	Auteur 		: 	D. Rickenbach
//	Version		:	V0.5    18.05.2020
//	Compilateur	:	XC32 V5.15 & Harmony 2_06
//
/*--------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

// prototypes des fonctions
bool Sensor_Init(void);

float BME280_readTemperature(void);
float BME280_readPressure(void) ;
float BME280_readHumidity(void);

#endif
