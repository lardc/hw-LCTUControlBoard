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
void DBGACT_GenerateOscSync()
{
	LL_OscSyncSetState(true);
	DELAY_MS(100);
	LL_OscSyncSetState(false);
}
//-----------------------------

void DBGACT_GeneratePAUSync()
{
	LL_PAUSyncSetState(true);
	DELAY_MS(100);
	LL_PAUSyncSetState(false);
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

void DBGACT_VoltageRangeSet(bool Range)
{
	LL_VoltageRangeSet(Range);
}
//-----------------------------

void DBGACT_PAU_Shunting(bool State)
{
	LL_PAU_Shunting(State);
}
//-----------------------------

void DBGACT_SelfTestCommutationControl(bool State)
{
	LL_SelfTestCommutationControl(State);
}
//-----------------------------

void DBGACT_OutputCommutationControl(bool State)
{
	LL_OutputCommutationControl(State);
}
//-----------------------------

void DBGACT_FanControl(bool State)
{
	LL_FanControl(State);
}
//-----------------------------

void DBGACT_ExtIndicationControl(bool State)
{
	LL_ExtIndicationControl(State);
}
//-----------------------------
