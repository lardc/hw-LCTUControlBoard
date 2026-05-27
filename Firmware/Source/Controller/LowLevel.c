// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "DataTable.h"
#include "ZwSPI.h"

// Variables
uint16_t PrevMask = 0;
uint16_t Mask = 0;
Int32U CycleCounters[COMMUTATION_TABLE_SIZE] = {0};
uint16_t const CommMask[] = {RELAY_CH_0, RELAY_CH_1, RELAY_CH_2, RELAY_CH_3, RELAY_CH_4,
							RELAY_TEST_LOAD, RELAY_NEG_POLARITY};

// Forward functions
//
void LL_SPI_SetStateOE(bool State);
void LL_Counter_Increase();

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_ExtIndication(bool State)
{
	GPIO_SetState(GPIO_LED_EXT, State);
}
//-----------------------------

void LL_Sync(bool State)
{
	GPIO_SetState(GPIO_SYNC, State);
}
//-----------------------------

void LL_ToggleExternalLED()
{
	GPIO_Toggle(GPIO_LED_EXT);
}
//-----------------------------

void LL_Counter_Increase()
{
	for(uint8_t i = 0; i < COMMUTATION_TABLE_SIZE; i++)
	{
		// Проверка для канала 0 тока
		if(i == 0)
		{
			if((PrevMask & RELAY_ALL_CHANNELS) != 0 && (Mask & RELAY_ALL_CHANNELS) == 0)
				CycleCounters[i]++;
			continue;
		}

		if((PrevMask & CommMask[i]) != CommMask[i] && (Mask & CommMask[i]) == CommMask[i])
			CycleCounters[i]++;
	}
}
//-----------------------------

void LL_SPI_WriteByte(uint16_t Data)
{
	if(DataTable[REG_CNT_ACTIVE])
		LL_Counter_Increase();

	GPIO_SetState(GPIO_SPI_SS, false);
	SPI_WriteByte(SPI1, Data);
	LL_SPI_SetStateOE(true);
	GPIO_SetState(GPIO_SPI_SS, true);
}
//-----------------------------

void LL_SPI_SetStateOE(bool State)
{
	GPIO_SetState(GPIO_SPI_OE, !State);
}
//-----------------------------

void LL_WriteDAC(Int16U Data)
{
	DAC_SetValueCh2(DAC1, Data);
}
//-----------------------------

void LL_SetCurrentChannel(IChannel Channel)
{
	Mask = PrevMask;
	Mask &=~ RELAY_ALL_CHANNELS;
	switch(Channel)
	{
		case I_CHANNEL_0:
			Mask |= RELAY_CH_0;
			break;
		case I_CHANNEL_1:
			Mask |= RELAY_CH_1;
			break;
		case I_CHANNEL_2:
			Mask |= RELAY_CH_2;
			break;
		case I_CHANNEL_3:
			Mask |= RELAY_CH_3;
			break;
		case I_CHANNEL_4:
			Mask |= RELAY_CH_4;
			break;
	}
	LL_SPI_WriteByte(Mask);
	PrevMask = Mask;
}
//-----------------------------

bool LL_SafetyState()
{
	return GPIO_GetState(GPIO_SAFETY);
}
//-----------------------------

void LL_SetNegativePolarity(bool State)
{
	Mask = PrevMask;
	State ? (Mask |= RELAY_NEG_POLARITY) : (Mask &=~ RELAY_NEG_POLARITY);
	LL_SPI_WriteByte(Mask);
	PrevMask = Mask;
}
//-----------------------------

void LL_SetSelfTestLoad(bool State)
{
	Mask = PrevMask;
	State ? (Mask |= RELAY_TEST_LOAD) : (Mask &=~ RELAY_TEST_LOAD);
	LL_SPI_WriteByte(Mask);
	PrevMask = Mask;
}
//-----------------------------
