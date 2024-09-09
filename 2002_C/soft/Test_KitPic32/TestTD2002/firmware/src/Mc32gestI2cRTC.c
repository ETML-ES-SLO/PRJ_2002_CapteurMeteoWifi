//--------------------------------------------------------
// Mc32gestI2cEEprom.C
//--------------------------------------------------------
// Gestion I2C de la SEEPROM du MCP79411 (Solution exercice)
//	Description :	Fonctions pour EEPROM MCP79411
//
//	Auteur 		: 	C. HUBER
//      Date            :       26.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/


#include "app.h"
#include "Mc32gestI2cRTC.h"
#include "Mc32_I2cUtilCCS.h"



uint8_t decToBcd(uint8_t decimal) 
{
    return ((decimal / 10) << 4) | (decimal % 10);
}
uint8_t bcdToDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
/*
 * Fonction permettant de faire une écriture sur le MCP79411 en I2C
*/
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
            i2c_start();
        }while(!i2c_write(RTC_WRITE_ADDRESSE));
        // Écriture de l'adresse
        i2c_write((uint8_t)RTCAddr + (indexNombrePage * 8));
        
        // Boucle d'écriture d'une page
        for(indexNombreEcriture = 0; indexNombreEcriture < NbBytesPage; indexNombreEcriture++)
        {
           i2c_write(i2cData[indexNombreEcriture + (indexNombrePage*8)]);
        }
        i2c_stop(); // Stop après chaque page
    }
}

/*
 * Fonction permettant de faire une lecture sur le MCP79411 en I2C
*/
void MCP79411_RTC_Read(void *DstData, uint32_t RTCAddr, uint16_t NbBytes)
{
    uint8_t i = 0;
    uint8_t *i2cData = DstData;
   
    // Test si on peut écrire (ACK)
    do
    {
        i2c_start();
    }while(!i2c_write(RTC_WRITE_ADDRESSE));
    // Écriture de l'adresse
    i2c_write((uint8_t)RTCAddr);
    // Restart
    i2c_reStart();
    // Demande de lecture
    i2c_write(RTC_READ_ADDRESSE);
    // Lecture du nombre de Bytes demandés
    for(i = 0; i < NbBytes - 1; i++)
    {
        i2cData[i] = i2c_read(1);
    }
    // Finir par une lecture sans ACK
   i2cData[i] = i2c_read(0);
   i2c_stop();  // Arret
}  
void MCP79411_RTC_Init (void)
{ 
    bool Fast = true;
    
    i2c_init( Fast ); 
}
/*
 * Fonction permettant l'initialisation du MCP79411
*/
void MCP79411_RTC_Init_Horloge (void)
{                                                                         //Registre 7 pour paramètrage RTC
    uint8_t writeConfigue[8] = {0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20};  // Valeurs de configuration de la RTC

    
    MCP79411_RTC_Write(&writeConfigue[0], RTC_ADDRESSE_HORLOGES_SEC, 8);      // Démarrage RTC et écriture du format de l'heure
}


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
void MCP79411_RTC_Read_Horloge(uint8_t *Val_Sec,uint8_t *Val_Min,uint8_t *Val_Heu)
{
    uint8_t i2cData[3];
    
    MCP79411_RTC_Read(&i2cData[0], RTC_ADDRESSE_HORLOGES_SEC, 3);
    
    *Val_Sec = i2cData[0];
    *Val_Min = i2cData[1];
    *Val_Heu = i2cData[2];
}

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
        LED5_W = !LED5_R; // Inverser l'état d'une LED pour indication

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



