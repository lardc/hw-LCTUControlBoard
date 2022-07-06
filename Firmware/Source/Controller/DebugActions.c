// Header
#include "DebugActions.h"

// Include
//
#include "LowLevel.h"
#include "Board.h"
#include "Delay.h"
#include "Controller.h"
#include "DataTable.h"

// Functions
//
void DBGACT_GenerateImpulseToLineSync()
{
	LL_SetStateLineSync(true);
	CONTROL_DelayMs(100);
	LL_SetStateLineSync(false);
}
//-----------------------------

void DBGACT_OpAmpSetVoltage(uint16_t Data)
{
	LL_WriteDACx(Data | DAC_CHANNEL_B);
}
//-----------------------------

void DBGACT_PowerSupplyControl(bool State)
{
	LL_PowerSupply(State);
}
//-----------------------------
