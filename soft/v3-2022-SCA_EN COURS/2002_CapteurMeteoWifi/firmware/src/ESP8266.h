/*--------------------------------------------------------*/
// Mc32GestSensor.h
/*--------------------------------------------------------*/
//	Description :	Gestion de l'ESP8266
//	Auteur 		: 	D. Rickenbach
//	Version		:	V0.5    21.08.2020
//	Compilateur	:	XC32 V5.15 & Harmony 2_06
//
/*--------------------------------------------------------*/

#ifndef ESP8266_H
#define	ESP8266_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// define Mode 
#define ModeStation 0x31    //'1'
#define ModeSoftAP 0x32     //'2'
#define ModeStationSoftAP 0x33  //'3'

// Structure de ESP8266
typedef struct ESP8266 {
   char IP[15];     // Adress IP de l'ESP8266
   char MAC[17];    // adresse MAC
   bool Status;
   uint32_t Magic;
} S_ESP8266;
S_ESP8266 ESP8266;

//Structure du Wifi
typedef struct Wifi {
   char Ssid[100];   // Adress IP de l'ESP8266
   char Pwd[100];   // pointeur sur début du fifo
   char DomoIp[16];
   char DomoPort[6];
   char DomoIdx[6];
   uint32_t Magic;
} S_WIFI;
S_WIFI Wifi;

//Prtotoype des fonctions
bool ESP8266_Init(void);
uint8_t ESP8266_isStarted(void);
uint8_t ESP8266_Reset(void);
uint8_t ESP8266_ChangeMode(uint8_t);
uint8_t ESP8266_ConnectWifi(char *SSID, char *PWD);
//uint8_t getIpAndMac(char* inputStr, char* IpStr, char* MacStr);
uint8_t ESP8266_GetIpAndMac(char* StrIP, char* StrMAC);
uint8_t ESP8266_ConnectToTCPServer(char *IP, char *Port);
uint8_t ESP8266_DisconnectTCPServer(void);
uint8_t SendDomoticz(/*uint8_t Index, */float Temp, float Hum, uint8_t Hum_, float Press, uint8_t Press_);

#endif	/* ESP8266_H */

