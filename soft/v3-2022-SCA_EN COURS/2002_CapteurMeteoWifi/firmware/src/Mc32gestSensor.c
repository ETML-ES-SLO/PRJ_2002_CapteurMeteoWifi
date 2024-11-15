//--------------------------------------------------------
// Mc32gestSensor.C
//--------------------------------------------------------
// Gestion des capteurs
//	Description     :	Fonctions pour l'utilisation des capteurs
//
//	Auteur          : 	D. Rickenbach
//  Date            :   18.05.2020
//	Version         :	V0.5
//	Compilateur     :	XC32 V5.15 & Harmony 2_06
// Modifications    :
//
/*--------------------------------------------------------*/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "Mc32gestSensor.h"
#include "Mc32gestI2cSensor.h"
#include "Mc32_I2cUtilCCS.h"
#include "Mc32Delays.h"

// *****************************************************************************

// prototypes des fonctions
void BME280_ReadCoefficents(void);

// variable globale
int t_fine;
// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
typedef struct
    {
        uint16_t dig_T1;
        int16_t  dig_T2;
        int16_t  dig_T3;

        uint16_t dig_P1;
        int16_t  dig_P2;
        int16_t  dig_P3;
        int16_t  dig_P4;
        int16_t  dig_P5;
        int16_t  dig_P6;
        int16_t  dig_P7;
        int16_t  dig_P8;
        int16_t  dig_P9;

        uint8_t  dig_H1;
        int16_t  dig_H2;
        uint8_t  dig_H3;
        int16_t  dig_H4;
        int16_t  dig_H5;
        int8_t   dig_H6;
    } bme280_calib_data;
    
    bme280_calib_data BME280_coefficient;
// *****************************************************************************
 
    
// Initialisation des capteurs
bool Sensor_Init(void)
{
    // variable de configuration du BME280
    // ctrl_humid register sets humidity data acquisition options of the device
    // config register sets the rate, filter and interface options of the device
    // ctrl_meas register sets the pressure and temperature data acquisition options of the device
    int8_t ctrl_humid = 0b001, config = 0b1000000, ctrl_meas = 0b00100101;      
    
    I2C_Init();
    
    // reset the BME280 using soft-reset
    // this makes sure the IIR is off, etc.
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_SOFTRESET, 0xB6);
    // wait for chip to wake up.
    while(I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_STATUS));
    
    // check if sensor, i.e. the chip ID is correct
    if(I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_CHIPID) != 0x60)
        return false; // BME280 not found
    
    // read trimming parameters, see DS 4.2.2
    BME280_ReadCoefficents();
    
    // you must make sure to also set REGISTER_CONTROL after setting the
    // CONTROLHUMID register, otherwise the values won't be applied (see DS 5.4.3)
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONTROLHUMID, ctrl_humid);
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONFIG, config);
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONTROL, ctrl_meas);
    
    // reset the CCS811 using soft-reset
    // four bytes must be written to this register in a single I²C sequence: 0x11, 0xE5, 0x72, 0x8A. 
    uint8_t aReset[4] = {0x11, 0xE5, 0x72, 0x8A};
    I2CSensor_Write(CCS811_ADDRESS, CCS811_REGISTER_SOFTRESET, aReset, 4);
    // wait for chip to wake up.
    delay_msCt(2);
    // check if sensor, i.e. the chip ID is correct
    if (I2CSensor_Read8(CCS811_ADDRESS, CCS811_REGISTER_CHIPID) != 0x81)
        return false; // CCS811 not found 
    
    return true;    
} //end Sensor_Init

// Lecture des coefficents interne au BME280
void BME280_ReadCoefficents(void)
{
    BME280_coefficient.dig_T1 = I2CSensor_Read16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T1);
    BME280_coefficient.dig_T2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T2);
    BME280_coefficient.dig_T3 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T3);

    BME280_coefficient.dig_P1 = I2CSensor_Read16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P1);
    BME280_coefficient.dig_P2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P2);
    BME280_coefficient.dig_P3 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P3);
    BME280_coefficient.dig_P4 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P4);
    BME280_coefficient.dig_P5 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P5);
    BME280_coefficient.dig_P6 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P6);
    BME280_coefficient.dig_P7 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P7);
    BME280_coefficient.dig_P8 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P8);
    BME280_coefficient.dig_P9 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P9);

    BME280_coefficient.dig_H1 = I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H1);
    BME280_coefficient.dig_H2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_H2);
    BME280_coefficient.dig_H3 = I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H3);
    BME280_coefficient.dig_H4 = (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H4) << 4) | 
            (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H4+1) & 0xF);
    BME280_coefficient.dig_H5 = (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H5+1) << 4) | 
            (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H5) >> 4);
    BME280_coefficient.dig_H6 = (int8_t)I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H6);
}

// Lecture de la température
float BME280_readTemperature(void)
{
    int32_t var1, var2;
    int32_t adc_T = I2CSensor_Read24(BME280_ADDRESS, BME280_REGISTER_TEMPDATA);

    if (adc_T == 0x800000) // value in case temp measurement was disabled
        return false;
    adc_T >>= 4;

    var1 = ((((adc_T>>3) - ((int32_t)BME280_coefficient.dig_T1 <<1))) *
            ((int32_t)BME280_coefficient.dig_T2)) >> 11;
             
    var2 = (((((adc_T>>4) - ((int32_t)BME280_coefficient.dig_T1)) *
              ((adc_T>>4) - ((int32_t)BME280_coefficient.dig_T1))) >> 12) *
            ((int32_t)BME280_coefficient.dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T = (t_fine * 5 + 128) >> 8;
    return T/100;
}

// Lecture de la pression
float BME280_readPressure(void) 
{
    int64_t var1, var2, p;

    BME280_readTemperature(); // must be done first to get t_fine

    int32_t adc_P = I2CSensor_Read24(BME280_ADDRESS, BME280_REGISTER_PRESSUREDATA);
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return false;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)BME280_coefficient.dig_P6;
    var2 = var2 + ((var1*(int64_t)BME280_coefficient.dig_P5)<<17);
    var2 = var2 + (((int64_t)BME280_coefficient.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)BME280_coefficient.dig_P3)>>8) +
           ((var1 * (int64_t)BME280_coefficient.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)BME280_coefficient.dig_P1)>>33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)BME280_coefficient.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)BME280_coefficient.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)BME280_coefficient.dig_P7)<<4);
    return (float)p/256;
}

// Lecture de l'humidité
float BME280_readHumidity(void) 
{
    BME280_readTemperature(); // must be done first to get t_fine
	
    int32_t adc_H = I2CSensor_Read16(BME280_ADDRESS, BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return false;
        
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)BME280_coefficient.dig_H4) << 20) -
                (((int32_t)BME280_coefficient.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                (((((((v_x1_u32r * ((int32_t)BME280_coefficient.dig_H6)) >> 10) *
                (((v_x1_u32r * ((int32_t)BME280_coefficient.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                ((int32_t)2097152)) * ((int32_t)BME280_coefficient.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)BME280_coefficient.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float h = (v_x1_u32r>>12);
    return  h / 1024.0;
}






 



