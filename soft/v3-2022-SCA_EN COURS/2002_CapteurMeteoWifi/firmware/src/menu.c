/* 
 * File:   menu.c
 * Author: dierickenbach
 *
 * Created on 4. septembre 2020, 13:24
 */

//inlcudes
#include "menu.h"
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

//Enumeration des differents statuts du menu
typedef enum  { MENU_ENTER_SET = 0, MENU_ENTER_GET,
                MENU_QUITSAVE, MENU_QUITNOSAVE,
                MENU_SET, MENU_GET, 
                MENU_WIFI_SET, MENU_WIFI_GET,
//                    MENU_WIFI_DISP,
                    MENU_WIFI_SSID_SET, MENU_WIFI_SSID_GET, 
                    MENU_WIFI_PWD_SET, MENU_WIFI_PWD_GET,
                MENU_DOMO_SET, MENU_DOMO_GET,
//                    MENU_DOMO_DISP,
                    MENU_DOMO_IP_SET, MENU_DOMO_IP_GET, 
                    MENU_DOMO_PORT_SET, MENU_DOMO_PORT_GET, 
                    MENU_DOMO_IDX_SET, MENU_DOMO_IDX_GET, 
                MENU_PERIOD_SET, MENU_PERIOD_GET,                        
                MENU_ALTITUDE_SET, MENU_ALTITUDE_GET,
                MENU_MEASURE_DISP, 
                  } S_MenuStatus;
                
                
S_MenuStatus MenuStatus;


/*******************************************************************************
* Auteur : SCA                                                                 *                     
* Nom de la fonction :     MENU_Init                                           *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Initialise menu                                                *                                                    
* Remarques:    Voir le prototype dans menu.h.                                 *
*******************************************************************************/
void MENU_Init(void)
{
    MenuStatus = MENU_ENTER_SET;   
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     MENU_Mgmt                                            *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de gérer les états du menu               *                                                    
* Remarques:    Voir le prototype dans menu.h.                              *
*******************************************************************************/
void MENU_Mgmt (void)
{
    uint8_t i;
                
    switch (MenuStatus)
    {
        case MENU_ENTER_SET:
        {
            const int8_t strMenu[] = "\r\nAppuyez sur espace pour acceder au menu\r\n\r\n";
            SendUSBMessage(strMenu, -1);
            MenuStatus = MENU_ENTER_GET;
            break;
        }
        case MENU_ENTER_GET:
        {
            static uint16_t count5s = 0;
            
            if(count5s >= 5*FREQ_SERVICE_TASKS)
            {
                count5s = 0;
                MenuStatus = MENU_ENTER_SET;
            }
            else
            {
                count5s++;
                
                if(ReadSingleCharFromUsb(' ') == OK)
                {
//                    LED_G_Off();
//                    LED_B_On();           
                    MenuStatus = MENU_SET;
                }
            } 
            break;
        }
        case MENU_QUITSAVE:
        {
            const int8_t strMenu[] = "Merci et a bientot!\r\n";
            SendUSBMessage(strMenu, -1);
            
            BME280.Magic = 0x12345678;
            Wifi.Magic = 0x12345678;
            ESP8266.Magic = 0x12345678;     
                    
            memcpy(&Struct_save.BME280_save, &BME280, sizeof(S_BME280));
            memcpy(&Struct_save.ESP8266_save, &ESP8266, sizeof(S_ESP8266));
            memcpy(&Struct_save.Wifi_save, &Wifi, sizeof(S_WIFI));
    
            NVM_WriteBlock((uint32_t *)(&Struct_save),sizeof(S_SAVE));
            
            //rouge pour indiquer init
            LED_R_Off();
            LED_G_On();
            LED_B_On();            
            //Initialisation des Modules           
            BME280_Init();
            ESP8266_Init();
            
            MenuStatus = MENU_ENTER_SET;
            
            //todo SCA : adapter
            //APP_UpdateState(APP_STATE_WAIT);
            break;
        }
        case MENU_QUITNOSAVE:
        {
            const int8_t strMenu[] = "Merci et a bientot!\r\n";
            SendUSBMessage(strMenu, -1);
            MenuStatus = MENU_ENTER_SET;
            break;
        }
        case MENU_SET: 
        {
            //chaines du menu
            char strMenu[8][50] = { "\r\n\r\nFaites un choix :\r\n",
                                    " 1: Reglages Wifi (SSID ou PWD)\r\n",
                                    " 2: Reglages Domoticz(IP, Port, Index)\r\n",
                                    " 3: Periode de mesure\r\n",
                                    " 4: Altitude\r\n",           
                                    " 5: Afficher les mesures du BME280\r\n",
                                    " \'q\': Quitter et sauvegarder\r\n",
                                    " \'esc\': Quitter sans sauvegarder\r\n\r\n",
                                    };
            
            //envoi
            for(i=0 ; i<8 ; i++)
                SendUSBMessage((int8_t*)strMenu[i], -1);
            
            MenuStatus = MENU_GET;
            break;
        }
        case MENU_GET: 
        {
            if (ReadSingleCharFromUsb('1') == OK)
            {
                MenuStatus = MENU_WIFI_SET;
                break;
            } else if (ReadSingleCharFromUsb('2') == OK)
            {
                MenuStatus = MENU_DOMO_SET;
                break;
            } else if (ReadSingleCharFromUsb('3') == OK)
            {
                MenuStatus = MENU_PERIOD_SET;
                break;
            } else if (ReadSingleCharFromUsb('4') == OK)
            {
                MenuStatus = MENU_ALTITUDE_SET;
                break;
            } else if (ReadSingleCharFromUsb('5') == OK)
            {
                MenuStatus = MENU_MEASURE_DISP;
                break;
            } else if (ReadSingleCharFromUsb('6') == OK)
            {
                MenuStatus = MENU_QUITSAVE;
                break;
            } else if (ReadSingleCharFromUsb('\e') == OK)
            {
                MenuStatus = MENU_QUITNOSAVE;
                break;    
            }
            break;
        }
        case MENU_WIFI_SET: 
        {           
            //chaines du menu
            char strMenu[10][50] = { "\r\nReglages WIFI actuels :\r\n",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "",
                                    "\r\nFaites un choix :\r\n",
                                    " 1: Modifier le nom du reseau\r\n",
                                    " 2: Modifier le mot de passe\r\n",            
                                    " \"r\": Revenir en arriere\r\n\r\n",                   
                                    };
            
            //remplissage chaines dynamiques
            sprintf(strMenu[1], " SSID : %s\r\n", Wifi.Ssid);
            sprintf(strMenu[2], " PWD  : %s\r\n", Wifi.Pwd);
            sprintf(strMenu[3], " IP   : %s\r\n", ESP8266.IP);
            sprintf(strMenu[4], " MAC  : %s\r\n", ESP8266.MAC);
            sprintf(strMenu[5], " Connexion WIFI : %s\r\n", ESP8266.Status? "OK" : "NOK");            
            
            //envoi
            for(i=0 ; i<10 ; i++)
                SendUSBMessage((int8_t*)strMenu[i], -1);
            
            MenuStatus = MENU_WIFI_GET;
            break;
        }
        case MENU_WIFI_GET: 
        {
            if (ReadSingleCharFromUsb('1') == OK)
            {
                MenuStatus = MENU_WIFI_SSID_SET;
                break;
            } else if (ReadSingleCharFromUsb('2') == OK)
            {
                MenuStatus = MENU_WIFI_PWD_SET;
                break;
            } else if (ReadSingleCharFromUsb('r') == OK)
            {
                MenuStatus = MENU_SET;
                break;
            }  
            break;
        }
        case MENU_WIFI_SSID_SET: 
        {
            const int8_t strMenu[] = "Entrez le nouveau nom, \'esc\' pour annuler : ";
            SendUSBMessage(strMenu, -1);
            
            MenuStatus = MENU_WIFI_SSID_GET;
            break;
        }
        case MENU_WIFI_SSID_GET: 
        {
            char strSsid[100];
            int8_t retVal;
            
            retVal = ReadStrFromUsb(strSsid);
            
            if (retVal == OK)
            {
                //TODO ici il faudrait vérifier la validité de l'entrée avant de copier !
                strcpy(Wifi.Ssid, strSsid);
                MenuStatus = MENU_WIFI_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_WIFI_SET;    
            }
            break;
        }
        case MENU_WIFI_PWD_SET: 
        {
            const int8_t strMenu[] = "Entrez le nouveau mot de passe, \'esc\' pour annuler : ";
            SendUSBMessage(strMenu, -1);
            
            MenuStatus = MENU_WIFI_PWD_GET;
            break;
        }
        case MENU_WIFI_PWD_GET: 
        {            
            char strPwd[100];
            int8_t retVal;
            
            retVal = ReadStrFromUsb(strPwd);
            
            if (retVal == OK)
            {
                //TODO ici il faudrait vérifier la validité de l'entrée avant de copier !
                strcpy(Wifi.Pwd, strPwd);
                MenuStatus = MENU_WIFI_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_WIFI_SET;    
            }
            break;
        }
        case MENU_DOMO_SET: 
        {
            //chaines du menu
            char strMenu[9][50] = { "\r\nReglages Domoticz actuels :\r\n",
                                    "",
                                    "",
                                    "",
                                    "\r\nFaites un choix :\r\n",
                                    " 1: Modfier l'IP du serveur\r\n",
                                    " 2: Modifier le port du serveur\r\n",
                                    " 3: Modifier l'index du module\r\n",
                                    " \"r\": Revenir en arriere\r\n\r\n",
                                  };

            //remplissage chaines dynamiques
            sprintf(strMenu[1], " IP    : %s\r\n", Wifi.DomoIp);
            sprintf(strMenu[2], " Port  : %s\r\n", Wifi.DomoPort);
            sprintf(strMenu[3], " Index : %s\r\n", Wifi.DomoIdx);
            
            //envoi
            for(i=0 ; i<9 ; i++)
                SendUSBMessage((int8_t*)strMenu[i], -1);
            
            MenuStatus = MENU_DOMO_GET;
            break;
        }
        case MENU_DOMO_GET: 
        {             
            if (ReadSingleCharFromUsb('1') == OK)
            {
                MenuStatus = MENU_DOMO_IP_SET;
                break;
            } else if (ReadSingleCharFromUsb('2') == OK)
            {
                MenuStatus = MENU_DOMO_PORT_SET;
                break;
            } else if (ReadSingleCharFromUsb('3') == OK)
            {
                MenuStatus = MENU_DOMO_IDX_SET;
                break;
            } else if (ReadSingleCharFromUsb('r') == OK)
            {
                MenuStatus = MENU_SET;
                break;
            }  
            break;
        }
        case MENU_DOMO_IP_SET: 
        {
            int8_t strMenu[] = "Entrez la nouvelle IP, \'esc\' pour annuler : ";
            
            SendUSBMessage(strMenu, -1);
            MenuStatus = MENU_DOMO_IP_GET;
            break;
        }
        case MENU_DOMO_IP_GET: 
        {
            char strDomoIp[100];
            int8_t retVal;
            
            retVal = ReadStrFromUsb(strDomoIp);
            
            if (retVal == OK)
            {
                //TODO ici il faudrait vérifier la validité de l'entrée avant de copier !
                strcpy(Wifi.DomoIp, strDomoIp);
                MenuStatus = MENU_DOMO_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_DOMO_SET;    
            }
            break;
        }
        case MENU_DOMO_PORT_SET: 
        {
            const int8_t strMenu[] = "Entrez le nouveau port, \'esc\' pour annuler : ";
            SendUSBMessage(strMenu, -1);
            
            MenuStatus = MENU_DOMO_PORT_GET;
            break;
        }
        case MENU_DOMO_PORT_GET: 
        {
            char strDomoPort[100];
            int8_t retVal;
            
            retVal = ReadStrFromUsb(strDomoPort);
            
            if (retVal == OK)
            {
                //TODO ici il faudrait vérifier la validité de l'entrée avant de copier !
                strcpy(Wifi.DomoIdx, strDomoPort);
                MenuStatus = MENU_DOMO_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_DOMO_SET;    
            }
            break;
        }
        case MENU_DOMO_IDX_SET: 
        {
            const int8_t strMenu[] = "Entrez le nouvel index, \'esc\' pour annuler : ";
            
            SendUSBMessage(strMenu, -1);
            MenuStatus = MENU_DOMO_IDX_GET;
            break;
        }
        case MENU_DOMO_IDX_GET: 
        {
            char strDomoIdx[100];
            int8_t retVal;
            
            retVal = ReadStrFromUsb(strDomoIdx);
            
            if (retVal == OK)
            {
                //TODO ici il faudrait vérifier la validité de l'entrée avant de copier !
                strcpy(Wifi.DomoIdx, strDomoIdx);
                MenuStatus = MENU_DOMO_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_DOMO_SET;    
            }             
            break;
        }
        case MENU_PERIOD_SET: 
        {
            //chaines du menu
            char strMenu[2][60] = { "",
                                    "\r\nEntrez la nouvelle periode, \'esc\' pour annuler : ",
                                  };
                
            //remplissage chaines dynamiques
            sprintf(strMenu[0], "\r\nPeriode de mesure actuelle : %5d [s]\r\n", BME280.Period);
            
            //envoi
            for(i=0 ; i<2 ; i++)
                SendUSBMessage((int8_t*)strMenu[i], -1);
            
            MenuStatus = MENU_PERIOD_GET;
            break;
        }
        case MENU_PERIOD_GET: 
        {
            char strPeriod[100];
            int8_t retVal;
            uint32_t newPeriod;
            char* nextStr;
            
            retVal = ReadStrFromUsb(strPeriod);
            
            if (retVal == OK)
            {
                newPeriod = strtol(strPeriod, &nextStr, 10);
                if (newPeriod >= 5 && nextStr > strPeriod) //5 sec minmum d'intervalle entre mesures et trouvé un nombre ?
                    BME280.Period = newPeriod;
                MenuStatus = MENU_PERIOD_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_PERIOD_SET;    
            } 
            break;
        }
        case MENU_ALTITUDE_SET:
        {
            //chaines du menu
            char strMenu[2][60] = { "",
                                    "\r\nEntrez la nouvelle altitude, \'esc\' pour annuler : ",
                                  };
                
            //remplissage chaines dynamiques
            sprintf(strMenu[0], "\r\nAltitude actuelle : %5d [m]\r\n", BME280.Altitude);
            
            //envoi
            for(i=0 ; i<2 ; i++)
                SendUSBMessage((int8_t*)strMenu[i], -1);
                       
            MenuStatus = MENU_ALTITUDE_GET;
            break;
        }    
        case MENU_ALTITUDE_GET:
        {
            char strAltitude[100];
            int8_t retVal;
            uint32_t newAltitude;
            char* nextStr;
            
            retVal = ReadStrFromUsb(strAltitude);
            
            if (retVal == OK)
            {
                newAltitude = strtol(strAltitude, &nextStr, 10);
                if (newAltitude >= 0 && nextStr > strAltitude) //valeur >= 0 et trouvé un nombre ?
                    BME280.Altitude = newAltitude;
                MenuStatus = MENU_ALTITUDE_SET;    
            }
            else if (retVal == ESCAPE)
            {
                MenuStatus = MENU_ALTITUDE_SET;    
            }          
            break;
        }    
        case MENU_MEASURE_DISP: 
        {
            //chaines du menu
            char strMenu[4][50] = { "\r\nMesures actuelles :\r\n",
                                    "",
                                    "",
                                    "",
                                    };
                                  
            if(BME280.Temperature != 0.0 && BME280.Humidite != 0.0 && BME280.Pression != 0.0)
            {   
                //remplissage chaines dynamiques
                sprintf((char*)strMenu[1], " Temperature : %.1f C\r\n", (double)BME280.Temperature);
                sprintf((char*)strMenu[2], " Humidite    : %.1f %%\r\n", (double)BME280.Humidite);
                sprintf((char*)strMenu[3], " Pression    : %.1f hPa\r\n\r\n", (double)BME280.Pression);
                
                //envoi   
                for(i=0 ; i<4 ; i++)
                   SendUSBMessage((int8_t*)strMenu[i], -1);                 
            }
            else
            {
                strcpy(strMenu[1], " *** Pas de mesure ***\r\n\r\n");
                
                //envoi   
                for(i=0 ; i<2 ; i++)
                   SendUSBMessage((int8_t*)strMenu[i], -1); 
            }
            
            MenuStatus = MENU_SET;
            break;
        }        
        default:
            break;
    }
    //todo SCA : adapter
    //APP_UpdateState(APP_STATE_WAIT);
}