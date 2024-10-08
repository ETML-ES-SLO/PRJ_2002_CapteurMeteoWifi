/*******************************************************************************
*                                                                              *
* Auteur : Joao Marques                                                        *
* Fichier : ENS160.c                                                           *
*                                                                              *
* Description : Ce fichier contient les impl�mentations des fonctions          *
*               permettant d'interagir avec le capteur ENS160, notamment       *
*               l'initialisation du capteur, la lecture des donn�es de qualit� *
*               de l'air (AQI), de la concentration de TVOC et de CO2.         *
*                                                                              *
*******************************************************************************/

/******************************************************************************/
// Section: Included Files                                                    */
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "Mc32gest_I2C.h"
#include "Mc32Delays.h"
#include "ENS160.h"

/******************************************************************************/
// Section: Functions                                                         */
/******************************************************************************/
/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction : Swap_MSB_LSB                                            *                                    
* Entr�e : uint16_t Data : La donn�e avec les octets MSB et LSB � inverser     *                                                                      
* Sortie : uint16_t : La donn�e avec les octets MSB et LSB invers�s            *
* Description : Cette fonction inverse les octets MSB et LSB d'une valeur      *
*               uint16_t, puis retourne la valeur invers�e.                    *
*******************************************************************************/
uint16_t Swap_MSB_LSB(uint16_t Data)
{
    uint8_t msb = 0;
    uint8_t lsb = 0;
    uint16_t Result = 0;
    
    msb = (Data >> 8) & 0xFF;  // Extraire l'octet MSB
    lsb = Data & 0xFF;         // Extraire l'octet LSB
    
    // Inverser les octets et combiner les r�sultats
    Result = (uint16_t)lsb << 8 | msb;   
    
    return Result;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction : ENS160_Init                                             *                                    
* Entr�e : Aucun                                                               *                                                                      
* Sortie : Aucun                                                               *
* Description : Cette fonction configure le capteur ENS160 en d�finissant      *
*               le mode de fonctionnement et les param�tres de configuration.  *
*               Un d�lai de 100 ms est ajout� pour permettre au capteur de se  *
*               stabiliser apr�s la configuration.                             *
*******************************************************************************/
bool ENS160_Init(void)
{
    uint16_t ID_test = I2CSensor_Read16(ENS160_ADDRESS, ENS160_REGISTER_CHIPID);
    
    if (ID_test != 0x6001)
        return ENS160.Status = 0;

    
    // Configurer le mode de fonctionnement du capteur ENS160
    I2CSensor_Write8(ENS160_ADDRESS, ENS160_ADDRESS_OPMODE, 
                    ENS160_OPMODE_STANDARD);
    
    // Appliquer les param�tres de configuration au capteur ENS160
    I2CSensor_Write8(ENS160_ADDRESS, ENS160_ADDRESS_CONFIG, ENS160_CONFIG_SET);
    
    // Attendre 100 ms pour stabiliser le capteur apr�s la configuration
    delay_msCt(100);
    
    ENS160.Status = 1;
    return ENS160.Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction : ENS160_Read_Data_Aqi                                    *                                    
* Entr�e : Aucun                                                               *                                                                      
* Sortie : uint8_t : L'indice de qualit� de l'air (AQI)                        *
* Description : Cette fonction lit l'indice de qualit� de l'air (AQI) depuis   *
*               le capteur ENS160 en utilisant l'I2C.                          *
*******************************************************************************/
uint8_t ENS160_Read_Data_Aqi()
{
    uint8_t Indice_Qualite_Air = 0 ;
    
    // Lecture de l'indice de qualit� d'air
    Indice_Qualite_Air = I2CSensor_Read8(ENS160_ADDRESS, ENS160_ADDRESS_DATA_AQI
                                        );
    // retourne la valeur de la qualit� d'air
    return Indice_Qualite_Air;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction : ENS160_Read_Data_Tvoc                                   *                                    
* Entr�e : Aucun                                                               *                                                                      
* Sortie : uint16_t : La concentration de TVOC en ppb                          *
* Description : Cette fonction lit la concentration de TVOC (Total Volatile    *
*               Organic Compounds) depuis le capteur ENS160 en utilisant l'I2C.*
*******************************************************************************/
uint16_t ENS160_Read_Data_Tvoc()
{
    uint16_t Data = 0;
    uint16_t Concentration_Ppb = 0;
    
    // Lecture des Data 
    Data = I2CSensor_Read16(ENS160_ADDRESS, ENS160_ADDRESS_DATA_TVOC);
    
    // recuperer la valeur de TVOC
    Concentration_Ppb = Swap_MSB_LSB(Data);
    
    // retourne la valeur de TVOC
    return Concentration_Ppb;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction : ENS160_Read_Data_Eco2                                   *                                    
* Entr�e : Aucun                                                               *                                                                      
* Sortie : uint16_t : La concentration de CO2 en ppm                           *
* Description : Cette fonction lit la concentration de CO2 (eCO2) depuis le    *
*               capteur ENS160 en utilisant l'I2C.                             *
*******************************************************************************/
uint16_t ENS160_Read_Data_Eco2()
{
    uint16_t Data = 0;
    uint16_t Concentration_Co2 = 0;
    
    // Lecture des Data
    Data = I2CSensor_Read16(ENS160_ADDRESS, ENS160_ADDRESS_DATA_ECO2);
    
    // Recuperation de la valeur de CO2
    Concentration_Co2 = Swap_MSB_LSB(Data);
    
    // Retourne la valeur de CO2
    return Concentration_Co2;
}

/* *****************************************************************************
 End of File
 */
