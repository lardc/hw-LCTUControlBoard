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
	GPIO_SetState(GPIO_PS_EN, !State);
}
//-----------------------------

void LL_SetStateLineSync(bool State)
{
	GPIO_SetState(GPIO_SYNC, State);
}
//-----------------------------

void LL_WriteDACx(Int16U Data)
{
	GPIO_SetState(GPIO_OPAMP_SYNC, false);
	SPI_WriteByte(SPI2, Data);
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
