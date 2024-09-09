/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Gest_Menu.h"
#include "Mc32gest_RS232.h"
#include "ESP32.h"
#include "ENS160.h"
#include "BME280.h"
#include "app.h"
    
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
    uint8_t i;

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
  
/*******************************************************************************
* Auteur : SCA                                                                 *                     
* Nom de la fonction :     MENU_Init                                           *                                    
* Entrée :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Initialise menu                                                *                                                    
* Remarques:    Voir le prototype dans menu.h.                                 *
*******************************************************************************/
void Menu_init(void)
{
    MenuState = MENU_ENTER_SET;   
}    
    
/*******************************************************************************
* Auteur : Joao Marques                                                        *
* Nom de la fonction : State_Menu                                               *
* Entrée : Aucun                                                               *
* Sortie : Aucun                                                               *
* Description : Cette fonction gère l'affichage et la navigation dans le menu  *
*               utilisateur via USB. Elle prend en charge les réglages de      *
*               différents paramètres du système comme le Wi-Fi et Domoticz.   *
*******************************************************************************/
void State_Menu (void)
{
    static uint16_t count5s = 0; // Déclaration statique pour conserver l'état entre les appels

    
    switch (MenuState)
    {
        case MENU_ENTER_SET:
        {
            const int8_t strMenu[] = "\r\nAppuyez sur espace pour acceder au menu\r\n\r\n";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ENTER_GET;
            break;
        }

        case MENU_ENTER_GET:
        {
            if(count5s >= 5 * FREQ_SERVICE_TASKS)
            {
                count5s = 0;
                MenuState = MENU_ENTER_SET;
            }
            else
            {
                count5s++;
                if(ReadSingleCharFromUsb(' ') == OK)
                {         
                    MenuState = MENU_SET;
                }
            } 
            break;
        }

        case MENU_SAVE:
        {
            //reste a gerer avec controle de donnée
            GestSave();
            break;
        }
        case MENU_EXIT:
        {
            const int8_t strMenu[] = "Merci et a bientot!\r\n";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ENTER_SET;
            break;
        }

        case MENU_SET:
        {
            DisplayMainMenu();
            RecupValueInSytm();
            MenuState = MENU_GET;
            break;
        }

        case MENU_GET:
        {
            MainMenuSelection();
            break;
        }

        case MENU_WIFI_SET:
        {
            DisplayWifiSettings();
            MenuState = MENU_WIFI_GET;
            break;
        }

        case MENU_WIFI_GET:
        {
            WifiMenuSelection();
            break;
        }

        case MENU_WIFI_SSID_SET:
        {
            const int8_t strMenu[] ="Entrez le nouveau nom : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_WIFI_SSID_GET;
            break;
        }

        case MENU_WIFI_SSID_GET:
        {
            MenuInput(DeviceConfig.WifiSSID, MENU_WIFI_SET);
            break;
        }

        case MENU_WIFI_PWD_SET:
        {
            const int8_t strMenu[] ="Entrez le nouveau mot de passe : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_WIFI_PWD_GET;
            break;
        }

        case MENU_WIFI_PWD_GET:
        {
            MenuInput(DeviceConfig.WifiPWD, MENU_WIFI_SET);
            break;
        }

        case MENU_MQTT_SET:
        {
            DisplayMqttSettings();
            MenuState = MENU_MQTT_GET;
            break;
        }

        case MENU_MQTT_GET:
        {
            MqttMenuSelection();
            break;
        }

        case MENU_MQTT_IP_SET:
        {
            const int8_t strMenu[] ="Entrez la nouvelle IP : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_MQTT_IP_GET;
            break;
        }

        case MENU_MQTT_IP_GET:
        {
            MenuInput(DeviceConfig.MqttHost,MENU_MQTT_SET);
            break;
        }
        
        case MENU_MQTT_PORT_SET:
        {
            const int8_t strMenu[] ="Entrez le nouveau port : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_MQTT_PORT_GET;
            break;
        }
        
        case MENU_MQTT_PORT_GET:
        {
            MenuInput(DeviceConfig.MqttPort,MENU_MQTT_SET);
            break;
        }
        
        case MENU_TCP_SET:
        {
            DisplayDomoSettings();
            MenuState = MENU_TCP_GET;
            break;
        }

        case MENU_TCP_GET:
        {
            DomoMenuSelection();
            break;
        }

        case MENU_TCP_IP_SET:
        {
            const int8_t strMenu[] ="Entrez la nouvelle IP : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_TCP_IP_GET;
            break;
        }

        case MENU_TCP_IP_GET:
        {
            MenuInput(DeviceConfig.TcpIp,MENU_TCP_SET);
            break;
        }

        case MENU_TCP_PORT_SET:
        {
            const int8_t strMenu[] ="Entrez le nouveau port : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_TCP_PORT_GET;
            break;
        }

        case MENU_TCP_PORT_GET:
        {
            MenuInput(DeviceConfig.TcpPort,MENU_TCP_SET);
            break;
        }
        case MENU_ECART_SET:
        {
            DisplayEcartSettings();
            MenuState = MENU_ECART_GET;
            break;
        }

        case MENU_ECART_GET:
        {
            EcartMenuSelection();
            break;
        }

        case MENU_ECART_TEMP_SET:
        {
            const int8_t strMenu[] ="Entrez l'ecart de temperature : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ECART_TEMP_GET;
            break;
        }

        case MENU_ECART_TEMP_GET:
        {
            DeviceConfig.EcartTemp = ValueInput( DeviceConfig.EcartTemp, MENU_ECART_SET);
            break;
        }
        
        case MENU_ECART_HUM_SET:
        {
            const int8_t strMenu[] ="Entrez l'ecart d'humidite : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ECART_HUM_GET;
            break;
        }
        
        case MENU_ECART_HUM_GET:
        {
            DeviceConfig.EcartHum = ValueInput( DeviceConfig.EcartHum, MENU_ECART_SET);
            break;
        }
        
        case MENU_ECART_PRESS_SET:
        {
            const int8_t strMenu[] ="Entrez l'ecart de pression : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ECART_PRESS_GET;
            break;
        }
        
        case MENU_ECART_PRESS_GET:
        {
            DeviceConfig.EcartPress = ValueInput( DeviceConfig.EcartPress, MENU_ECART_SET);
            break;
        }
        
        case MENU_ECART_QUALITE_SET:
        {
            const int8_t strMenu[] ="Entrez l'ecart de la qualite d'aire : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ECART_QUALITE_GET;
            break;
        }
        
        case MENU_ECART_QUALITE_GET:
        {
            DeviceConfig.EcartQualite = ValueInput( DeviceConfig.EcartQualite, MENU_ECART_SET);
            break;
        }
        
        case MENU_PERIOD_SET:
        {
            DisplayPeriodSettings();
            MenuState = MENU_PERIOD_GET;
            break;
        }

        case MENU_PERIOD_GET:
        {
            PeriodeMenuSelection();
            break;
        }
            
        case MENU_PERIOD_Value_SET:
        {
            const int8_t strMenu[] ="Entrez la nouvelle periode : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_PERIOD_Value_GET;
            break;
        }
            
        case MENU_PERIOD_Value_GET:
        {
            DeviceConfig.Period = ValueInput( DeviceConfig.Period, MENU_PERIOD_SET);
            break;
        }

        case MENU_ALTITUDE_SET:
        {
            DisplayAltitudeSettings();
            MenuState = MENU_ALTITUDE_GET;
            break;
        }

        case MENU_ALTITUDE_GET:
        {
            AltitudeMenuSelection();
            break;
        }
        
        case MENU_ALTITUDE_Value_SET:
        {
            const int8_t strMenu[] ="Entrez la nouvelle altitude : ";
            SendUSBMessage(strMenu, -1);
            MenuState = MENU_ALTITUDE_Value_GET;
            break;
        }

        case MENU_ALTITUDE_Value_GET:
        {
            DeviceConfig.Altitude = ValueInput( DeviceConfig.Altitude, MENU_ALTITUDE_SET);
            break;
        }

        case MENU_MEASURE_DISP:
        {
            DisplayCurrentMeasurements();
            MenuState = MENU_MEASURE_GET;
            break;
        }
        
        case MENU_MEASURE_GET:
        {
            ExitGet();
            break;
    }
               
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayMainMenu                                         *
* Description : Affiche le menu principal                                      *
*******************************************************************************/
void DisplayMainMenu(void)
{
    char strMenu[][100] = {
        "\r\n\r\n================================================\r\n",
        "                  MENU PRINCIPAL                  \r\n",
        "================================================\r\n\r\n",
        "   [1]   Reglages Wifi (SSID ou PWD)\r\n",
        "   [2]   Reglages MQTT(IP, Port)\r\n",
        "   [3]   Reglages TCP(IP, Port)\r\n",
        "   [4]   Ecart des mesure \r\n",
        "   [5]   Periode de mesure\r\n",
        "   [6]   Altitude\r\n",           
        "   [7]   Afficher les mesures du capteur meteo\r\n",
        "   [8]   sauvegarder\r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    for (i = 0; i < sizeof(strMenu) / sizeof(strMenu[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenu[i], -1);
    }
}
/*******************************************************************************
* Nom de la fonction : GetUserSelection                                        *
* Description : Lit une sélection de l'utilisateur via USB                     *
* Retour : Caractère représentant la sélection de l'utilisateur                *
*******************************************************************************/
int8_t GetUserSelection(void)
{
    // Remplace les appels répétitifs de ReadSingleCharFromUsb
    int8_t chars[] = {'1', '2', '3', '4', '5', '6', '7','8', '\e'};
    for (i = 0; i < sizeof(chars); i++)
    {
        if (ReadSingleCharFromUsb(chars[i]) == OK)
        {
            return chars[i];
        }
    }
    return -1; // Aucun caractère valide lu
}

/*******************************************************************************
* Nom de la fonction : HandleMainMenuSelection                                 *
* Description : Gère les sélections du menu principal                          *
*******************************************************************************/
void MainMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_WIFI_SET; break;
        case '2': MenuState = MENU_MQTT_SET; break;
        case '3': MenuState = MENU_TCP_SET; break;
        case '4': MenuState = MENU_ECART_SET; break;
        case '5': MenuState = MENU_PERIOD_SET; break;
        case '6': MenuState = MENU_ALTITUDE_SET; break;
        case '7': MenuState = MENU_MEASURE_DISP; break;
        case '8': MenuState = MENU_SAVE; break;
        case '\e': MenuState = MENU_EXIT; break;
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayWifiSettings                                     *
* Description : Affiche le menu Wifi                                           *
*******************************************************************************/
void DisplayWifiSettings(void)
{
    char strMenuWifi[][100] = {
        "\r\n\r\n================================================\r\n",
        "                  REGLAGE  Wi-fi                  \r\n",
        "================================================\r\n\r\n",
        "",
        "",
        "",
        "",
        "",
        "   [1]   Modifier le nom du reseau \r\n",
        "   [2]   Modifier le mot de passe \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuWifi[3], "         SSID : %s\r\n", DeviceConfig.WifiSSID);
    sprintf(strMenuWifi[4], "         PWD  : %s\r\n", DeviceConfig.WifiPWD);
    sprintf(strMenuWifi[5], "         IP   : %s\r\n", ESP32.IP);
    sprintf(strMenuWifi[6], "         MAC  : %s\r\n", ESP32.MAC);
    sprintf(strMenuWifi[7], "         Connexion WIFI : %s\r\n", 
            ESP32.Status? "OK" : "NOK"); 
    
    for ( i = 0; i < sizeof(strMenuWifi) / sizeof(strMenuWifi[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuWifi[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : WifiMenuSelection                                       *
* Description : Gère les sélections du menu Wi-fi                              *
*******************************************************************************/
void WifiMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_WIFI_SSID_SET; break;
        case '2': MenuState = MENU_WIFI_PWD_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayMqttSettings                                     *
* Description : Affiche le menu mosquitto                                      *
*******************************************************************************/
void DisplayMqttSettings(void)
{
    char strMenuMqtt[][100] = {
        "\r\n\r\n================================================\r\n",
        "                   REGLAGE MQTT                   \r\n",
        "================================================\r\n\r\n",
        "",
        "",
        "   [1]   Modifier l'IP \r\n",
        "   [2]   Modifier le PORT \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuMqtt[3], "         IP : %s\r\n", DeviceConfig.MqttHost);
    sprintf(strMenuMqtt[4], "         PORT  : %s\r\n", DeviceConfig.MqttPort);
    
    for ( i = 0; i < sizeof(strMenuMqtt) / sizeof(strMenuMqtt[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuMqtt[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : MqttMenuSelection                                       *
* Description : Gère les sélections du menu Mosquitto                          *
*******************************************************************************/
void MqttMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_MQTT_IP_SET; break;
        case '2': MenuState = MENU_MQTT_PORT_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}
/*******************************************************************************
* Nom de la fonction : DisplayWifiSettings                                     *
* Description : Affiche le menu Wifi                                           *
*******************************************************************************/
void DisplayEcartSettings(void)
{
    char strMenuEcart[][100] = {
        "\r\n\r\n================================================\r\n",
        "                  REGLAGE  Ecart                  \r\n",
        "================================================\r\n\r\n",
        "",
        "",
        "",
        "",
        "   [1]   Modifier l'ecart pour temperature \r\n",
        "   [2]   Modifier l'ecart pour humidite \r\n",
        "   [3]   Modifier l'ecart pour pression \r\n",
        "   [4]   Modifier l'ecart pour qualite \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuEcart[3], "         Temp    : %5d\r\n", DeviceConfig.EcartTemp);
    sprintf(strMenuEcart[4], "         Hum     : %5d\r\n", DeviceConfig.EcartHum);
    sprintf(strMenuEcart[5], "         Press   : %5d\r\n", DeviceConfig.EcartPress);
    sprintf(strMenuEcart[6], "         Qualite : %5d\r\n", DeviceConfig.EcartQualite);
    
    for ( i = 0; i < sizeof(strMenuEcart) / sizeof(strMenuEcart[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuEcart[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : WifiMenuSelection                                       *
* Description : Gère les sélections du menu Wi-fi                              *
*******************************************************************************/
void EcartMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_ECART_TEMP_SET; break;
        case '2': MenuState = MENU_ECART_HUM_SET; break;
        case '3': MenuState = MENU_ECART_PRESS_SET; break;
        case '4': MenuState = MENU_ECART_QUALITE_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}

/*******************************************************************************
* Nom de la fonction : MqttIpInput                                             *
* Description : Lit une sélection de l'utilisateur via USB                     *
* Retour : Caractère représentant la sélection de l'utilisateur                *
*******************************************************************************/
void MenuInput(char *configField,int8_t State)
{
    char strSsid[100];
    int8_t retVal;
    
    retVal = ReadStrFromUsb(strSsid);
    
    if (retVal == OK)
    {

        strcpy(configField, strSsid);
        MenuState = State;
    }
    
}

/*******************************************************************************
* Nom de la fonction : ValueInput                                              *
* Description : Lit une sélection de l'utilisateur via USB                     *
* Retour : Caractère représentant la sélection de l'utilisateur                *
*******************************************************************************/
uint32_t ValueInput(uint32_t oldvalue ,int8_t State)
{
    char strAltitude[100];
    int8_t retVal;
    uint32_t newAltitude = 0;
    char* nextStr;
    
    retVal = ReadStrFromUsb(strAltitude);

    if (retVal == OK)
    {
        newAltitude = strtol(strAltitude, &nextStr, 10);
        //valeur >= 0 et trouvé un nombre ?
        if (newAltitude >= 0 && nextStr > strAltitude)MenuState = State;
    }

    return newAltitude;
}
/*******************************************************************************
* Nom de la fonction : DisplayDomoSettings                                     *
* Description : Affiche le menu Domoticz                                       *
*******************************************************************************/
void DisplayDomoSettings(void)
{
    char strMenuDomo[][100] = {
        "\r\n\r\n================================================\r\n",
        "                   REGLAGE TCP                    \r\n",
        "================================================\r\n\r\n",
        "",
        "",
        "   [1]   Modifier l'IP \r\n",
        "   [2]   Modifier le PORT \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuDomo[3], "         IP : %s\r\n", DeviceConfig.TcpIp);
    sprintf(strMenuDomo[4], "         PORT  : %s\r\n", DeviceConfig.TcpPort);
    
    for ( i = 0; i < sizeof(strMenuDomo) / sizeof(strMenuDomo[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuDomo[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : DomoMenuSelection                                       *
* Description : Gère les sélections du menu Domoticz                           *
*******************************************************************************/
void DomoMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_TCP_IP_SET; break;
        case '2': MenuState = MENU_TCP_PORT_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayPeriodSettings                                   *
* Description : Affiche le menu Periode                                        *
*******************************************************************************/
void DisplayPeriodSettings(void)
{
    char strMenuPeriod[][100] = {
        "\r\n\r\n================================================\r\n",
        "                 REGLAGE Periode                  \r\n",
        "================================================\r\n\r\n",
        "",
        "   [1]   Modifier la periode de mesure \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuPeriod[3], "         Periode : %5d [s]\r\n", DeviceConfig.Period);
    
    for ( i = 0; i < sizeof(strMenuPeriod) / sizeof(strMenuPeriod[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuPeriod[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : PeriodeMenuSelection                                    *
* Description : Gère les sélections du menu periode                            *
*******************************************************************************/
void PeriodeMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_PERIOD_Value_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayAltitudeSettings                                 *
* Description : Affiche le menu Altitude                                       *
*******************************************************************************/
void DisplayAltitudeSettings(void)
{
    char strMenuAlt[][100] = {
        "\r\n\r\n================================================\r\n",
        "                REGLAGE  Altitude                 \r\n",
        "================================================\r\n\r\n",
        "",
        "   [1]   Modifier l'altitude \r\n",
        "  [esc]  Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf(strMenuAlt[3], "         Altitude : %5d [m]\r\n", DeviceConfig.Altitude
            );
    
    for (i = 0; i < sizeof(strMenuAlt) / sizeof(strMenuAlt[0]); i++)
    {
        SendUSBMessage((int8_t*)strMenuAlt[i], -1);
    }
}

/*******************************************************************************
* Nom de la fonction : PeriodeMenuSelection                                    *
* Description : Gère les sélections du menu periode                            *
*******************************************************************************/
void AltitudeMenuSelection(void)
{
    int8_t selection = GetUserSelection();
    switch (selection)
    {
        case '1': MenuState = MENU_ALTITUDE_Value_SET; break;
        case '\e': MenuState = MENU_SET; break;
    }
}

/*******************************************************************************
* Nom de la fonction : DisplayCurrentMeasurements                              *
* Description : Affiche le menu Mesures                                        *
*******************************************************************************/
void DisplayCurrentMeasurements(void)
{
    
    char strMenuMesure[][100] = {
        "\r\n\r\n================================================\r\n",
        "                   Menu mesures                   \r\n",
        "================================================\r\n\r\n",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "            [esc]   Quitter\r\n\r\n",
        "================================================\r\n\r\n"
    };
    
    //remplissage chaines dynamiques
    sprintf((char*)strMenuMesure[3], " Temperature : %.1f C\r\n", (double)BME280.Temperature);
    sprintf((char*)strMenuMesure[4], " Humidite    : %.1f %%\r\n", (double)BME280.Humidite);
    sprintf((char*)strMenuMesure[5], " Pression    : %.1f hPa\r\n", (double)BME280.Pression);
    sprintf((char*)strMenuMesure[6], " Indice prevision Meteo     : %5d\r\n", BME280.Pression_Forecast);
    sprintf((char*)strMenuMesure[7], " Indice qualite d'air       : %5d\r\n", ENS160.Indice_Qualite_Air);
    sprintf((char*)strMenuMesure[8], " Concentration de TVOC      : %5d ppm\r\n", ENS160.Concentration_Co2);
    sprintf((char*)strMenuMesure[9], " Concentration de CO2     : %5d ppb\r\n\r\n", ENS160.Concentration_Ppb);
    
    for ( i = 0; i < sizeof(strMenuMesure) / sizeof(strMenuMesure[0]); i++) 
    {
        SendUSBMessage((int8_t*)strMenuMesure[i], -1);
    }
    
}

/*******************************************************************************
* Nom de la fonction : ExitGet                                                 *
* Description : controle si on veux quiter le menu mesure                      *
*******************************************************************************/
void ExitGet(void)
{
    int8_t selection = GetUserSelection();
    
    if(selection == '\e')MenuState = MENU_SET;
}

/*******************************************************************************
* Nom de la fonction : GestSave                                                *
* Description : recuperation des nouvel configuration et sauvegarde flash      *
*******************************************************************************/
void GestSave(void)
{
    // recuperation des nouvels valeurs
    BME280.Altitude = DeviceConfig.Altitude;
    BME280.EcartHum = DeviceConfig.EcartHum;
    BME280.EcartPress = DeviceConfig.EcartPress;
    BME280.EcartTemp = DeviceConfig.EcartTemp;
    ENS160.EcartQualite = DeviceConfig.EcartQualite;
    strcpy(MQTT.Host, DeviceConfig.MqttHost);
    strcpy(MQTT.Port, DeviceConfig.MqttPort);
    RTC.Periode = DeviceConfig.Period;
    strcpy(Domoticz.Ip, DeviceConfig.TcpIp);
    strcpy(Domoticz.Port, DeviceConfig.TcpPort);
    strcpy(ESP_Wifi.Password, DeviceConfig.WifiPWD);
    strcpy(ESP_Wifi.Ssid, DeviceConfig.WifiSSID);
    
    //saugarde flash
//    Struct_save.Magic = 0x12345678;
//    NVM_WriteBlock((uint32_t *)&Struct_save,sizeof(Struct_save));
}

/*******************************************************************************
* Nom de la fonction : RecupValueInSytm                                        *
* Description : recuperation des valeur du sytheme                             *
*******************************************************************************/
void RecupValueInSytm(void)
{
    // recuperation des nouvels valeurs
    DeviceConfig.Altitude = BME280.Altitude;
    DeviceConfig.EcartHum = BME280.EcartHum;
    DeviceConfig.EcartPress = BME280.EcartPress;
    DeviceConfig.EcartTemp = BME280.EcartTemp;
    DeviceConfig.EcartQualite = ENS160.EcartQualite;
    strcpy(DeviceConfig.MqttHost, MQTT.Host);
    strcpy(DeviceConfig.MqttPort, MQTT.Port);
    DeviceConfig.Period = RTC.Periode;
    strcpy(DeviceConfig.TcpIp, Domoticz.Ip);
    strcpy(DeviceConfig.TcpPort, Domoticz.Port);
    strcpy(DeviceConfig.WifiPWD, ESP_Wifi.Password);
    strcpy( DeviceConfig.WifiSSID, ESP_Wifi.Ssid);
}
/* *****************************************************************************
 End of File
 */
