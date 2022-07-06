#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

// Include
#include "Board.h"
#include "stdinc.h"

// Defines
#define DAC_CHANNEL_B		BIT15

// Functions
//
void LL_ToggleBoardLED();
void LL_PowerSupply(bool State);
void LL_SetStateLineSync(bool State);
void LL_WriteDACx(Int16U Data);
void LL_ToggleLDAC();

#endif //__LOWLEVEL_H
