/******************************************************************************* 
*                                                                              * 
* Auteur : Joao Marques                                                        *                     
* Fichier : ESP32.h                                                            *
*                                                                              *
*    Ce fichier d'en-tête définit les structures et les fonctions utilisées    *
*    pour configurer et interagir avec le module ESP32 via des commandes AT.   *
*                                                                              *
*******************************************************************************/

#ifndef ESP32_H    
#define ESP32_H

/******************************************************************************/
/* Section : Fichiers Inclus                                                  */
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/* Section : Defines                                                          */
/******************************************************************************/

// Définir les modes
#define ModeStation 0x31         //'1'
#define ModeSoftAP 0x32          //'2'
#define ModeStationSoftAP 0x33   //'3'

//******************************************************************************
// Section : Données
//******************************************************************************

// Structure pour les paramètres Wi-Fi
typedef struct ESP_Wifi {
   char Ssid[100];      // SSID du réseau Wi-Fi
   char Password[100];  // Mot de passe du réseau Wi-Fi
   uint32_t Magic;      // Valeur de vérification
} S_ESP_WIFI;
// Déclaration d'une variable globale de type S_WIFI
S_ESP_WIFI ESP_Wifi;     

// Structure pour l'ESP32
typedef struct ESP32 {
   char IP[15];         // Adresse IP de l'ESP32
   char MAC[17];        // Adresse MAC de l'ESP32
   bool Status;         // Statut de l'ESP32
   uint32_t Magic;      // Valeur de vérification
} S_ESP32;
// Déclaration d'une variable globale de type S_ESP32
S_ESP32 ESP32;   

// Structure pour les paramètres MQTT
typedef struct MQTT {
    char Host[100];      // Hôte du serveur MQTT
    char Port[100];      // Port du serveur MQTT
    char Username[100];  // Nom d'utilisateur MQTT
    char Password[100];  // Mot de passe MQTT
    char Id_client[100]; // ID du client MQTT
    char Topic[100];     // Sujet du message MQTT
    char Qos[100];       // Qualité de service du message MQTT
    char Retain[100];    // Indicateur de conservation du message MQTT
    bool Status;
    uint32_t Magic;      // Valeur de vérification
} S_MQTT;
// Déclaration d'une variable globale de type S_MQTT
S_MQTT MQTT;      

// Structure pour les paramètres Domoticz
typedef struct Domoticz { 
   char Ip[16];         // Adresse IP de Domoticz
   char Port[6];        // Port de Domoticz
   char Idx[6];         // Index de Domoticz
   uint32_t Magic;      // Valeur de vérification
} S_Domoticz;
// Déclaration d'une variable globale de type S_Domoticz
S_Domoticz Domoticz;

// *****************************************************************************
// Section : Fonctions
// *****************************************************************************

// Fonction d'initialisation
bool ESP32_Init(void);

// Fonction pour envoyer une commande AT
void SendATCommand(const char* command);

// Fonction pour extraire les adresses IP et MAC
uint8_t ReadIpAndMac(char* inputStr, char* IpStr, char* MacStr);

// Fonction pour vérifier si l'ESP32 est démarré
uint8_t ESP32_isStarted(void);

// Fonction pour se connecter au Wi-Fi
uint8_t ESP32_ConnectWifi(char *SSID, char *PWD);

// Fonction pour changer le mode de l'ESP32
uint8_t ESP32_ChangeMode(uint8_t mode);

// Fonction pour récupérer l'adresse IP et MAC de l'ESP32
uint8_t ESP32_GetIpAndMac(char* StrIP, char* StrMAC);

// Fonction pour publier un message MQTT
void PublishMQTT(const char* topic, double value);

// Fonction pour configurer MQTT et publier des données
void SetupMQTTAndPublish(float temp_ext, float taux_humidite, float pression_air
                        , uint8_t indice_qualite_air, uint16_t concentration_CO2
                        );

uint8_t ESP32_ConnectToTCPServer(char *StrIP, char *StrPort);

uint8_t ESP32_DisconnectTCPServer(void);

uint8_t ESP32_SendTCP(void);

#endif /* ESP32_H */

/* *****************************************************************************
 Fin du fichier
 */