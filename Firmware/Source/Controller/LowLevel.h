#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "stdinc.h"

// Defines
#define COMMUTATION_TABLE_SIZE 7
#define RELAY_ALL_CHANNELS (RELAY_CH_0|RELAY_CH_1|RELAY_CH_2|RELAY_CH_3|RELAY_CH_4)

// Types
typedef enum IChannel
{
	I_CHANNEL_0 = 1,
	I_CHANNEL_1,
	I_CHANNEL_2,
	I_CHANNEL_3,
	I_CHANNEL_4
} IChannel;

typedef enum RelayMask
{
	RELAY_CH_0 			= 0,
	RELAY_CH_1 			= BIT0 | BIT4,
	RELAY_CH_2 			= BIT0 | BIT5,
	RELAY_CH_3 			= BIT0 | BIT6,
	RELAY_CH_4 			= BIT0 | BIT7,
	RELAY_TEST_LOAD		= BIT9,
	RELAY_NEG_POLARITY	= BIT8
} RelayMask;

// Variables
//
extern Int32U CycleCounters[COMMUTATION_TABLE_SIZE];

// Functions
//
void LL_ToggleBoardLED();
void LL_ExtIndication(bool State);
void LL_Sync(bool State);
void LL_ToggleExternalLED();
void LL_SPI_WriteByte(Int16U Data);
void LL_SPI_SetStateOE(bool State);
void LL_WriteDAC(Int16U Data);
void LL_SetCurrentChannel(IChannel Channel);
bool LL_SafetyState();
void LL_SetNegativePolarity(bool State);
void LL_SetSelfTestLoad(bool State);

#endif //__LOWLEVEL_H
