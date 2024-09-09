#ifndef Mc32GestI2CSensor_H
#define Mc32GestI2CSensor_H
/*--------------------------------------------------------*/
// Mc32GestI2cSensor.h
/*--------------------------------------------------------*/
//	Description :	Gestion par I2C des capteur
//	Auteur 		: 	D. Rickenbach
//	Version		:	V0.5    18.05.2020
//	Compilateur	:	XC32 V5.15 & Harmony 2_06
//
/*--------------------------------------------------------*/


#include <stdint.h>

// prototypes des fonctions
void I2C_Init(void);
void I2CSensor_Write8(uint8_t Device, uint8_t Reg, uint8_t Data);
void I2CSensor_Write( uint8_t Device, uint8_t Reg, uint8_t* Data, uint8_t NbBytes);

void I2CSensor_Read(uint8_t Device, uint8_t Reg, uint8_t* Data, uint8_t NbBytes);
uint8_t I2CSensor_Read8(uint8_t Device, uint8_t Reg);
uint16_t I2CSensor_Read16(uint8_t Device, uint8_t Reg);
uint16_t I2CSensor_Read16_LE(uint8_t Device, uint8_t Reg);
int16_t I2CSensor_ReadS16(uint8_t Device, uint8_t Reg);
int16_t I2CSensor_ReadS16_LE(uint8_t Device, uint8_t Reg);
uint32_t I2CSensor_Read24(uint8_t Device, uint8_t Reg);


#endif
