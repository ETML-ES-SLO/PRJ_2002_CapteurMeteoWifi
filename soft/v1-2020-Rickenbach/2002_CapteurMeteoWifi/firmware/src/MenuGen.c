/* 
 * File:   MenuGen.c
 * Author: dierickenbach
 *
 * Created on 4. septembre 2020, 13:24
 */

//inlcudes
#include "MenuGen.h"
#include "Mc32gest_RS232.h"
#include "BME280.h"
#include "ESP8266.h"
#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"
#include "Mc32Delays.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Mc32NVMUtil.h"

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     GestMenu                                            *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de gérer les états du menu               *                                                    
* Remarques:    Voir le prototype dans MenuGen.h.                              *
*******************************************************************************/
void GestMenu (void)
{
    switch (MenuStatus)
    {
        case Enter_Menu_Set:
        {
            int8_t sendMess[] = "\r\nVeuillez appuyer sur la touche \"q\" pour acceder au menu\r\n\r\n";
            SendUSBMessage(sendMess, strlen((char*)sendMess));
            MenuStatus = Enter_Menu_Get;
            break;
        }
        case Enter_Menu_Get:
        {
            static uint16_t count5s = 0;
            int8_t getMess[5] = {0};
            
            if(count5s >= 50)
            {
                count5s = 0;
                MenuStatus = Enter_Menu_Set;
            }
            else
            {
                count5s++;
                
                if(GetUSBMessage(getMess, "q") == OK)
                {
                    LED_G_Off();
                    LED_B_On();
            
                    MenuStatus = Menu_Set;
                }
            } 
            break;
        }
        case QuitSave_Menu:
        {
            int8_t sendBye[] = "Merci et a bientot!\r\n";
            SendUSBMessage(sendBye, strlen((char*)sendBye));
            
            BME280.Magic = 0x1234;
            Wifi.Magic = 0x1234;
            ESP8266.Magic = 0x1234;     
                    
            memcpy(&Struct_save.BME280_save, &BME280, sizeof(S_BME280));
            memcpy(&Struct_save.ESP8266_save, &ESP8266, sizeof(S_ESP8266));
            memcpy(&Struct_save.Wifi_save, &Wifi, sizeof(S_WIFI));
    
            NVM_WriteBlock((uint32_t *)(&Struct_save),sizeof(S_SAVE));
            
            LED_G_On();
            BME280_Init();
            ESP8266_Init();
            LED_G_Off();
            
            LED_B_Off();
            
            MenuStatus = Enter_Menu_Set;
            
            APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        case Menu_Set: 
        {
            int8_t Line1[] = "\r\n\r\nQue voulez-vous faire?\r\n\r\n";
            int8_t Line2[] = "1: Modifier les regalges Wifi (SSID ou PWD)\r\n";
            int8_t Line3[] = "2: Mofifier les reglages Domoticz(IP, Port, Index)\r\n";
            int8_t Line4[] = "3: Modifier le temps entre chaques mesures\r\n";
            int8_t Line5[] = "4: Afficher les mesures du BME280\r\n";
            int8_t Line6[] = "5: Modifier l'altitude\r\n";
            int8_t Line7[] = "\"q\": Quitter et sauvegarder\r\n\r\n";

            SendUSBMessage(Line1, strlen((char*)Line1));
            SendUSBMessage(Line2, strlen((char*)Line2));
            SendUSBMessage(Line3, strlen((char*)Line3));
            SendUSBMessage(Line4, strlen((char*)Line4));
            SendUSBMessage(Line5, strlen((char*)Line5));
            SendUSBMessage(Line6, strlen((char*)Line6));
            SendUSBMessage(Line7, strlen((char*)Line7));
            
            MenuStatus = Menu_Get;
            break;
        }
        case Menu_Get: 
        {
            int8_t menuResponse[5];
            uint8_t status = GetUSBMessage(menuResponse,"1");
            if(status == OK){
                MenuStatus = Wifi_Set;
                break;
            }
            status = GetUSBMessage(menuResponse,"2");
            if(status == OK){
                MenuStatus = Domoticz_Set;
                break;
            }
            status = GetUSBMessage(menuResponse,"3");
            if(status == OK){
                MenuStatus = Interval_Set;
                break;
            }
            status = GetUSBMessage(menuResponse,"4");
            if(status == OK){
                MenuStatus = Measure;
                break;
            }
            status = GetUSBMessage(menuResponse,"5");
            if(status == OK){
                MenuStatus = Altitude_Set;
                break;
            }
            status = GetUSBMessage(menuResponse,"q");
            if(status == OK){
                MenuStatus = QuitSave_Menu;
                break;
            }
            break;
        }
        case Wifi_Set: 
        {
            int8_t Line1[] = "\r\nQue voulez-vous faire?\r\n\r\n";
            int8_t Line2[] = "1: Afficher les informations actuelles du Wifi\r\n";
            int8_t Line3[] = "2: Modifier le nom du reseau\r\n";
            int8_t Line4[] = "3: Modifier le mot de passe\r\n";
            int8_t Line5[] = "\"r\": Revenir en arriere\r\n\r\n";
            
            SendUSBMessage(Line1, strlen((char*)Line1));
            SendUSBMessage(Line2, strlen((char*)Line2));
            SendUSBMessage(Line3, strlen((char*)Line3));
            SendUSBMessage(Line4, strlen((char*)Line4));
            SendUSBMessage(Line5, strlen((char*)Line5));
            
            MenuStatus = Wifi_Get;
            break;
        }
        case Wifi_Get: 
        {
            int8_t menuResponse[10];
            
            if(GetUSBMessage(menuResponse,"1") == OK){
                MenuStatus = Wifi_Info;
                break;
            }         

            if(GetUSBMessage(menuResponse,"2") == OK){
                MenuStatus = Wifi_SSID_Set;
                break;
            }   
            
            if(GetUSBMessage(menuResponse,"3") == OK){
                MenuStatus = Wifi_PWD_Set;
                break;
            }  
            
            if(GetUSBMessage(menuResponse,"r") == OK){
                MenuStatus = Menu_Set;
                break;
            }  
            break;
        }
        case Wifi_Info:
        {
            int8_t Line1[30];
            int8_t Line2[30];
            int8_t Line3[30];
            
            sprintf((char*)Line1, "SSID : %s\r\n", Wifi.SSID);
            sprintf((char*)Line2, "PWD  : %s\r\n", Wifi.PWD);
            sprintf((char*)Line3, "IP   : %s\r\n", Wifi.IP);


            SendUSBMessage(Line1, strlen((char*)Line1));
            SendUSBMessage(Line2, strlen((char*)Line2));
            SendUSBMessage(Line3, strlen((char*)Line3));

            MenuStatus = Wifi_Set;
            break;
        }
        case Wifi_SSID_Set: 
        {
            int8_t Line1[] = "Entrez le nouveau nom : ";
            SendUSBMessage(Line1, strlen((char*)Line1));
            
            MenuStatus = Wifi_SSID_Get;
            break;
        }
        case Wifi_SSID_Get: 
        {
            int8_t nSSID[100]={0};
            
            if(GetUSBMessage(nSSID, "\r") == OK)
            {
                memcpy(Wifi.SSID, nSSID, sizeof(nSSID));
                
                int8_t Line1[] = "Nouveau nom de reseau enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                
                MenuStatus = Wifi_Set;
            }
            break;
        }
        case Wifi_PWD_Set: 
        {
            int8_t Line1[] = "Entrez le nouveau mot de passe : ";
            SendUSBMessage(Line1, strlen((char*)Line1));
            
            MenuStatus = Wifi_PWD_Get;
            break;
        }
        case Wifi_PWD_Get: 
        {
            int8_t nPWD[100]={0};
            
            if(GetUSBMessage(nPWD, "\r") == OK)
            {
                memmove(Wifi.PWD, nPWD, sizeof(nPWD));
                
                int8_t Line1[] = "Nouveau mot de passe enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                MenuStatus = Wifi_Set;
            }
            break;
        }
        case Domoticz_Set: 
        {
            int8_t Line1[] = "\r\nQue voulez-vous faire ?\r\n\r\n";
            int8_t Line2[] = "1: Afficher les informations actuelles du serveur\r\n";
            int8_t Line3[] = "2: Modfier l'IP du serveur\r\n";
            int8_t Line4[] = "3: Modifier le port du serveur\r\n";
            int8_t Line5[] = "4: Modifier l'index du module\r\n";
            int8_t Line6[] = "\"r\": Revenir en arriere\r\n\r\n";
            
            SendUSBMessage(Line1, strlen((char*)Line1));
            SendUSBMessage(Line2, strlen((char*)Line2));
            SendUSBMessage(Line3, strlen((char*)Line3));
            SendUSBMessage(Line4, strlen((char*)Line4));
            SendUSBMessage(Line5, strlen((char*)Line5));
            SendUSBMessage(Line6, strlen((char*)Line6));
            
            MenuStatus = Domoticz_Get;
            break;
        }
        case Domoticz_Get: 
        {
            int8_t menuResponse[10];
            
            if(GetUSBMessage(menuResponse,"1") == OK){
                MenuStatus = Domo_Info;
                break;
            }    
            
            if(GetUSBMessage(menuResponse,"2") == OK){
                MenuStatus = Domo_IP_Set;
                break;
            }         

            if(GetUSBMessage(menuResponse,"3") == OK){
                MenuStatus = Domo_Port_Set;
                break;
            }    

            if(GetUSBMessage(menuResponse,"4") == OK){
                MenuStatus = Domo_Index_Set;
                break;
            }
            
            if(GetUSBMessage(menuResponse,"r") == OK){
                MenuStatus = Menu_Set;
                break;
            }  
            break;
        }
        case Domo_Info:
        {        
            int8_t Line1[30];
            int8_t Line2[30];
            int8_t Line3[30];
            int8_t Line4[30];
            int8_t Line5[30];
            
            sprintf((char*)Line1, "IP       : %s\r\n", ESP8266.IP);
            sprintf((char*)Line2, "Port     : %s\r\n", Wifi.Port);
            sprintf((char*)Line3, "MAC      : %s\r\n", ESP8266.MAC);
            sprintf((char*)Line4, "Interval : %dmin\r\n", (ESP8266.Interval/20/60));
            sprintf((char*)Line5, "Index    : %d\r\n", BME280.Index);

            SendUSBMessage(Line1, strlen((char*)Line1));
            SendUSBMessage(Line2, strlen((char*)Line2));
            SendUSBMessage(Line3, strlen((char*)Line3));
            SendUSBMessage(Line4, strlen((char*)Line4));
            SendUSBMessage(Line5, strlen((char*)Line5));
            
            MenuStatus = Domoticz_Set;

            break;
        }
        case Domo_IP_Set: 
        {
            int8_t Line1[] = "Entrez la nouvelle IP : ";
            
            SendUSBMessage(Line1, strlen((char*)Line1));
            MenuStatus = Domo_IP_Get;
            break;
        }
        case Domo_IP_Get: 
        {
            int8_t nIP[100]={0};
            
            if(GetUSBMessage(nIP, "\r") == OK)
            {
                memmove(Wifi.IP, nIP, sizeof(nIP));
                
                int8_t Line1[] = "Nouvel IP enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                MenuStatus = Domoticz_Set;
            }
            break;
        }
        case Domo_Port_Set: 
        {
            int8_t Line1[] = "Entrez le nouveau port : ";
            SendUSBMessage(Line1, strlen((char*)Line1));
            
            MenuStatus = Domo_Port_Get;
            break;
        }
        case Domo_Port_Get: 
        {
            int8_t nPort[100]={0};
            
            if(GetUSBMessage(nPort, "\r") == OK)
            {
                memmove(Wifi.Port, nPort, strlen((char*)nPort));
                
                int8_t Line1[] = "Nouveau port enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                MenuStatus = Domoticz_Set;
            }
            break;
        }
        case Domo_Index_Set: 
        {
            int8_t Line1[] = "Entrez le nouvel index : ";
            
            SendUSBMessage(Line1, strlen((char*)Line1));
            MenuStatus = Domo_Index_Get;
            break;
        }
        case Domo_Index_Get: 
        {
            int8_t nIndex[5]={0};
            
            if(GetUSBMessage(nIndex, "\r") == OK)
            {
                BME280.Index = atoi((char*)nIndex);
                
                int8_t Line1[] = "Nouvel index enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                
                MenuStatus = Domoticz_Set;
            }
            break;
        }
        case Interval_Set: 
        {
            int8_t Line1[] = "Combien de temps voulez-vous entre chaque mesure? [min] : ";
            SendUSBMessage(Line1, strlen((char*)Line1));
            
            MenuStatus = Interval_Get;
            break;
        }
        case Interval_Get: 
        {
            int8_t nInterval[5]={0};
            uint8_t Intervalmin = 0;
            
            if(GetUSBMessage(nInterval, "\r") == OK)
            {
                Intervalmin = atoi((char*)nInterval);
                
                ESP8266.Interval = (20*60*Intervalmin); 
                
                int8_t Line1[50];
                sprintf((char*)Line1, "Les mesures se feront toutes les %dmin\r\n", Intervalmin);
                SendUSBMessage(Line1, strlen((char*)Line1));
                
                MenuStatus = Menu_Set;
            }
            break;
        }
        case Measure: 
        {
            int8_t Line1[30];
            int8_t Line2[30];
            int8_t Line3[30];
            
            if(BME280.Temperature != 0 && BME280.Humidite != 0 && BME280.Pression != 0)
            {
                sprintf((char*)Line1, "Il fait actuellement %.1f C\r\n", (double)BME280.Temperature);
                sprintf((char*)Line2, "L'humidite est de %.1f %%\r\n", (double)BME280.Humidite);
                sprintf((char*)Line3, "Et la pression est de %.1f hPa\r\n\r\n", (double)BME280.Pression);

                SendUSBMessage(Line1, strlen((char*)Line1));
                SendUSBMessage(Line2, strlen((char*)Line2));
                SendUSBMessage(Line3, strlen((char*)Line3));
            }
            else
            {
                int8_t Line4[] = "Pour le moment, aucunes mesures n'a ete faite\r\n";
                SendUSBMessage(Line4, strlen((char*)Line4));
            }
            
            MenuStatus = Menu_Set;
            break;
        }
        case Altitude_Set:
        {
            int8_t Line1[] = "Entrez votre altitude actuelle [m]: ";
            
            SendUSBMessage(Line1, strlen((char*)Line1));
            MenuStatus = Altitude_Get;
            break;
        }    
        case Altitude_Get:
        {
            int8_t nAltitude[5]={0};
            
            if(GetUSBMessage(nAltitude, "\r") == OK)
            {
                BME280.Altitude = atoi((char*)nAltitude);
                
                int8_t Line1[] = "Nouvelle altitude enregistre!\r\n";
                SendUSBMessage(Line1, strlen((char*)Line1));
                
                MenuStatus = Menu_Set;
            }
            break;
        }    
        default:
            break;
    }
    APP_UpdateState(APP_STATE_WAIT);
}