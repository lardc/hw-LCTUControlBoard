// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "DataTable.h"
#include "ZwSPI.h"

// Variables
Int32U CycleCounters[COMMUTATION_TABLE_SIZE] = {0};
static bool RelayState[RELAY_COUNT] = {false};
const GPIO_PortPinSettingMacro* RelayPins[RELAY_COUNT] = {
		&GPIO_LCAU_INPUT_CONTACTOR, &GPIO_LCAU_HV_OUT, &GPIO_LCAU_DISCHARGE_DISABLE,
		&GPIO_HV_OUT, &GPIO_SELFTEST1_7MEG, &GPIO_SELFTEST2_700MEG,
		&GPIO_RMES1_NC, &GPIO_RMES2, &GPIO_RMES3, &GPIO_RMES4, &GPIO_RMES5};

// Forward functions
//
static void LL_UpdateRelayCounter(RelayId Id, bool NewState);
static void LL_SetChannelRelaysOff();

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED_BOARD);
}
//-----------------------------

void LL_ExtIndication(bool State)
{
	GPIO_SetState(GPIO_SW_IND, State);
}
//-----------------------------

void LL_SyncOSC(bool State)
{
	GPIO_SetState(GPIO_SW_SYNC, !State);
}
//-----------------------------

bool LL_IsSyncOn()
{
	return !GPIO_GetState(GPIO_SW_SYNC);
}
//-----------------------------

void LL_ToggleExternalLED()
{
	GPIO_Toggle(GPIO_SW_IND);
}
//-----------------------------

static void LL_UpdateRelayCounter(RelayId Id, bool NewState)
{
	if (!DataTable[REG_CNT_ACTIVE])
		return;
	if (!RelayState[Id] && NewState)
		CycleCounters[Id]++;
}
//-----------------------------

void LL_SetStateRelay(RelayId Id, bool State)
{
	if (Id >= RELAY_COUNT)
		return;
	LL_UpdateRelayCounter(Id, State);
	RelayState[Id] = State;
	GPIO_SetState(*RelayPins[Id], State);
}
//-----------------------------

void LL_SetRelaySafeState()
{
	LL_SetStateRelay(RELAY_LCAU_INPUT_CONTACTOR, false);
	LL_SetStateRelay(RELAY_LCAU_HV_OUT, false);
	LL_SetStateRelay(RELAY_LCAU_DISCHARGE_DISABLE, true);	// NC реле разряда батареи

	LL_SetStateRelay(RELAY_HV_OUT, false);
	LL_SetStateRelay(RELAY_SELFTEST1_7MEG, false);
	LL_SetStateRelay(RELAY_SELFTEST2_700MEG, false);

	LL_SetStateRelay(RELAY_RMES1_NC, false);				// NC реле канала тока
	LL_SetStateRelay(RELAY_RMES2, false);
	LL_SetStateRelay(RELAY_RMES3, false);
	LL_SetStateRelay(RELAY_RMES4, false);
	LL_SetStateRelay(RELAY_RMES5, false);
}
//-----------------------------

void LL_LCAU_SoftStart(bool State)
{
	GPIO_SetState(GPIO_LCAU_SOFTSTART_DISABLE, !State);
}
//-----------------------------

void LL_SPI_WriteByte(uint16_t Data, bool DACChannel)
{
	Data = DACChannel ? (Data | DAC_CHANNEL_B) : (Data & ~DAC_CHANNEL_B);

	GPIO_SetState(GPIO_SPI_SYNC, false);
	SPI_WriteByte(SPI1, Data);
	GPIO_SetState(GPIO_SPI_SYNC, true);

}
//-----------------------------

void LL_WriteDAC(Int16U DataA, Int16U DataB)
{
	LL_SPI_WriteByte(DataA, false);
	LL_SPI_WriteByte(DataB, true);
	LL_ToggleLDAC();
}
//-----------------------------

static void LL_SetChannelRelaysOff()
{
	LL_SetStateRelay(RELAY_RMES1_NC, false);
	LL_SetStateRelay(RELAY_RMES2, false);
	LL_SetStateRelay(RELAY_RMES3, false);
	LL_SetStateRelay(RELAY_RMES4, false);
	LL_SetStateRelay(RELAY_RMES5, false);
}
//-----------------------------

void LL_SetCurrentChannel(IChannel Channel)
{
	switch(Channel)
	{
		case I_CHANNEL_1:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES1_NC, true);
			break;
		case I_CHANNEL_2:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES2, true);
			break;
		case I_CHANNEL_3:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES3, true);
			break;
		case I_CHANNEL_4:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES4, true);
			break;
		case I_CHANNEL_5:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES5, true);
			break;
		default:
			LL_SetChannelRelaysOff();
			LL_SetStateRelay(RELAY_RMES1_NC, true);
			break;
	}
}
//-----------------------------

bool LL_IsSafetyOk()
{
	return GPIO_GetState(GPIO_SAFETY);
}
//-----------------------------

void LL_ToggleLDAC()
{
	GPIO_SetState(GPIO_SPI_LDAC, false);
	DELAY_US(1);
	GPIO_SetState(GPIO_SPI_LDAC, true);
}
//---------------------
