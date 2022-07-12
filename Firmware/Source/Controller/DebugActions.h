#ifndef __DEBUGACTIONS_H
#define __DEBUGACTIONS_H

// Include
//
#include "ZwBase.h"

// Functions
//
void DBGACT_GenerateOscSync();
void DBGACT_GeneratePAUSync();
void DBGACT_OpAmpSetVoltage(uint16_t Data);
void DBGACT_PowerSupplyControl(bool State);
void DBGACT_VoltageRangeSet(bool Range);
void DBGACT_PAU_Shunting(bool State);
void DBGACT_SelfTestCommutationControl(bool State);
void DBGACT_OutputCommutationControl(bool State);
void DBGACT_FanControl(bool State);
void DBGACT_ExtIndicationControl(bool State);

#endif //__DEBUGACTIONS_H
