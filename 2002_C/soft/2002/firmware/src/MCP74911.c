/*******************************************************************************
*                                                                              *
* Auteur : Joao Marques                                                        *                     
* Fichier : MCP74911.c                                                         *
*                                                                              *
* Description : Ce fichier source contient les implémentations des fonctions   *
*               pour configurer et interagir avec la RTC MCP74911.             *
*               Les fonctions incluent l'initialisation de la RTC, l'horloge   *
*               et l'alalrme2.                                                 *
*                                                                              *
*******************************************************************************/

/******************************************************************************/
/* Section: Included Files                                                    */
/******************************************************************************/

#include "app.h"
#include "MCP74911.h"
#include "RTC_I2cUtilCCS.h"


/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : decToBcd                                                *
* Entrée : Aucun                                                               *
* Sortie : Aucun                                                               *
* Description : Cette fonction gère l'affichage et la navigation dans le menu  *
*               utilisateur via USB. Elle prend en charge les réglages de      *
*               différents paramètres du système comme le Wi-Fi et Domoticz.   *
*******************************************************************************/
uint8_t decToBcd(uint8_t decimal) 
{
        
    return ((decimal / 10) << 4) | (decimal % 10);
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : bcdToDec                                                *
* Entrée : uint8_t bcd - La valeur BCD à convertir en décimal                  *
* Sortie : uint8_t - La valeur convertie en décimal                            *
* Description : Cette fonction convertit un nombre en BCD (Binary-Coded        *
*               Decimal) en son équivalent décimal.                            *
*******************************************************************************/
uint8_t bcdToDec(uint8_t bcd) 
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Write                                      *
* Entrée : void *SrcData - Pointeur vers les données à écrire                  *
*          uint32_t RTCAddr - Adresse de la RTC où écrire                      *
*          uint16_t NbBytes - Nombre de bytes à écrire                         *
* Sortie : Aucun                                                               *
* Description : Cette fonction permet d'écrire des données sur le RTC MCP79411 *
*               via le protocole I2C en gérant l'écriture par pages.           *
*******************************************************************************/
void MCP79411_RTC_Write (void *SrcData, uint32_t RTCAddr, uint16_t NbBytes)
{
    uint8_t indexNombreEcriture = 0;  // Variable pour la boucle de la page
    uint8_t indexNombrePage = 0;  // Variable pour la boucle du nombre de page
    uint8_t *i2cData = SrcData; // Pointeur qui prend l'adresse de SrcData pour le prendre comme un tableau
    uint8_t NbBytesPage = 0;    // Nombre de bytes dans la page
    
    // Boucle qui s'exécute pour chaque page
    for(indexNombrePage = 0; indexNombrePage <= (NbBytes/8); indexNombrePage++)
    {
        // Test si on est à la dernière page
        if(indexNombrePage == (NbBytes/8))
        {
            // La boucle de la page s'exécute le nombre de fois qu'il y a à écrire
            NbBytesPage = NbBytes - 8*(indexNombrePage);
        }
        else
        {
            // La boucle de la page s'exécute 8 fois
            NbBytesPage = 8;
        }
        
        // Test si on peut écrire (ACK)
        do
        {
            RTC_i2c_start();
        }while(!RTC_i2c_write(RTC_WRITE_ADDRESSE));
        // Écriture de l'adresse
        RTC_i2c_write((uint8_t)RTCAddr + (indexNombrePage * 8));
        
        // Boucle d'écriture d'une page
        for(indexNombreEcriture = 0; indexNombreEcriture < NbBytesPage; indexNombreEcriture++)
        {
           RTC_i2c_write(i2cData[indexNombreEcriture + (indexNombrePage*8)]);
        }
        RTC_i2c_stop(); // Stop après chaque page
    }
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Read                                       *
* Entrée : void *DstData - Pointeur où stocker les données lues                *
*          uint32_t RTCAddr - Adresse de la RTC à lire                         *
*          uint16_t NbBytes - Nombre de bytes à lire                           *
* Sortie : Aucun                                                               *
* Description : Cette fonction permet de lire des données à partir de la RTC   *
*               MCP79411 via le protocole I2C.                                 *
*******************************************************************************/
void MCP79411_RTC_Read(void *DstData, uint32_t RTCAddr, uint16_t NbBytes)
{
    uint8_t i = 0;
    uint8_t *i2cData = DstData;
   
    // Test si on peut écrire (ACK)
    do
    {
        RTC_i2c_start();
    }while(!RTC_i2c_write(RTC_WRITE_ADDRESSE));
    // Écriture de l'adresse
    RTC_i2c_write((uint8_t)RTCAddr);
    // Restart
    RTC_i2c_reStart();
    // Demande de lecture
    RTC_i2c_write(RTC_READ_ADDRESSE);
    // Lecture du nombre de Bytes demandés
    for(i = 0; i < NbBytes - 1; i++)
    {
        i2cData[i] = RTC_i2c_read(1);
    }
    // Finir par une lecture sans ACK
   i2cData[i] = RTC_i2c_read(0);
   RTC_i2c_stop();  // Arret
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Init                                       *
* Entrée : Aucun                                                               *
* Sortie : Aucun                                                               *
* Description : Cette fonction initialise la communication I2C pour le RTC    *
*               MCP79411 avec la configuration rapide ou lente selon le        *
*               paramètre.                                                     *
*******************************************************************************/
void MCP79411_RTC_Init (void)
{ 
    bool Fast = true;
    
    RTC_i2c_init( Fast ); 
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Init_Horloge                               *
* Entrée : Aucun                                                               *
* Sortie : Aucun                                                               *
* Description : Cette fonction initialise la RTC du MCP79411 en configurant    *
*               les registres pour démarrer l'horloge et définir le format     *
*               de l'heure.                                                    *
*******************************************************************************/
void MCP79411_RTC_Init_Horloge (void)
{                                                                         //Registre 7 pour paramètrage RTC
    uint8_t writeConfigue[8] = {0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20};  // Valeurs de configuration de la RTC

    
    MCP79411_RTC_Write(&writeConfigue[0], RTC_ADDRESSE_HORLOGES_SEC, 8);      // Démarrage RTC et écriture du format de l'heure
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Init_Alarm2                                *
* Entrée : uint32_t totalSeconds - Le nombre total de secondes pour            *
*          configurer l'alarme                                                 *
* Sortie : Aucun                                                               *
* Description : Cette fonction configure la deuxième alarme du MCP79411 en     *
*               fonction du nombre total de secondes donné.                    *
*******************************************************************************/
void MCP79411_RTC_Init_Alarm2(uint32_t totalSeconds) 
{
    uint8_t writeConfig[6] = {0x00, 0x00, 0x00, 0x88, 0x00, 0x00};  // Valeurs de configuration de la RTC
    
    // Redistribuer les secondes en heures, minutes et secondes
    uint8_t hours = totalSeconds / 3600;
    uint8_t minutes = (totalSeconds % 3600) / 60;
    uint8_t seconds = totalSeconds % 60;

    // Assurer que les heures ne dépassent pas 23 (pour format 24h)
    if (hours >= 24) {
        hours = hours % 24;
    }

    // Convertir les valeurs ajustées en BCD
    writeConfig[0] = decToBcd(seconds);
    writeConfig[1] = decToBcd(minutes);
    writeConfig[2] = decToBcd(hours);
        
    // Configuration du MCP79411 pour démarrer la RTC
    MCP79411_RTC_Write(&writeConfig[0], RTC_ADDRESSE_ALARM1_SEC, 6);  // Configurer et démarrer la RTC

}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Read_Horloge                               *
* Entrée : uint8_t *Val_Sec - Pointeur pour stocker les secondes lues          *
*          uint8_t *Val_Min - Pointeur pour stocker les minutes lues           *
*          uint8_t *Val_Heu - Pointeur pour stocker les heures lues            *
* Sortie : Aucun                                                               *
* Description : Cette fonction lit les valeurs des secondes, minutes et heures *
*               de la RTC du MCP79411.                                         *
*******************************************************************************/
void MCP79411_RTC_Read_Horloge(uint8_t *Val_Sec,uint8_t *Val_Min,uint8_t *Val_Heu)
{
    uint8_t i2cData[3];
    
    MCP79411_RTC_Read(&i2cData[0], RTC_ADDRESSE_HORLOGES_SEC, 3);
    
    *Val_Sec = i2cData[0];
    *Val_Min = i2cData[1];
    *Val_Heu = i2cData[2];
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : MCP79411_RTC_Check_and_Update_Alarm                     *
* Entrée : uint8_t time - Temps additionnel en secondes pour mettre à jour     *
*          l'alarme                                                            *
* Sortie : bool - Retourne vrai si l'alarme a été déclenchée                   *
* Description : Cette fonction vérifie si l'alarme a été déclenchée sur la     *
*               RTC MCP79411 et met à jour l'heure de l'alarme en fonction du  *
*               temps additionnel spécifié.                                    *
*******************************************************************************/
bool MCP79411_RTC_Check_and_Update_Alarm(uint8_t time) 
{
    uint8_t i2cData[4];  // Pour stocker secondes, minutes, heures et le registre d'alarme.
    bool triggered = false;

    // Lire le registre d'alarme correspondant pour vérifier le drapeau ALMxIF
    MCP79411_RTC_Read(&i2cData[3], RTC_ADDRESSE_ALARM1_WEEK, 1);

    // Vérifier si le bit ALMxIF est à 1 (l'alarme a été déclenchée)
    if (i2cData[3] & 0x08) 
    {
        triggered = true;
        LED_RGB_ROn();
        LED_RGB_BOff();
        LED_RGB_GOff();

        // Lire les secondes, minutes et heures de l'alarme
        MCP79411_RTC_Read(&i2cData[0], RTC_ADDRESSE_ALARM1_SEC, 3);
        
        // Convertir BCD en décimal
        uint8_t seconds = bcdToDec(i2cData[0]);
        uint8_t minutes = bcdToDec(i2cData[1]);
        uint8_t hours = bcdToDec(i2cData[2]);

        // Convertir heures et minutes en secondes, puis ajouter tout
        uint32_t totalSeconds = (hours * 3600) + (minutes * 60) + seconds + time;

        // Redistribuer les secondes en heures, minutes et secondes
        hours = totalSeconds / 3600;
        minutes = (totalSeconds % 3600) / 60;
        seconds = totalSeconds % 60;

        // Assurer que les heures ne dépassent pas 23 (pour format 24h)
        if (hours >= 24) {
            hours = hours % 24;
        }

        // Convertir les valeurs ajustées en BCD
        i2cData[0] = decToBcd(seconds);
        i2cData[1] = decToBcd(minutes);
        i2cData[2] = decToBcd(hours);
        i2cData[3] &= 0xF7; // Effacer le bit ALMxIF en réécrivant le registre (ALMPOL et autres bits non modifiés)

        // Écrire les nouvelles valeurs dans les registres de l'alarme
        MCP79411_RTC_Write(&i2cData[0], RTC_ADDRESSE_ALARM1_SEC, 4);
    }

    return triggered;
}



