/*******************************************************************************
*                                                                              *
* Auteur : Joao Marques                                                        *                     
* Fichier : Gest_Menu.h                                                        *
*                                                                              *
* Description : Déclarations des fonctions et données pour la gestion du menu  *
*                                                                              *
*******************************************************************************/

#ifndef GEST_MENU_H    
#define GEST_MENU_H

/******************************************************************************/
/* Section : Fichiers Inclus                                                  */
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//*****************************************************************************/
// Section : Définitions                                                      */
//*****************************************************************************/


//*****************************************************************************/
// Section : Données                                                          */
//*****************************************************************************/

// Enumeration des differents statuts du menu
typedef enum  
{ 
    MENU_ENTER_SET = 0, 
    MENU_ENTER_GET,
    MENU_SAVE, 
    MENU_EXIT,
    MENU_SET, 
    MENU_GET, 
    MENU_WIFI_SET, 
    MENU_WIFI_GET,
    MENU_WIFI_DISP,
    MENU_WIFI_SSID_SET, 
    MENU_WIFI_SSID_GET, 
    MENU_WIFI_PWD_SET, 
    MENU_WIFI_PWD_GET,
    MENU_MQTT_SET, 
    MENU_MQTT_GET,
    MENU_MQTT_DISP,
    MENU_MQTT_IP_SET, 
    MENU_MQTT_IP_GET, 
    MENU_MQTT_PORT_SET, 
    MENU_MQTT_PORT_GET, 
    MENU_TCP_SET, 
    MENU_TCP_GET,
    MENU_TCP_DISP,
    MENU_TCP_IP_SET, 
    MENU_TCP_IP_GET, 
    MENU_TCP_PORT_SET, 
    MENU_TCP_PORT_GET,
    MENU_ECART_SET, 
    MENU_ECART_GET,
    MENU_ECART_DISP,
    MENU_ECART_TEMP_SET, 
    MENU_ECART_TEMP_GET,
    MENU_ECART_HUM_SET, 
    MENU_ECART_HUM_GET,
    MENU_ECART_PRESS_SET, 
    MENU_ECART_PRESS_GET,
    MENU_ECART_QUALITE_SET, 
    MENU_ECART_QUALITE_GET,
    MENU_PERIOD_SET, 
    MENU_PERIOD_GET,   
    MENU_PERIOD_Value_SET, 
    MENU_PERIOD_Value_GET, 
    MENU_ALTITUDE_SET, 
    MENU_ALTITUDE_GET,
    MENU_ALTITUDE_Value_SET,
    MENU_ALTITUDE_Value_GET,
    MENU_MEASURE_DISP,
    MENU_MEASURE_GET,
} S_MenuStatus;

// Déclaration d'une variable globale de type S_MenuStatus                                
S_MenuStatus MenuState;


typedef struct DeviceConfig 
{
    char WifiSSID[100];     // SSID du réseau Wi-Fi
    char WifiPWD[100];      // Mot de passe du réseau Wi-Fi
    char MqttHost[100];      // Hôte du serveur MQTT
    char MqttPort[100];       // Port du serveur MQTT
    char TcpIp[100];        // Adresse IP de Domoticz
    char TcpPort[100];       // Port de Domoticz
    uint32_t Period;        // Intervalle de mesure [s]     
    uint32_t Altitude;      // Altitude de mesure [m]
    uint32_t EcartTemp;     // ecart pour limite max et min tempearature
    uint32_t EcartHum;      // ecart pour limite max et min humidite
    uint32_t EcartPress;    // ecart pour limite max et min pression
    uint32_t EcartQualite;  // ecart pour limite max et min qualite d'aire
} S_DeviceConfig;

// Déclaration d'une variable globale de type S_DeviceConfig
S_DeviceConfig DeviceConfig;


//*****************************************************************************/
// Section : Fonctions                                                        */
//*****************************************************************************/
void Menu_init(void);

void State_Menu(void);

void DisplayMainMenu(void);

void MainMenuSelection(void);

void MainMenuSelection(void);

void DisplayWifiSettings(void);

void WifiMenuSelection(void);

void DisplayMqttSettings(void);

void SSIDInput(void);

void PasswordInput(void);

void MqttMenuSelection(void);

void MenuInput(char *configField, int8_t State);

uint32_t ValueInput(uint32_t oldvalue ,int8_t State);

void DisplayDomoSettings(void);

void DomoMenuSelection(void);

void DisplayEcartSettings(void);

void EcartMenuSelection(void);

void DisplayPeriodSettings(void);

void PeriodeMenuSelection(void);

void DisplayAltitudeSettings(void);

void AltitudeMenuSelection(void);

void DisplayCurrentMeasurements(void);

void ExitGet(void);

void GestSave(void);

void RecupValueInSytm(void);

#endif /* GEST_MENU_H */

/* *****************************************************************************
 Fin du fichier
 */