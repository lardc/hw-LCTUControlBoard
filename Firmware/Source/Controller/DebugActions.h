#ifndef __DEBUGACTIONS_H
#define __DEBUGACTIONS_H

// Include
//
#include "ZwBase.h"

// Functions
//
void DBGACT_GenerateImpulseToLineSync();
void DBGACT_OpAmpSetVoltage(uint16_t Data);
void DBGACT_PowerSupplyControl(bool State);

#endif //__DEBUGACTIONS_H
