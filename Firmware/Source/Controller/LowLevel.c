// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "Global.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_PowerSupply(bool State)
{
	GPIO_SetState(GPIO_PS_CTRL, State);
}
//-----------------------------

bool LL_SafetyGetState()
{
	return GPIO_GetState(GPIO_SAFETY);
}
//-----------------------------

void LL_OscSyncSetState(bool State)
{
	GPIO_SetState(GPIO_OSC_SYNC, State);
}
//-----------------------------

void LL_PAUSyncSetState(bool State)
{
	GPIO_SetState(GPIO_PAU_SO, State);
}
//-----------------------------

void LL_VoltageRangeSet(bool Range)
{
	GPIO_SetState(GPIO_U_RANGE, Range);
}
//-----------------------------

void LL_PAU_Shunting(bool State)
{
	GPIO_SetState(GPIO_PAU_SHUNT, State);
}
//-----------------------------

void LL_SelfTestCommutationControl(bool State)
{
	GPIO_SetState(GPIO_COMM, false);
	GPIO_SetState(GPIO_STST, State);
}
//-----------------------------

void LL_OutputCommutationControl(bool State)
{
	GPIO_SetState(GPIO_STST, false);
	GPIO_SetState(GPIO_COMM, State);
}
//-----------------------------

void LL_FanControl(bool State)
{
	GPIO_SetState(GPIO_FAN_CTRL, State);
}
//-----------------------------

void LL_ExtIndicationControl(bool State)
{
	GPIO_SetState(GPIO_IND_CTRL, State);
}
//-----------------------------

void LL_WriteDACx(Int16U Data)
{
	GPIO_SetState(GPIO_OPAMP_SYNC, false);
	SPI_WriteByte(SPI1, Data);
	GPIO_SetState(GPIO_OPAMP_SYNC, true);

	LL_ToggleLDAC();
}
//---------------------

void LL_ToggleLDAC()
{
	GPIO_SetState(GPIO_OPAMP_LDAC, false);
	DELAY_US(1);
	GPIO_SetState(GPIO_OPAMP_LDAC, true);
}
//---------------------
