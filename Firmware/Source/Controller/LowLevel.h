#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "stdinc.h"

// Defines
#define COMMUTATION_TABLE_SIZE 11

#define DAC_CHANNEL_B		BIT15

// Types
typedef enum IChannel
{
	I_CHANNEL_1 = 1,
	I_CHANNEL_2,
	I_CHANNEL_3,
	I_CHANNEL_4,
	I_CHANNEL_5
} IChannel;

typedef enum RelayId
{
	RELAY_LCAU_INPUT_CONTACTOR = 0,
	RELAY_LCAU_HV_OUT,
	RELAY_LCAU_DISCHARGE_DISABLE,
	RELAY_HV_OUT,
	RELAY_SELFTEST1_7MEG,
	RELAY_SELFTEST2_700MEG,
	RELAY_RMES1_NC,
	RELAY_RMES2,
	RELAY_RMES3,
	RELAY_RMES4,
	RELAY_RMES5,
	RELAY_COUNT = COMMUTATION_TABLE_SIZE
} RelayId;

// Variables
//
extern Int32U CycleCounters[COMMUTATION_TABLE_SIZE];

// Functions
//
void LL_ToggleBoardLED();
void LL_ExtIndication(bool State);
void LL_SyncOSC(bool State);
void LL_ToggleExternalLED();
void LL_SetStateRelay(RelayId Id, bool State);
void LL_SetRelaySafeState();
void LL_SPI_WriteByte(uint16_t Data, bool DACChannel);
void LL_WriteDAC(Int16U DataA,Int16U DataB);
void LL_SetCurrentChannel(IChannel Channel);
bool LL_IsSafetyOk();
void LL_ToggleLDAC();
void LL_LCAU_SoftStart(bool State);

#endif //__LOWLEVEL_H
