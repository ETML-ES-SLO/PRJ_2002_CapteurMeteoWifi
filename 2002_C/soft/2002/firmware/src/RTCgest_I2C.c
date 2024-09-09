//--------------------------------------------------------
// RTCgestI2cSensor.C
//--------------------------------------------------------
// Gestion I2C pour la communication I2C
//	Description     :	Fonctions pour la communication en I2C
//
//	Auteur          : 	D. Rickenbach
//  Date            :   18.05.2020
//	Version         :	V1
//	Compilateur     :	XC32 V5.15 & Harmony 2_06
// Modifications    :
//
/*--------------------------------------------------------*/


#include "RTCgest_I2C.h"
#include "RTC_I2cUtilCCS.h"

// Initialisation de la communication I2C
void RTC_I2C_Init(void)
{
   bool Fast = false;
   RTC_i2c_init( Fast );
} //end I2CSensor_Init

// Ecriture d'un bloc  
void RTC_I2CSensor_Write( uint8_t Device, uint8_t Reg, uint8_t* Data, uint8_t NbBytes)
{
    uint8_t i;
    //uint8_t* pData = Data;
    
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    
    RTC_i2c_write(Device + 0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);
    
    for(i = 0; i < NbBytes; i++)
    {
        RTC_i2c_write(*Data);
        Data++;
    }
    
    RTC_i2c_stop();
} // end I2CSensor_Write

// Ecriture d'un byte
void RTC_I2CSensor_Write8(uint8_t Device, uint8_t Reg, uint8_t Data)
{
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    
    RTC_i2c_write(Device +0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);
    RTC_i2c_write(Data);
    
    RTC_i2c_stop();
} // end I2CSensor_Write8

// Lecture d'un bloc
void RTC_I2CSensor_Read(uint8_t Device, uint8_t Reg, uint8_t* Data, uint8_t NbBytes)
{
    uint8_t i;
    uint8_t* pByteData = Data;
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    RTC_i2c_write(Device +0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);    
    
    RTC_i2c_reStart();
    RTC_i2c_write(Device +1); // "+1" pour que l'adresse soit en mode lecture
    for(i = 0; i < NbBytes - 1; i++)
    {
        pByteData[i] = RTC_i2c_read(1); // lecture du registre avec acknowledge
    }
    i++;
    pByteData[i] = RTC_i2c_read(0); // lecture du registre sans acknowledge
    RTC_i2c_stop();
    
} // end I2CSensor_Read

// Lecture d'un byte
uint8_t RTC_I2CSensor_Read8(uint8_t Device, uint8_t Reg)
{
    uint8_t Data;
    
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    
    RTC_i2c_write(Device +0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);
    
    RTC_i2c_reStart();
    RTC_i2c_write(Device +1); // "+1" pour que l'adresse soit en mode lecture
    Data = RTC_i2c_read(0);   // lecture du registre sans acknowledge
    
    RTC_i2c_stop();
    
    return Data;
} // end I2CSensor_Read8

// Lecture de deux byte non-signé
uint16_t RTC_I2CSensor_Read16(uint8_t Device, uint8_t Reg)
{
    uint16_t Data;
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    RTC_i2c_write(Device +0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);
    
    RTC_i2c_reStart();
    RTC_i2c_write(Device +1); // "+1" pour que l'adresse soit en mode lecture
    Data = RTC_i2c_read(1);
    Data <<= 8;
    Data |= RTC_i2c_read(0);
    RTC_i2c_stop();
    
    return Data;
} // end I2CSensor_Read16

// Lecture de deux byte en Little endian
uint16_t RTC_I2CSensor_Read16_LE(uint8_t Device, uint8_t Reg)
{
    uint16_t Data = RTC_I2CSensor_Read16(Device, Reg);
    return (Data >> 8) | (Data << 8);
} // end I2CSensor_Read16_LE

// Lecture de deux byte en signé
int16_t RTC_I2CSensor_ReadS16(uint8_t Device, uint8_t Reg)
{
    return (int16_t)RTC_I2CSensor_Read16(Device, Reg);
} // end I2CSensor_ReadS16

// Lecture de deux byte signés en Little endian
int16_t RTC_I2CSensor_ReadS16_LE(uint8_t Device, uint8_t Reg)
{
    return (int16_t)RTC_I2CSensor_Read16_LE(Device,  Reg);
} // end I2CSensor_ReadS16_LE

// Lecture de trois byte
uint32_t RTC_I2CSensor_Read24(uint8_t Device, uint8_t Reg)
{
    uint32_t Data;
    Device = Device << 1; // décalage d'un bit pour le bit W/R
    
    RTC_i2c_start();
    RTC_i2c_write(Device +0); // "+0" pour que l'adresse soit en mode écriture
    RTC_i2c_write(Reg);
    
    RTC_i2c_reStart();
    RTC_i2c_write(Device +1); // "+1" pour que l'adresse soit en mode lecture
    
    Data = RTC_i2c_read(1);
    Data <<= 8;
    Data |= RTC_i2c_read(1);
    Data <<= 8;
    Data |= RTC_i2c_read(0);
    RTC_i2c_stop();
    
    return Data;
} // end I2CSensor_Read24

   





 



