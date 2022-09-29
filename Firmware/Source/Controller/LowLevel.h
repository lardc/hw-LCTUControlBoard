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
void LL_OscSyncSetState(bool State);
void LL_PAUSyncSetState(bool State);
void LL_VoltageRangeSet(bool Range);
void LL_PAU_Shunting(bool State);
void LL_SelfTestCommutationControl(bool State);
void LL_OutputCommutationControl(bool State);
void LL_FanControl(bool State);
void LL_ExtIndicationControl(bool State);
void LL_ToggleExtIndication();
void LL_WriteDACx(Int16U Data);
void LL_ToggleLDAC();
bool LL_SafetyGetState();
void LL_PAUSyncFlip();

#endif //__LOWLEVEL_H
