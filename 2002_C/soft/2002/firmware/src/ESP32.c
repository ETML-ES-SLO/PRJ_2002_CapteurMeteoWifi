/*******************************************************************************
*                                                                              *
* Auteur : Joao Marques                                                        *                     
* Fichier : ESP32.c                                                            *
*                                                                              *
* Description : Ce fichier source contient les implémentations des fonctions   *
*               pour configurer et interagir avec le module ESP32 via des      *
*               commandes AT. Les fonctions incluent l'initialisation du       *
*               module, la connexion au réseau Wi-Fi, la publication de        *
*               messages MQTT, et la gestion des paramètres réseau.            *
*                                                                              *
*******************************************************************************/

/******************************************************************************/
/* Section: Included Files                                                    */
/******************************************************************************/

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
#include "ESP32.h"


/******************************************************************************/
// Section: Functions                                                         */
/******************************************************************************/

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     ESP32_Init                                          *                                    
* Entrée :      Aucun                                                          *
* Sortie :      bool                                                           *
*               true    si l'initialisation de l'ESP32 a réussi et que la      *
*                       connexion au réseau Wi-Fi est établie correctement,    *
*                       avec une adresse IP valide.                            *
*               false   si l'initialisation ou la connexion échoue.            *
* Description :  Cette fonction initialise le module ESP32 en configurant les  *
*               paramètres réseau, en activant le module, et en effectuant les *
*               vérifications nécessaires. Elle tente de se connecter au réseau*
*               Wi-Fi spécifié et change le mode de l'ESP32 en mode Station.   *
*               La fonction récupère également l'adresse IP et l'adresse MAC de*
*               l'ESP32, et met à jour le statut en fonction de la validité de *
*               l'adresse IP obtenue.                                          *
*******************************************************************************/
bool ESP32_Init(void)
{
    //configuration des donnée
    //recuperation des valeurs dans la memoir
    
    //si pas dans la memoire 
    memmove(ESP_Wifi.Ssid, "Wifi-Projet-2002", strlen("Wifi-Projet-2002"));
    memmove(ESP_Wifi.Password, "2002-CapteurMeteo", strlen("2002-CapteurMeteo"));
    
    memmove(MQTT.Host, "192.168.1.100", strlen("192.168.1.100"));
    memmove(MQTT.Port, "1883", strlen("1883"));
    
    memmove(Domoticz.Ip, "192.168.1.102", strlen("192.168.1.102"));
    memmove(Domoticz.Port, "1000", strlen("1000"));
    
    // activer la pin enable
    ESP_ENOn();
    
    //délais d'attente de 5s jusqu'à ce que le module soit pret
    delay_msCt(5000);
    
    //test la présence du module Wifi
    if(ESP32_isStarted() == OK)
    {
        //délais d'attente de 100ms 
        delay_msCt(100);
    }
        if(ESP32_ConnectWifi(ESP_Wifi.Ssid,ESP_Wifi.Password) == OK)
        {
            //délais d'attente de 100ms
            delay_msCt(100);
            
            // change le mode de l'ESP32
            ESP32_ChangeMode(ModeStation);
            //délais d'attente de 100ms
            delay_msCt(100);
            
            // Recuperation de l'adresse IP et MAC
            ESP32_GetIpAndMac(ESP32.IP, ESP32.MAC);
            //délais d'attente de 100ms
            delay_msCt(100);
            
            //test si l'adresse IP est différente de 0
            ESP32.Status = (ESP32.IP[0] != 0)? true:false;
        }

    return ESP32.Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     SendATCommand                                       *                                    
* Entrée :      const char* command                                            *                                                                      
* Sortie :      Aucun                                                          *
* Desription :  Cette fonction envoie une commande AT au module Wi-Fi via la   *
*               fonction SendWifiMessage, et attend un délai pour s'assurer    *
*               que la requête est correctement envoyée et que la réponse est  *
*               reçue.                                                         *
*******************************************************************************/
// Fonction pour envoyer une commande AT
void SendATCommand(const char* command) 
{
    //envoie des donner en UART a l'ESP32
    SendWifiMessage((int8_t*)command, strlen(command));
    // Attendre pour que la requête est envoyée et que la réponse est reçue
    delay_msCt(100);  
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     ReadIpAndMac                                        *                                    
* Entrée :      char* inputStr, char* IpStr, char* MacStr                      *                                                                      
* Sortie :      true                                                           *
*               false                                                          *
* Desription :  Cette fonction permet de récuprer l'adresse IP et l'adresse MAC*
* Remarques:    Voir le prototype dans ESP8266.h.                              *
*******************************************************************************/
uint8_t ReadIpAndMac(char* inputStr, char* IpStr, char* MacStr)
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
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     ESP32_isStarted                                     *                                    
* Entrée :      Aucun                                                          *                                                                      
* Sortie :      bool                                                           *
*               true    si l'ESP32 est démarré et répond "OK"                  *
*               false   si l'ESP32 n'est pas démarré ou ne répond pas "OK"     *
* Description :  Cette fonction envoie une commande AT de vérification "AT"    *
*               pour tester si l'ESP32 est démarré. Elle attend ensuite une    *
*               réponse du module. Si la réponse reçue est "OK", la fonction   *
*               retourne true, sinon elle retourne false.                      *
*******************************************************************************/
uint8_t ESP32_isStarted(void)
{
    char cmdString[100] = "AT\r\n";   //création de la trame a envoyer
    int8_t StrRes[10] = {0};
    uint8_t Status = 0;
    
    //envoie de la trame
    SendATCommand(cmdString);
    delay_msCt(2);
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 2); 
    
    return Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     ESP32_ConnectWifi                                   *                                    
* Entrée :      char *SSID  - Pointeur vers une chaîne de caractères contenant *
*                 le SSID du réseau Wi-Fi auquel se connecter.                 *
*               char *PWD   - Pointeur vers une chaîne de caractères contenant *
*                 le mot de passe du réseau Wi-Fi.                             *                                                                      
* Sortie :      bool                                                           *
*               true    si la connexion au réseau Wi-Fi a réussi (réponse "OK")*
*               false   si la connexion a échoué ou si une réponse différente  *
*                       de "OK" est reçue.                                     *
* Description :  Cette fonction construit une commande AT pour connecter       *
*               l'ESP32 à un réseau Wi-Fi en utilisant le SSID et le mot de    *
*               passe fournis. La commande AT est ensuite envoyée au module    *
*               ESP32. La fonction attend une réponse du module et retourne    *
*               true si la réponse est "OK", sinon false.                      *
*******************************************************************************/
uint8_t ESP32_ConnectWifi(char *SSID, char *PWD)
{
    int8_t StrRes[200] = {0};
    uint8_t Status;
    
    //envoie des trames
    SendATCommand("AT+CWJAP=\"");
    SendWifiMessage((int8_t*)SSID, strlen((char*)SSID));
    delay_msCt(100);
    SendATCommand("\",\"");   
    SendWifiMessage((int8_t*)PWD, strlen((char*)PWD));
    delay_msCt(100);
    SendATCommand("\"\r\n\n");
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 11000);
    
    return Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     ESP32_ChangeMode                                    *                                    
* Entrée :      uint8_t mode                                                   *
*               - Mode à configurer pour l'ESP32.                              * 
*                               1 (Station)                                    *
*                               2 (Point d'accès)                              *
*                               3 (Station + Point d'accès).                   *                                                                      
* Sortie :      bool                                                           *
*               true    si le mode a été changé avec succès (réponse "OK")     *
*               false   si le changement de mode a échoué ou si une réponse    *
*                       différente de "OK" est reçue.                          *
* Description :  Cette fonction construit une commande AT pour changer le mode *
*               de l'ESP32 en fonction de la valeur fournie. La commande AT est*
*               ensuite envoyée au module ESP32. La fonction attend une réponse*
*               et retourne true si la réponse est "OK", sinon false.          *
*******************************************************************************/
uint8_t ESP32_ChangeMode(uint8_t mode)
{
    char cmdString[20] = {0};
    int8_t StrRes[18] = {0};
    int8_t Status;
    
    // Construire la commande AT complète
    snprintf(cmdString, sizeof(cmdString), "AT+CWMODE=%d\r\n", mode);
    
    // Envoie de la commande AT complète
    SendATCommand(cmdString);
    
    //récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 2);
    
    return Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     ESP32_GetIpAndMac                                   *                                    
* Entrée :      char* StrIP - Pointeur vers une chaîne de caractères où        *
*                 l'adresse IP récupérée sera stockée.                         *
*               char* StrMAC - Pointeur vers une chaîne de caractères où       *
*                 l'adresse MAC récupérée sera stockée.                        *                                                                      
* Sortie :      bool                                                           *
*               true    si la récupération de l'adresse IP et de l'adresse MAC *
*                       a réussi (réponse "OK" reçue).                         *
*               false   si la récupération a échoué ou si une réponse          *
*                       différente de "OK" est reçue.                          *
* Description :  Cette fonction envoie une commande AT pour récupérer l'adresse*
*               IP et l'adresse MAC de l'ESP32. La fonction attend une réponse *
*               et utilise une autre fonction pour extraire ces informations   *
*               de la réponse reçue.                                           *
*******************************************************************************/
uint8_t ESP32_GetIpAndMac(char* StrIP, char* StrMAC)
{
    int8_t StrRes[100] = {0};
    int8_t Status;
    
    // Envoie de la commande AT complète
    SendATCommand("AT+CIFSR\r\n");
    
    // Récuperation du status de la réception
    Status = GetWifiMessage(StrRes, "OK", 5);
    
    // extraire l'adresse IP et MAC 
    Status = ReadIpAndMac((char*)StrRes, StrIP, StrMAC);
    
    return Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     PublishMQTT                                         *                                    
* Entrée :      const char* topic - Pointeur vers une chaîne de caractères     *
*                 contenant le sujet (topic) du message MQTT à publier.        *
*               double value - La valeur numérique à publier dans le message   *
 *              MQTT.                                                          *
* Sortie :      Aucun                                                          *
* Description :  Cette fonction construit une commande AT pour publier un      *
*               message MQTT avec le sujet et la valeur spécifiés. La commande *
*               est ensuite envoyée au module en utilisant la fonction         *
*               SendATCommand.                                                 *
*******************************************************************************/
void PublishMQTT(const char* topic, double value) 
{
    char cmdString[100];
    
    // Construire la commande AT complète
    sprintf(cmdString, "AT+MQTTPUB=0,\"%s\",\"%.1f\",0,0\r\n", topic, value);
    // Envoie de la commande AT complète
    SendATCommand(cmdString);
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     SetupMQTTAndPublish                                 *                                    
* Entrée :      float temp_ext - Température extérieure.                       *
*               float taux_humidite - Taux d'humidité.                         *
*               float pression_air - Pression de l'air.                        *
*               uint8_t indice_qualite_air - Indice de qualité de l'air.       *
*               uint16_t concentration_CO2 - Concentration de CO2.             *                                                                      
* Sortie :      Aucun                                                          *
* Description :  Cette fonction configure les paramètres de l'utilisateur MQTT,*
*               se connecte à un broker MQTT, publie plusieurs messages MQTT   *
*               avec les données fournies, puis nettoie les sessions MQTT.     *
*******************************************************************************/
void SetupMQTTAndPublish(float temp_ext, float taux_humidite, float pression_air
                       , uint8_t indice_qualite_air, uint16_t concentration_CO2) 
{
    //alumer led 
    LED_RGB_ROff();
    LED_RGB_BOff();
    LED_RGB_GOff();
    
    // Configurer l'utilisateur MQTT
    SendATCommand("AT+MQTTUSERCFG=0,1,\"ESP32Client\",\"\",\"\",0,0,\"\"\r\n");

    // Se connecter au broker MQTT
    SendATCommand("AT+MQTTCONN=0,\"192.168.1.100\",1883,0\r\n");
    delay_msCt(500);
    
    // Publier les messages MQTT
    PublishMQTT("sensor/Temp", temp_ext);
    PublishMQTT("sensor/Hum", taux_humidite);
    PublishMQTT("sensor/Press", pression_air);
    PublishMQTT("sensor/Qualite", indice_qualite_air);
    PublishMQTT("sensor/CO2", concentration_CO2);


    // Nettoyer les sessions MQTT
    SendATCommand("AT+MQTTCLEAN=0\r\n");
    //eteindre led 
    LED_RGB_ROn();
    LED_RGB_BOn();
    LED_RGB_GOn();
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
uint8_t ESP32_ConnectToTCPServer(char *StrIP, char *StrPort)
{
    //alumer led 
    LED_RGB_ROff();
    LED_RGB_BOff();
    LED_RGB_GOff();
    
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
    
    //eteindre led 
    LED_RGB_ROn();
    LED_RGB_BOn();
    LED_RGB_GOn();
    
    return Status;
}
/*******************************************************************************
* Auteur : Joao marques                                                        *                     
* Nom de la fonction :     ESP32_DisconnectTCPServer                           *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet de déconnecter le module Wifi au serveur *
*               auquel il est connecté                                         *  
*******************************************************************************/
uint8_t ESP32_DisconnectTCPServer(void)
{
    int8_t StrRes[30] = {0};
    uint8_t Status;

    SendATCommand("AT+CIPCLOSE\r\n");
    
    Status = GetWifiMessage(StrRes, "OK", 10);
    
    return Status;
}

/*******************************************************************************
* Auteur : Joao Marques                                                        *                     
* Nom de la fonction :     SendTCP                                             *                                    
* Entrée :      void                                                           *
*                                                                              *                                                                        
* Sortie :      OK :        Status = 0                                         *
*               ERROR :     Status = 1                                         *
*               NO_DATA :   Status = 2                                         *
*               TIMEOUT :   Status = 3                                         *
* Desription :  Cette fonction permet d'envoyer une requêtte HTTP              *  
*******************************************************************************/
uint8_t ESP32_SendTCP(void)
{  
    uint8_t Status;

    Status = ESP32_ConnectToTCPServer(Domoticz.Ip,Domoticz.Port);
    delay_msCt(500);

    SendATCommand("AT+CIPSEND=16\r\n");
    delay_msCt(100);
    SendATCommand("2002_CapteurWifi");
    delay_msCt(100);
    
    ESP32_DisconnectTCPServer();
    
    
    return Status;
}

/*******************************************************************************
 End of File
 */
