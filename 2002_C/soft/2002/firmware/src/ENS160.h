/*******************************************************************************
*                                                                              *
* Auteur : Joao Marques                                                        *                     
* Fichier : ENS160.h                                                           *
*                                                                              *
* Description : Déclarations des fonctions et données pour le capteur ENS160   *
*                                                                              *
*******************************************************************************/

#ifndef ENS160_H    
#define ENS160_H

/******************************************************************************/
/* Section : Fichiers Inclus                                                  */
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/* Section : Définitions                                                      */
/******************************************************************************/

#define ENS160_ADDRESS           0x53 // Adresse I2C du capteur ENS160

#define ENS160_REGISTER_CHIPID   0x00 // Adresse Device identity

// Registres de configuration
#define ENS160_ADDRESS_OPMODE    0x10 // Registre pour le mode de fonctionnement
#define ENS160_OPMODE_DEEP_SLEEP 0x00 // Mode sommeil profond
#define ENS160_OPMODE_IDLE       0x01 // Mode veille
#define ENS160_OPMODE_STANDARD   0x02 // Mode standard
#define ENS160_OPMODE_RESET      0xF0 // Mode réinitialisation

#define ENS160_ADDRESS_CONFIG    0x11 // Registre pour la configuration
#define ENS160_CONFIG_SET        0x23 // Configuration à appliquer

// Registres de données
#define ENS160_ADDRESS_DATA_AQI  0x21 // Registre pour l'Indice de Qualité d'Air
#define ENS160_ADDRESS_DATA_TVOC 0x22 // Registre pour la concentration de TVOC 
#define ENS160_ADDRESS_DATA_ECO2 0x24 // Registre pour la concentration de CO2 

//*****************************************************************************/
// Section : Données                                                          */
//*****************************************************************************/

typedef struct ENS160 {
    uint8_t Indice_Qualite_Air; // Indice de qualité de l'air
    uint16_t Concentration_Ppb; // Concentration de TVOC en parties par milliard
    uint16_t Concentration_Co2; // Concentration de CO2 équivalent en ppm
    uint32_t EcartQualite;  // ecart pour limite max et min qualite d'aire
    bool Status;                // État du capteur
    uint32_t Magic;             // Valeur magique pour la validation
} S_ENS160;
// Déclaration d'une variable globale de type S_ENS160
S_ENS160 ENS160;  

//*****************************************************************************/
// Section : Fonctions                                                        */
//*****************************************************************************/

// Inverse les octets MSB et LSB d'une donnée 16 bits.
uint16_t Swap_MSB_LSB(uint16_t Data);

// Initialise le capteur ENS160 en configurant les registres nécessaires.
bool ENS160_Init(void);

// Lit l'indice de qualité de l'air (AQI) à partir du capteur ENS160.
uint8_t ENS160_Read_Data_Aqi(void);

// Lit la concentration de TVOC (Total Volatile Organic Compounds) en ppb.
uint16_t ENS160_Read_Data_Tvoc(void);

// Lit la concentration de CO2 équivalent en ppm.
uint16_t ENS160_Read_Data_Eco2(void);

#endif /* ENS160_H */

/* *****************************************************************************
 Fin du fichier
 */