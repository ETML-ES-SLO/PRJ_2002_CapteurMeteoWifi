//--------------------------------------------------------
// Mc32gestSensor.C
//--------------------------------------------------------
// Gestion des capteurs
//	Description     :	Fonctions pour l'utilisation des capteurs
//
//	Auteur          : 	D. Rickenbach
//  Date            :   18.08.2020
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
#include "BME280.h"
#include "app.h"
#include "Mc32gest_I2C.h"
#include "Mc32_I2cUtilCCS.h"
#include "Mc32NVMUtil.h"
#include "Mc32Delays.h"
#include <math.h>

// *****************************************************************************

// prototypes des fonctions
void BME280_ReadCoefficents(void);


//S_BME280 BME280;

// variable globale
int32_t t_fine;

//Structure des coefficients de calibration
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
    
bme280_calib_data BME280_calibration;

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     BME280_Init                                         *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      OK :    bool BME280.Status = 1                                 *
*               ERROR : bool BME280.Status = 0                                 *
* Desription :  Cette fonction permet d'initialiser le BME280 en fonction des  *
*               des registres ctrl_hum, config et ctrl_meas.                   *                                                     
* Remarques:    Voir le prototype dans BME280.h.                               *
*******************************************************************************/
bool BME280_Init(void)
{
    // variable de configuration du BME280:
    // - ctrl_humid register sets humidity data acquisition options of the device
    // - config register sets the rate filter and interface options of the device
    // - ctrl_meas register sets the pressure and temperature data acquisition 
    //   options of the device
    int8_t ctrl_hum = 0b001, config = 0b1010000, ctrl_meas = 0b00100111;      
    //   OS hum 1  / SB 1s, no filter, spi disable / OS temp. 1, OS press. 1, normal mode 
    
    NVM_ReadBlock((uint32_t *)(&Struct_save),sizeof(S_SAVE));
    if(Struct_save.BME280_save.Magic == 0x1234)
    {
        BME280.Index = Struct_save.BME280_save.Index;
    }
    else
    {
        BME280.Index = 1;
        BME280.Altitude = 450;
    }
        
    I2C_Init();
    
    // reset the BME280 using soft-reset
    // this makes sure the IIR is off, etc.
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_SOFTRESET, 0xB6);
    // wait for chip to wake up.
    while(I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_STATUS));
    
    // check if sensor, i.e. the chip ID is correct
    
    uint8_t ID_test = I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_CHIPID);
    
    if(ID_test != 0x60)
    {
        BME280.Status = 0;
        return false; // BME280 not found
    }
    
    
    // read trimming parameters, see DS 4.2.2
    BME280_ReadCoefficents();
    
    // you must make sure to also set REGISTER_CONTROL after setting the
    // CTRL_HUMID register, otherwise the values won't be applied
    // (see DS 5.4.3)
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONTROLHUMID, ctrl_hum);
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONFIG, config);
    I2CSensor_Write8(BME280_ADDRESS, BME280_REGISTER_CONTROL, ctrl_meas);
    
    BME280.Status = 1;
    return true;    
} //end Sensor_Init

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_ReadCoefficents                                 *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de lire tous les registre contenant les  *
*               coefficient de calibration pour la temp�rature, l'humidite et  *
*               la pression et enregistre les donn�es dans la tructure         *
*               BME280_calibration.                                            *                                                     
* Remarques:    Voir le prototype dans BME280.h.                               *
*******************************************************************************/
void BME280_ReadCoefficents(void)
{
    BME280_calibration.dig_T1 = I2CSensor_Read16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T1);
    BME280_calibration.dig_T2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T2);
    BME280_calibration.dig_T3 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_T3);

    BME280_calibration.dig_P1 = I2CSensor_Read16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P1);
    BME280_calibration.dig_P2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P2);
    BME280_calibration.dig_P3 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P3);
    BME280_calibration.dig_P4 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P4);
    BME280_calibration.dig_P5 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P5);
    BME280_calibration.dig_P6 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P6);
    BME280_calibration.dig_P7 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P7);
    BME280_calibration.dig_P8 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P8);
    BME280_calibration.dig_P9 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_P9);

    BME280_calibration.dig_H1 = I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H1);
    BME280_calibration.dig_H2 = I2CSensor_ReadS16_LE(BME280_ADDRESS, BME280_REGISTER_DIG_H2);
    BME280_calibration.dig_H3 = I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H3);
    BME280_calibration.dig_H4 = (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H4) << 4) | 
            (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H4+1) & 0xF);
    BME280_calibration.dig_H5 = (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H5+1) << 4) | 
            (I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H5) >> 4);
    BME280_calibration.dig_H6 = (int8_t)I2CSensor_Read8(BME280_ADDRESS, BME280_REGISTER_DIG_H6);
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_readTemperature                                 *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      float T                                                        *
* Desription :  Cette fonction permet de r�cup�rer les donn�es et gr�ce aux    *
*               calcules de compensations, donn�s dans le datasheet, d'en      *
*               sortir la temp�rature.                                         *                                                     
* Remarques:    t_fine est une variable golbale utilis�e dans plusieurs        *
*               fonctions.                                                     *
*               Voir le prototype dans BME280.h.                               *
*******************************************************************************/
float BME280_readTemperature(void)
{    
    int32_t var1, var2, adc_T;
    
    adc_T = I2CSensor_Read24(BME280_ADDRESS, BME280_REGISTER_TEMPDATA);

    if (adc_T == 0x800000) // value in case temp measurement was disabled
        return false;
    
    adc_T >>= 4;

    var1 = ((((adc_T>>3) - ((int32_t)BME280_calibration.dig_T1 << 1))) * 
            ((int32_t)BME280_calibration.dig_T2)) >> 11;
             
    var2 = (((((adc_T>>4) - ((int32_t)BME280_calibration.dig_T1)) * 
            ((adc_T >> 4) - ((int32_t)BME280_calibration.dig_T1))) >> 12) *
            ((int32_t)BME280_calibration.dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T = (t_fine * 5 + 128) >> 8;
    return T/100;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_readPressure                                    *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      float p                                                        *
* Desription :  Cette fonction permet de r�cup�rer les donn�es et gr�ce aux    *
*               calcules de compensations, donn�s dans le datasheet, d'en      *
*               sortir la pression.                                            *                                                     
* Remarques:    t_fine est une variable golbale utilis�e dans plusieurs        *
*               fonctions.                                                     *
*               Voir le prototype dans BME280.h.                               *
*******************************************************************************/
float BME280_readPressure(void) 
{
    int64_t var1, var2, p;
    int32_t adc_P;
    
    BME280_readTemperature(); // must be done first to get t_fine

    adc_P = I2CSensor_Read24(BME280_ADDRESS, BME280_REGISTER_PRESSUREDATA);
    
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return false;
    
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)BME280_calibration.dig_P6;
    var2 = var2 + ((var1*(int64_t)BME280_calibration.dig_P5)<<17);
    var2 = var2 + (((int64_t)BME280_calibration.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)BME280_calibration.dig_P3)>>8) +
           ((var1 * (int64_t)BME280_calibration.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)BME280_calibration.dig_P1)>>33;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    
    p = 1048576 - adc_P;
    p = (((p<<31) - var2)*3125) / var1;
    var1 = (((int64_t)BME280_calibration.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)BME280_calibration.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)BME280_calibration.dig_P7)<<4);
    return (float)p/25600;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_readHumidity                                    *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      float h                                                        *
* Desription :  Cette fonction permet de r�cup�rer les donn�es et gr�ce aux    *
*               calcules de compensations, donn�s dans le datasheet, d'en      *
*               sortir l'humidit�.                                             *                                                     
* Remarques:    t_fine est une variable golbale utilis�e dans plusieurs        *
*               fonctions.                                                     *
*               Voir le prototype dans BME280.h.                               *
*******************************************************************************/
float BME280_readHumidity(void) 
{
    int32_t adc_H;
    
    BME280_readTemperature(); // must be done first to get t_fine
	
    adc_H = I2CSensor_Read16(BME280_ADDRESS, BME280_REGISTER_HUMIDDATA);
    if (adc_H == 0x8000) // value in case humidity measurement was disabled
        return false;
        
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) - (((int32_t)BME280_calibration.dig_H4) << 20) -
                (((int32_t)BME280_calibration.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                (((((((v_x1_u32r * ((int32_t)BME280_calibration.dig_H6)) >> 10) *
                (((v_x1_u32r * ((int32_t)BME280_calibration.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                ((int32_t)2097152)) * ((int32_t)BME280_calibration.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)BME280_calibration.dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    
    float h = (v_x1_u32r>>12);
    return  h / 1024.0;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_HumidityStatus                                  *                                    
* Entr�e :      float Hum                                                      *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de d�terminer le status de l'humidit�    *
*               (sec, normal, confortable et humide) en fonction du taux       *
*               d'humidit� en param�tre.                                       *                                                     
* Remarques:    Voir le prototype dans BME280.h.                               *
*******************************************************************************/
void BME280_HumidityStatus(float Hum)
{
    if(Hum <= 30)
        BME280.Hum_Status = 2; //Dry
    else if(Hum > 30 && Hum <= 50)
        BME280.Hum_Status = 0; //Normal
    else if(Hum > 50 && Hum < 70)
        BME280.Hum_Status = 1; //Confortable
    else if(Hum >= 70)
        BME280.Hum_Status = 3; //Wet
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :  BME280_Forecast                                        *                                    
* Entr�e :      float Press                                                    *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de d�terminer une pr�vision m�t�rologique*
*               (Sunny, Cloudy, Partly Cloudy, Rain, no info)en fonction de la *
*               pression en param�tre, cette pr�vision se fait toutes les 20min*
*               pendant 3h.                                                    *                        
* Remarques:    Voir le prototype dans BME280.h.                               *
*******************************************************************************/
void BME280_Forecast(float Press)
{
    static float Pression[10] = {0};
    float PressionMoy = 0;
    uint8_t cPression;
    int8_t cMoyenne;
    static uint8_t OldForecast = 0;
    
    //d�calage des mesures pr�c�dentes
    for(cPression = 9; cPression >= 1; cPression--)
    {
        Pression[cPression] = Pression[cPression-1];
    }
    //ajout de la derni�re mesure
    Pression[0] = Press;
    

    //moyenne de toutes les d�riv�es entre les mesures
    for(cMoyenne = 0; cMoyenne <= 8; cMoyenne++)
    {
        PressionMoy += (Pression[cMoyenne]- Pression[cMoyenne+1])*3;
    }
    
    PressionMoy /= 9;
    
    //test si la derni�re case du tableau des mesures de pression est remplie
    //si oui, cela veut dire que �a fait au moins 3h que l'on prend des mesures
    if(Pression[9] != 0)
    {
        //test si la moyenne calcul�es est plus grande ou �gale que 2.5hPa
        if(PressionMoy >= 2.5)
        {
            BME280.Pression_Forecast = 1;   //Pr�vision : beau temps (Sunny)
        }
        //test si la moyenne calcul�es est plus grande ou �gale que 0.5hPa
        else if (PressionMoy >= 0.5)
        {
            BME280.Pression_Forecast = 1;   //Pr�vision : beau temps (Sunny)
        }
        //test si la moyenne calcul�es est plus petite ou �gale que 0.5hPa et
        //plus grande ou �gale � -0.5hPa (Stable)
        else if ( PressionMoy >= -0.5 && PressionMoy <= 0.5)
        {
            //test si la derni�re pr�vision est �gale � 0, ce qui signifie no 
            //info et on d�termine en fonction de la derni�re mesure de la 
            //pression
            if(OldForecast == 0)
            {
                //test si la derni�re mesure de la pression est plus grande que 
                //La pression normalis�e en fonction de l'altitude
                if(BME280.Pression >= (1013.25*exp(-(BME280.Altitude/8400))))
                {
                    BME280.Pression_Forecast = 1;   //Pr�vision : beau temps (Sunny)
                }
                else
                {
                    BME280.Pression_Forecast = 2;   //Pr�vision : nuageux (Cloudy)
                }
            }
            //Sinon on continue sur la derni�re pr�vision
            else
            {
                BME280.Pression_Forecast = OldForecast;
            }
        }
        //test si la moyenne calcul�es est plus petite que -0.5hPa
        else if (PressionMoy < -0.5)
        {
            BME280.Pression_Forecast = 2;   //Pr�vision : nuageux (Cloudy)
        }
        //test si la moyenne calcul�es est plus petite ou �gale que -1.5hPa
        else if(PressionMoy <= -1.5)
        {
            BME280.Pression_Forecast = 3;   //Pr�vision : plutot nuageux (Pratly Cloudy)
        }
        //test si la moyenne calcul�es est plus grande ou �gale que -2.5hPa
        else if(PressionMoy <= -2.5)
        {
            BME280.Pression_Forecast = 4;   //Pr�vision : pluie (rain)
        }
        
        OldForecast = BME280.Pression_Forecast;
    }
    //sinon pas assez d'info pour donner une pr�vision m�t�o
    else
    {
        BME280.Pression_Forecast = 0; //No Info
    }
}






 



