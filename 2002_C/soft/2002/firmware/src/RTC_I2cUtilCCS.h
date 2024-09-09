#ifndef RTC_I2CUTILCCS_H
#define RTC_I2CUTILCCS_H
//--------------------------------------------------------
//	RTC_I2cUtilCCS.h
//--------------------------------------------------------
//	Description :	Adaptation des fonctions I2C afin
//			de pouvoir utiliser la même syntaxe 
//			que sous CCS.
//
//	Auteur 		: 	C. HUBER
//      Date            :       22.05.2014
//	Version		:	V1.5
//	Compilateur	:	XC32 V1.33 & Harmony V1.00
// Modifications :
//      CHR 19.03.2015  Migration sur plib_i2c de Harmony 1.00   CHR
/*--------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>


//------------------------------------------------------------------------------
// i2c_init
//
// Initialisationde l'I2c
//   si bool Fast = false   LOW speed
//   si bool Fast = true    HIGH speed
//------------------------------------------------------------------------------

void RTC_i2c_init( bool Fast );
void RTC_i2c_start(void);
void RTC_i2c_reStart(void);
bool RTC_i2c_write( uint8_t data );

//------------------------------------------------------------------------------
// i2c_read()
//
// Syntaxe : 	data = i2c_read (ack);
// La fonction retourne l'octet lu.
// Le paramètre ack :
// 1 signifie qu'il faut effectuer l'acquittement.
// 0 signifie qu'il ne faut pas effectuer l'acquittement.
//
// Remarque : réalisée à partir du code de l'exemple
//

uint8_t RTC_i2c_read(bool ackTodo);

void RTC_i2c_stop( void );
#endif
