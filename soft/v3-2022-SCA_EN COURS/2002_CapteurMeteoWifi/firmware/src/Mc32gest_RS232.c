/*--------------------------------------------------------*/
// Mc32gest_R232.c
/*--------------------------------------------------------*/
//
//	Auteur 		: 	Diego Rickenbach
//
//	Version		:	V1
//
// Mc32Gest_RS232.c
/*--------------------------------------------------------*/

//Includes
#include <xc.h>
#include <sys/attribs.h>
#include "system_definitions.h"
#include <string.h>

#include <GenericTypeDefs.h>
#include "app.h"
#include "GesFifoTh32.h"
#include "Mc32gest_RS232.h"
#include "Mc32Delays.h"


typedef union {
        uint16_t val;
        struct {uint8_t lsb;
                uint8_t msb;} shl;
} U_manip16;


// Definition pour les messages
//#define MESS_SIZE  30

// Declaration des FIFO pour r�ception et �mission
//#define FIFO_RX_SIZE ((4*MESS_SIZE) + 1)  // 4 messages
//#define FIFO_TX_SIZE ((4*MESS_SIZE) + 1)  // 4 messages
#define FIFO_WIFI_RX_SIZE 500
#define FIFO_WIFI_TX_SIZE 500

#define FIFO_USB_RX_SIZE 500
#define FIFO_USB_TX_SIZE 500

int8_t fifoWifiRX[FIFO_WIFI_RX_SIZE];
int8_t fifoUSBRX[FIFO_USB_RX_SIZE];

// Declaration du descripteur du FIFO de r�ception
S_fifo descrFifoWifiRX;
S_fifo descrFifoUSBRX;

int8_t fifoWifiTX[FIFO_WIFI_TX_SIZE];
int8_t fifoUSBTX[FIFO_USB_TX_SIZE];

// Declaration du descripteur du FIFO d'�mission
S_fifo descrFifoWifiTX;
S_fifo descrFifoUSBTX;

// Initialisation de la communication s�rielle
void FifoComm_Init(void)
{    
    // Initialisation du fifo de r�ception Wifi
    InitFifo (&descrFifoWifiRX, FIFO_WIFI_RX_SIZE, fifoWifiRX, 0);
    // Initialisation du fifo d'�mission
    InitFifo (&descrFifoWifiTX, FIFO_WIFI_TX_SIZE, fifoWifiTX, 0);
    
    // Initialisation du fifo de r�ception USB
    InitFifo (&descrFifoUSBRX, FIFO_USB_RX_SIZE, fifoUSBRX, 0);
    // Initialisation du fifo d'�mission
    InitFifo (&descrFifoUSBTX, FIFO_USB_TX_SIZE, fifoUSBTX, 0);
   
} // InitComm

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     FlushWifiRxBuf                                      *                                    
* Entr�e :      rien                                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de supprimer tout ce qu'il y a dans le   *                                                     
*               Fifo SW                                                        *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
void FlushWifiRxBuf(void)
{
    int8_t c;
    while (GetReadSize(&descrFifoWifiRX) > 0)
        GetCharFromFifo(&descrFifoWifiRX, &c);        
}

void FlushUSBRxBuf(void)
{
    int8_t c;
    while (GetReadSize(&descrFifoUSBRX) > 0)
        GetCharFromFifo(&descrFifoUSBRX, &c);        
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     GetWifiMessage                                      *                                    
* Entr�e :      int8_t *pDataRX, char* CompareStr, uint32_t nbMsWait           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de r�cup�rer tous les caract�res se      *
*               trouvant de la Fifo SW du Wifi.                                *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
int8_t GetWifiMessage(int8_t *pDataRX, char* CompareStr, uint32_t nbMsWait)
{
	//D�claration des variables
    uint32_t nbMs = 0;
    int32_t NbCharToRead = 0;
    uint8_t charCntr = 0;
    uint8_t i;
    
    //tant que le timeout n'est pas termin�
    do{
        //v�rifie le nombre de caract�re dans le fifoSW
        NbCharToRead = GetReadSize(&descrFifoWifiRX);

        //test si il y en a plus que 0
        if(NbCharToRead > 0)
        {  
           for(i=0; i < NbCharToRead; i++)
           {
               //r�cup�re les charact�res du fifoSW
               GetCharFromFifo(&descrFifoWifiRX, (int8_t*)&pDataRX[charCntr]);
               charCntr++;
           }
           
           //test si dans la trame re�ue, il y a un \r\n
           if(strstr((char*)pDataRX, "\x0D\x0A") != 0)
           {
               //si oui, teste si dans la trame il y a les m�me caract�re que l'on veut
               if (strstr((char*)pDataRX, CompareStr) != 0)
               {
                    //si oui, on supprime tout ce qu'il y a dans le fifoSW et return OK
                    FlushWifiRxBuf();
                    return OK;
               }
               //si oui, teste sur le message ERROR a �t� re�u 
               if (strstr((char*)pDataRX, "ERROR") != 0)
               {
                    //si oui, on supprime tout ce qu'il y a dans le fifoSW et return ERROR
                    FlushWifiRxBuf();
                    return ERROR;
               }         
           }
        }
        delay_msCt(1);
        nbMs++;
    }while(nbMs <= nbMsWait);
    
    //test si on a r�cup�r� des caracteres dans le FifoSW
    if (charCntr > 0)
    {
        //si oui, on supprime tout ce qu'il y a dans le fifoSW et return TIMEOUT
        FlushWifiRxBuf();
        return TIMEOUT;
    }
    //sinon
    else
        //return NO_DATA
        return NO_DATA;    
} // GetWifiMessage

/*******************************************************************************
* Auteur : SCA                                                                 *                     
* Nom de la fonction :     CheckSingleCharFromUsb                              *                                    
* Entr�e :      char expectedChar                                              *                                                                      
* Sortie :      code de retour OK/ERROR/NO_DATA/TIMEOUT                        *
* Desription :  Contr�le dans le buffer de r�ception si le caract�re voulu     *
*               est pr�sent                                                    *                              *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
int8_t ReadSingleCharFromUsb(char expectedChar)
{
    static uint8_t rxBufLvl = 0;
    static char rxBuf[200];
    uint8_t NbCharToRead;
    uint8_t i;
    
     //v�rifie le nombre de caract�re dans le fifoSW
    NbCharToRead = GetReadSize(&descrFifoUSBRX);   
    
    //stocke les nouveaux car. dans buffer local
    for(i=0 ; i<NbCharToRead ; i++)
    {
        GetCharFromFifo(&descrFifoUSBRX, (int8_t*)&rxBuf[rxBufLvl]);
        rxBufLvl++;
    }
    
    if(rxBufLvl > 0)
    {
        //cherche caract�re dans buffer local
        rxBuf[rxBufLvl] = 0;    //place fin d echaine
        if (strchr(rxBuf, expectedChar) != NULL)
        {
            rxBufLvl = 0;
            return (OK);    //trouv�
        }
        else
            return (NOT_FOUND); //pas trouv�
    }
    else
        return (NO_DATA);   //buffer vide   
}

/*******************************************************************************
* Auteur : SCA                                                                 *                     
* Nom de la fonction :     ReadStrFromUsb                              *                                    
* Entr�e :      char expectedChar                                              *                                                                      
* Sortie :      code de retour OK/ERROR/NO_DATA/TIMEOUT                        *
* Desription :  Contr�le dans le buffer de r�ception si le caract�re voulu     *
*               est pr�sent                                                    *                              *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
int8_t ReadStrFromUsb(char* pStr)
{
    static uint8_t rxBufLvl = 0;
    static char rxBuf[200];
    uint8_t NbCharToRead;
    uint8_t i;
    
     //v�rifie le nombre de caract�re dans le fifoSW
    NbCharToRead = GetReadSize(&descrFifoUSBRX);   
    
    //stocke les nouveaux car. dans buffer local
    for(i=0 ; i<NbCharToRead ; i++)
    {
        GetCharFromFifo(&descrFifoUSBRX, (int8_t*)&rxBuf[rxBufLvl]);
        rxBufLvl++;
    }
    
    if(rxBufLvl > 0)
    {
        //cherche string dans buffer local
        rxBuf[rxBufLvl] = 0;    //place fin de chaine
        if ((strchr(rxBuf, '\r') != NULL) || (strchr(rxBuf, '\n') != NULL)) //touche 'enter' ?
        {            
            strcpy(pStr, rxBuf);   //copie chaine
            rxBufLvl = 0;
            return (OK);    //trouv�
        }
        else if (strchr(rxBuf, '\e') != NULL) //'esc' ?
        {
            rxBufLvl = 0;
            return (ESCAPE); //annulation
        }
        return (NO_DATA);   //ni valid� par enter, ni escape ==> pas de data � analyser
    }
    else
        return (NO_DATA);   //buffer vide      
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     GetUSBMessage                                       *                                    
* Entr�e :      int8_t *pDataRX, char* CompareStr                              *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de r�cup�rer tout les caract�res se      *
*               trouvant de la Fifo SW de l'USB.                               *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
//int8_t GetUSBMessage(int8_t *pDataRX, char* CompareStr)
//{
//	//D�claration des variables
//    int32_t NbCharToRead = 0;
//    uint8_t charCntr = 0;
//    uint8_t i, j;
//    static int8_t OldCharFromFifo[500] = {0};
//    
//    //v�rifie le nombre de caract�re dans le fifoSW
//    NbCharToRead = GetReadSize(&descrFifoUSBRX);
//
//    //test si il y en a plus que 0
//    if(NbCharToRead > 0)   
//    {  
//        //r�cup�re les charact�res du fifoSW et en fait une copie dans 
//        //OldCharFromFifo
//        for(i = 0; i < NbCharToRead; i++)
//        {
//            GetCharFromFifo(&descrFifoUSBRX, (int8_t*)&pDataRX[charCntr]);
//            OldCharFromFifo[charCntr] = pDataRX[charCntr];
//            charCntr++;
//        }
//        //test si dans la trame il y a les m�me caract�re que l'on veut
//        if (strstr((char*)pDataRX, CompareStr) != 0)
//        {
//            //si oui, on supprime tout ce qu'il y a dans le fifo SW,
//            //supprime ce qu'il y a dans le tableau et return OK
//            for(j = 0; j < strlen((char*)OldCharFromFifo); j++)
//            {
//                OldCharFromFifo[j] = 0;
//            }
//            FlushUSBRxBuf();
//            return OK;
//        }        
//    }
//    //sinon, test si dans l'ancienne trame il y a les m�me caract�re que l'on veut 
//    else if (strstr((char*)OldCharFromFifo, CompareStr) != 0)
//    {
//        //si oui, on supprime tout ce qu'il y a dans le tableau et retourne OK
//        uint8_t charCntr = strlen((char*)OldCharFromFifo);
//        for(j = 0; j < charCntr-1; j++)
//        {
//            pDataRX[j] = OldCharFromFifo[j];
//            OldCharFromFifo[j] = 0;
//        } 
//        return OK;
//    }
//    else
//    {
//       //retourne NO_DATA
//        return NO_DATA;
//    }
//    //retourne ERROR
//    return ERROR;
//} // GetUSBMessage

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     SendWifiMessage                                     *                                    
* Entr�e :      int8_t *pDataRX, char* CompareStr                              *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de mettre les caract�re dans le fifo SW  *
*               du Wifi.                                                       *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
void SendWifiMessage(int8_t *pDataTX, uint8_t NbByte)
{
//    uint8_t txFifoLvl;			// Niveau de remplissage du fifo
    uint8_t i;
    
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT);
    
    //remplit buffer tx
    for(i = 0; i < NbByte; i++)
    {
        //r�cup�re les characteres et les met dans le fifoSW
        PutCharInFifo(&descrFifoWifiTX, pDataTX[i]);
    }
    
    //v�rifie le nombre de caract�re dans le fifoSW
//    txFifoLvl = GetReadSize(&descrFifoWifiTX);
    
    //test si il y a plus que 0 charactere dans le fifo
    if (GetReadSize(&descrFifoWifiTX) > 0)
    {
        // Active interruption   
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT);
    }
    FlushWifiRxBuf();
}

/*******************************************************************************
* Auteur : Diego Rickenbach                                                    *                     
* Nom de la fonction :     SendUSBMessage                                      *                                    
* Entr�e :  int8_t *pDataTX donnees a envoyer                                  *
*           int8_t NbByte taille des donnees. Si < 0, envoie jusqu'a           *
*             rencontrer caractere 0                                           *                                                                      
* Sortie :      rien                                                           *
* Desription :  Cette fonction permet de mettre les caract�res dans le fifo SW *
*               de l'USB.                                                      *
* Remarques:    Voir le prototype dans Mc32gest_RS232.h                        *
*******************************************************************************/
void SendUSBMessage(const int8_t *pDataTX, int8_t nbByte)
{
//    uint32_t txFifoLvl;			// Niveau de remplissage du fifo
    uint8_t i;
    
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    
    //remplit buffer tx
    if (nbByte < 0)
    {
        i=0;
        while (pDataTX[i] != 0)
        {
            PutCharInFifo(&descrFifoUSBTX, pDataTX[i]);
            i++;
        }
    }
    else
    {
        for(i = 0; i < nbByte; i++)
        {
            //r�cup�re les characteres et les met dans le fifoSW
            PutCharInFifo(&descrFifoUSBTX, pDataTX[i]);
        }
    }
    
    //test si il y a plus que 0 charactere dans le fifo
//    txFifoLvl = GetReadSize(&descrFifoUSBTX);
    if (GetReadSize(&descrFifoUSBTX) > 0)
    {
        // Active interruption   
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
//    FlushUSBRxBuf();
}

// Interruption USART2 - Wifi
void __ISR(_UART_2_VECTOR, ipl3AUTO) _IntHandlerDrvUsartInstance0(void)
{
//    static uint8_t i = 0;
    int8_t TXchar;
    int8_t c = 0;
    USART_ERROR UsartStatus;   
    
     // Is this an Error interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_ERROR) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_2_ERROR) ) 
    {
        /* Clear pending interrupt */
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_ERROR);
        // Traitement de l'erreur � la r�ception.
    } 

    // Is this an RX interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_RECEIVE) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_2_RECEIVE))
    {

        // Oui Test si erreur parit� ou overrun
        UsartStatus = PLIB_USART_ErrorsGet(USART_ID_2);

        if ((UsartStatus & (USART_ERROR_PARITY | USART_ERROR_FRAMING |  
                USART_ERROR_RECEIVER_OVERRUN)) == 0) 
        {

            // Lecture des caract�res depuis le buffer HW -> fifo SW
			//  (pour savoir s'il y a une data dans le buffer HW RX : 
            // PLIB_USART_ReceiverDataIsAvailable())
			//  (Lecture via fonction PLIB_USART_ReceiverByteReceive())
            
			//Tant qu'il des donn�es dans le Fifo HW, on les mets dans le FifoSW
            while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_2))
            {
                c = PLIB_USART_ReceiverByteReceive(USART_ID_2);
                PutCharInFifo(&descrFifoWifiRX, c);
            }
            // buffer is empty, clear interrupt flag
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
        } 
        else 
        {
            // Suppression des erreurs
            // La lecture des erreurs les efface sauf pour overrun
            if ( (UsartStatus & USART_ERROR_RECEIVER_OVERRUN) 
                    == USART_ERROR_RECEIVER_OVERRUN) 
            {
                   PLIB_USART_ReceiverOverrunErrorClear(USART_ID_2);
            }
        }
    } // end if RX

    
     // Is this an TX interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT) 
            && PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT)) 
    {            
        // Avant d'�mettre, on v�rifie 2 conditions :
        //  S'il y a un caract�res � �mettre dans le fifo
        //  S'il y a de la place dans le buffer d'�mission 
        // (PLIB_USART_TransmitterBufferIsFull)
        //   (envoi avec PLIB_USART_TransmitterByteSend())
         while((GetReadSize(&descrFifoWifiTX) > 0) && 
                 (PLIB_USART_TransmitterBufferIsFull(USART_ID_2) == false))
        {
            GetCharFromFifo(&descrFifoWifiTX, &TXchar);
            PLIB_USART_TransmitterByteSend(USART_ID_2, TXchar);
//            i++;
        }
            // disable TX interrupt (pour �viter une interrupt. inutile si 
            // plus rien � transmettre)
        if (GetReadSize(&descrFifoWifiTX) == 0)
            PLIB_INT_SourceDisable(INT_ID_0,INT_SOURCE_USART_2_TRANSMIT);
            
        // Clear the TX interrupt Flag (Seulement apres TX) 
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT);
    }// end if TX
}

// Interruption USART1 - USB
void __ISR(_UART_1_VECTOR, ipl3AUTO) _IntHandlerDrvUsartInstance1(void)
{
//    static uint8_t i = 0;
    int8_t TXchar;
    int8_t c = 0;
    USART_ERROR UsartStatus;   
    
     // Is this an Error interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_ERROR) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_ERROR) ) 
    {
        // Clear pending interrupt 
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
        // Traitement de l'erreur � la r�ception.
    }
   

    // Is this an RX interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_RECEIVE))
    {

        // Oui Test si erreur parit� ou overrun
        UsartStatus = PLIB_USART_ErrorsGet(USART_ID_1);

        if ((UsartStatus & (USART_ERROR_PARITY | USART_ERROR_FRAMING |  
                USART_ERROR_RECEIVER_OVERRUN)) == 0) 
        {
            // Lecture des caract�res depuis le buffer HW -> fifo SW
			//  (pour savoir s'il y a une data dans le buffer HW RX : 
            // PLIB_USART_ReceiverDataIsAvailable())
			//  (Lecture via fonction PLIB_USART_ReceiverByteReceive())
            
			//Tant qu'il des donn�es dans le Fifo HW, on les mets dans le FifoSW
            while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1))
            {
                c = PLIB_USART_ReceiverByteReceive(USART_ID_1);
                PutCharInFifo(&descrFifoUSBRX, c);
            }
            // buffer is empty, clear interrupt flag
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        } 
        else 
        {
            // Suppression des erreurs
            // La lecture des erreurs les efface sauf pour overrun
            if ( (UsartStatus & USART_ERROR_RECEIVER_OVERRUN) 
                    == USART_ERROR_RECEIVER_OVERRUN) 
            {
                   PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
            }
        }
    } // end if RX

    
     // Is this an TX interrupt ?
    if ( PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) 
            && PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT)) 
    {
        // Avant d'�mettre, on v�rifie 2 conditions :
        //  S'il y a un caract�res � �mettre dans le fifo
        //  S'il y a de la place dans le buffer d'�mission 
        // (PLIB_USART_TransmitterBufferIsFull)
        //   (envoi avec PLIB_USART_TransmitterByteSend())
         while((GetReadSize(&descrFifoUSBTX) > 0) && 
                 (!PLIB_USART_TransmitterBufferIsFull(USART_ID_1)))
        {
            GetCharFromFifo(&descrFifoUSBTX, &TXchar);
            PLIB_USART_TransmitterByteSend(USART_ID_1, TXchar);
//            i++;
        }
        // disable TX interrupt (pour �viter une interrupt. inutile si 
        // plus rien � transmettre)
        if (GetReadSize(&descrFifoUSBTX) == 0)
            PLIB_INT_SourceDisable(INT_ID_0,INT_SOURCE_USART_1_TRANSMIT);
            
        // Clear the TX interrupt Flag (Seulement apres TX) 
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        
    }// end if TX
}



