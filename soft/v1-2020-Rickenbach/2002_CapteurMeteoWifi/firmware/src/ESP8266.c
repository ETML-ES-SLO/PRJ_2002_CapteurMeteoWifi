//--------------------------------------------------------
// ESP8266.C
//--------------------------------------------------------
// Gestion des capteurs
//	Description     :	Fonctions pour l'utilisation de l'ESP8266
//
//	Auteur          : 	D. Rickenbach
//  Date            :   21.08.2020
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
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "peripheral/ports/plib_ports.h"
#include "driver/tmr/drv_tmr_static.h"
#include "Mc32gest_RS232.h"
#include "Mc32NVMUtil.h"
#include "Mc32Delays.h"
#include "ESP8266.h"
#include "BME280.h"
#include "app.h"


/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_Init                                        *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      OK :    bool BME280.Status = 1                                 *
*               ERROR : bool BME280.Status = 0                                 *
* Desription :  Cette fonction permet d'initialiser de l'ESP8266 en le         *
*               connectant au Wifi par defaut et mettons à jour la Structure   *
*               ESP8266                                                        *                                                     
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
bool ESP8266_Init(void)
{     
    //lecture dans la flash interne au microcontroleur la structure.
    NVM_ReadBlock((uint32_t *)(&Struct_save),sizeof(S_SAVE));
    
    //test après la lecture dans la flash, si on retrouve la valeur du Magic
    if(Struct_save.ESP8266_save.Magic == 0x1234)
    {
        //si oui, on récupère la valeur d'interval qui était enregistrée
        ESP8266.Interval = Struct_save.ESP8266_save.Interval;
    }
    else
    {
        //sinon, on met une valeur par défaut (10min)
        ESP8266.Interval = 12000;
    }
    
    //test après la lecture dans la flash, si on retrouve la valeur du Magic
    if(Struct_save.Wifi_save.Magic == 0x1234)
    {
        //si oui, on récupère les valeurs qui étaient enregistrées
        memmove(Wifi.SSID, Struct_save.Wifi_save.SSID, sizeof(Struct_save.Wifi_save.SSID));
        memmove(Wifi.PWD, Struct_save.Wifi_save.PWD, sizeof(Struct_save.Wifi_save.PWD));
        memmove(Wifi.IP, Struct_save.Wifi_save.IP, sizeof(Struct_save.Wifi_save.IP));
        memmove(Wifi.Port, Struct_save.Wifi_save.Port, sizeof(Struct_save.Wifi_save.Port));
    }
    else
    {
        //sinon, on met une valeur par défaut
        memmove(Wifi.SSID, "Wifi-Projet-2002", strlen("Wifi-Projet-2002"));
        memmove(Wifi.PWD, "2002-CapteurMeteo", strlen("2002-CapteurMeteo"));
        memmove(Wifi.IP, "192.168.0.101", strlen("192.168.0.101"));
        memmove(Wifi.Port, "8080", strlen("8080"));
    }
    
    //HW Reset 
    WIFI_RST_Off();
    WIFI_RST_On();
    
    //délais d'attente de 5s jusqu'à ce que le module soit pret
    delay_msCt(5000);
    
    //test la présence du module Wifi
    if(ESP8266_isStarted() == OK)
    {
        //si oui, test de le connecter au Wifi
        if(ESP8266_ConnectWifi(Wifi.SSID,Wifi.PWD) == OK)
        {
            //si oui, le met en mode station et on enregistre son adresse IP et MAC
            ESP8266_Mode(ModeStation);
            ESP8266_IP(ESP8266.IP, ESP8266.MAC);
            
            //test si l'adresse IP est différente de 0
            if(ESP8266.IP[0] != 0)
            {
                //si oui, c'est que le module est fonctionnel
                ESP8266.Status = 1;
                return true;
            } 
            //sinon, le module n'est pas fonctionnel
            else
            {
                ESP8266.Status = 0;
                return false;
            }
        }
        else
        {
            ESP8266_Mode(ModeStation);
            ESP8266_IP(ESP8266.IP, ESP8266.MAC);

            if(ESP8266.IP[0] != 0)
            {
                ESP8266.Status = 1;
                return true;
            }
            else
            {
                ESP8266.Status = 0;
                return false;
            }   
        }
    }
    ESP8266.Status = 0;
    return false;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_isStarted                                   *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de vérifier la présence du module Wifi   *                                             
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_isStarted(void)
{
    
    int8_t CommandisStarted[] = "AT\r\n";   //création de la trame a envoyer
    int8_t StrRes[10] = {0};
    uint8_t Status = 0;
    
    //envoie de la trame 
    SendWifiMessage(CommandisStarted, strlen((char*)CommandisStarted));
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 2);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_Reset                                       *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de faire un reset SW du module Wifi      *                                           
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_Reset(void)
{
    int8_t CommandReset[] = "AT+RST\r\n";   //création de la trame a envoyer
    int8_t StrRes[14] = {0};
    uint8_t Status;
    
    //envoie de la trame 
    SendWifiMessage(CommandReset, strlen((char*)CommandReset));
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 100);

    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_Mode                                        *                                    
* Entrée :      uint8_t mode                                                   *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de mettre le module dans un certain mode *                                             
*               (Station, Acces Point ou Station et Acces Point)               *
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_Mode(uint8_t mode)
{
    int8_t CommandMode[] = "AT+CWMODE= \r\n";   //création de la trame a envoyer
    CommandMode[10] = mode;
    int8_t StrRes[18] = {0};
    uint8_t Status;
    
    //envoie de la trame
    SendWifiMessage(CommandMode, strlen((char*)CommandMode));
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 2);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_ConnectWifi                                 *                                    
* Entrée :      char *SSID, char *PWD                                          *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de connecter le module Wifi à un reseau  *
*               Wifi                                                           *
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_ConnectWifi(char *SSID, char *PWD)
{
    //création de la trame a envoyer
    int8_t CommandConnectWifi1[] = "AT+CWJAP=\"";
    int8_t CommandConnectWifi2[] = "\",\"";
    int8_t CommandConnectWifi3[] = "\"\r\n\n";
    int8_t StrRes[200] = {0};
    uint8_t Status;
    
    //envoie des trames
    SendWifiMessage(CommandConnectWifi1, strlen((char*)CommandConnectWifi1));
    SendWifiMessage((int8_t*)SSID, strlen((char*)SSID));
    SendWifiMessage(CommandConnectWifi2, strlen((char*)CommandConnectWifi2));
    SendWifiMessage((int8_t*)PWD, strlen((char*)PWD));
    SendWifiMessage(CommandConnectWifi3, strlen((char*)CommandConnectWifi3));
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 11000);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     getIpAndMac                                         *                                    
* Entrée :      char* inputStr, char* IpStr, char* MacStr                      *                                                                      
* Sortie :      true                                                           *
*               false                                                          *
* Desription :  Cette fonction permet de récuprer l'adresse IP et l'adresse MAC*
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t getIpAndMac(char* inputStr, char* IpStr, char* MacStr)
{
    char* quotePos[4];
        
    if (strstr(inputStr, "STAIP") != NULL && strstr(inputStr, "STAMAC") != NULL)
    {
        //cherche les guillemets
        quotePos[0] = strchr(inputStr, '"');
        quotePos[1] = strchr(quotePos[0]+1, '"');
        quotePos[2] = strchr(quotePos[1]+1, '"');
        quotePos[3] = strchr(quotePos[2]+1, '"');
        
        *(quotePos[1]) = 0;
        *(quotePos[3]) = 0;
    
        strcpy(IpStr ,quotePos[0]+1);
        strcpy(MacStr ,quotePos[2]+1); 
        return true;
    } else
    {
        return false;  //pas trouvé les chaines
    }
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_IP                                          *                                    
* Entrée :      char* StrIP, char* StrMAC                                      *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de demander au module Wifi de donner son *
*               adresse IP ainsi que MAC                                       * 
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_IP(char* StrIP, char* StrMAC)
{
    int8_t CommandIP[] = "AT+CIFSR\r\n";
    int8_t StrRes[100] = {0};
    uint8_t Status;
    
    SendWifiMessage(CommandIP, strlen((char*)CommandIP));
    delay_msCt(100);
    Status = GetWifiMessage(StrRes, "OK", 5);
    
    getIpAndMac((char*)StrRes, StrIP, StrMAC);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_ConnectToTCPServer                          *                                    
* Entrée :      char *StrIP, char *StrPort                                     *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de connecter le module Wifi à un serveur *
*               grâce à son adresse IP et son Port                             *  
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_ConnectToTCPServer(char *StrIP, char *StrPort)
{
    int8_t CommandConnectTCPServer1[] = "AT+CIPSTART=\"TCP\",\"";    
    int8_t CommandConnectTCPServer2[] = "\",";
    int8_t CommandConnectTCPServer3[] = "\r\n";
    int8_t StrRes[60] = {0};
    uint8_t Status;
    
    SendWifiMessage(CommandConnectTCPServer1, strlen((char*)CommandConnectTCPServer1));
    SendWifiMessage((int8_t*)StrIP, strlen((char*)StrIP));
    SendWifiMessage(CommandConnectTCPServer2, strlen((char*)CommandConnectTCPServer2));
    SendWifiMessage((int8_t*)StrPort, strlen((char*)StrPort));
    SendWifiMessage(CommandConnectTCPServer3, strlen((char*)CommandConnectTCPServer3));
    Status = GetWifiMessage(StrRes, "OK", 1000);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ESP8266_DisconnectTCPServer                         *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de déconnecter le module Wifi au serveur *
*               auquel il est connecté                                         *  
* Remarques:    Un Timeout est prédéfinit par le module Wifi dans le cas ou    *
*               l'on oublirait de se déconnecter                               *
*               Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ESP8266_DisconnectTCPServer(void)
{
    int8_t CommandDisconnectTCPServer[] = "AT+CIPCLOSE\r\n";
    int8_t StrRes[30] = {0};
    uint8_t Status;
    
    SendWifiMessage(CommandDisconnectTCPServer, strlen((char*)CommandDisconnectTCPServer));
    
    Status = GetWifiMessage(StrRes, "OK", 10);
    
    return Status;
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     SendDomoticz                                        *                                    
* Entrée :      uint8_t Index, float Temp, float Hum, uint8_t Hum_stat,        *
*               float Press, uint8_t Press_fore                                *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet d'envoyer une requêtte HTTP                                         *  
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t SendDomoticz(uint8_t Index, float Temp, float Hum, uint8_t Hum_stat, float Press, uint8_t Press_fore)
{  
    int8_t cmdString[20];
    uint8_t cmdStringLen;
    
    int8_t httpString[120];
    uint8_t httpStringLen;
    
    int8_t StrRes[500] = {0};
    uint8_t Status;
    
    sprintf((char*)httpString, "GET /json.htm?type=command&param=udevice&idx=%d&nvalue=0&svalue=%.1f;%.1f;%d;%.1f;%d HTTP/1.0\r\n\r\n", 
            Index, (double)Temp, (double)Hum , Hum_stat, (double)Press, Press_fore);        
    httpStringLen = strlen((char*)httpString);
    
    sprintf ((char*)cmdString, "AT+CIPSEND=%2d\r\n", httpStringLen);
    cmdStringLen = strlen((char*)cmdString);    
    
    if(ESP8266_ConnectToTCPServer(Wifi.IP,Wifi.Port) == OK)
    {
        SendWifiMessage(cmdString, cmdStringLen);
        delay_msCt(10);
        SendWifiMessage(httpString, httpStringLen);
    }
    
    Status = GetWifiMessage(StrRes, "CLOSE", 20000);
    return Status;
}
// *****************************************************************************
