#ifndef Mc32Gest_RS232_H
#define Mc32Gest_RS232_H
/*--------------------------------------------------------*/
// Mc32gest_R232.h
/*--------------------------------------------------------*/
//
//	Auteur 		: 	Diego Rickenbach
//
//	Version		:	V1
//
// Mc32Gest_RS232.h
/*--------------------------------------------------------*/

//includes
#include <stdint.h>
#include "GesFifoTh32.h"

//define des status
#define OK      0
#define ERROR   1
#define NO_DATA 2
#define TIMEOUT 3


// prototypes des fonctions
void FifoComm_Init(void);

int8_t GetWifiMessage(int8_t *pDataRX, char* CompareStr, uint32_t nbMsWait);
void SendWifiMessage(int8_t *pDataTX, uint8_t NbByte);
void FlushWifiRxBuf(void);

int8_t GetUSBMessage(int8_t *pDataRX, char* CompareStr);
void SendUSBMessage(int8_t *pDataTX, uint8_t NbByte);
void FlushUSBRxBuf(void);

// Descripteur des fifos
extern S_fifo descrFifoWifiRX;
extern S_fifo descrFifoWifiTX;

extern S_fifo descrFifoUSBRX;
extern S_fifo descrFifoUSBTX;

#endif
