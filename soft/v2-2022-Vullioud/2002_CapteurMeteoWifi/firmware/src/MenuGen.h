/* 
 * File:   MenuGen.h
 * Author: dierickenbach
 *
 * Created on 4. septembre 2020, 13:24
 */

#ifndef MENUGEN_H
#define	MENUGEN_H

//Enumeration des differents status du menu
typedef enum  { Enter_Menu_Set, Enter_Menu_Get,
                QuitSave_Menu,
                Menu_Set, Menu_Get, 
                Wifi_Set, Wifi_Get,
                    Wifi_Info,
                    Wifi_SSID_Set, Wifi_SSID_Get, 
                    Wifi_PWD_Set, Wifi_PWD_Get,
                Domoticz_Set, Domoticz_Get,
                    Domo_Info,
                    Domo_IP_Set, Domo_IP_Get, 
                    Domo_Port_Set, Domo_Port_Get, 
                    Domo_Index_Set, Domo_Index_Get, 
                Interval_Set, Interval_Get, 
                Measure, 
                Altitude_Set, Altitude_Get  } S_MenuStatus;
S_MenuStatus MenuStatus;

//prototype
void GestMenu (void);

#endif	/* MENUGEN_H */

