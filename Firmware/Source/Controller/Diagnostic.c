// Header
#include "Diagnostic.h"

// Include
#include "Delay.h"
#include "LowLevel.h"
#include "Board.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "Measurement.h"
#include "Regulator.h"
#include "SysConfig.h"

// Variables
//
Int16U DIAG_PulseDataBuffer[DIAG_PULSE_BUFFER_SIZE];

// Functions
//
bool DIAG_HandleDiagnosticAction(Int16U ActionID, Int16U *pUserError)
{
	switch (ActionID)
	{
		case ACT_DBG_EXT_INDICATION:
			LL_ExtIndication(true);
			DELAY_MS(500);
			LL_ExtIndication(false);
			break;

		case ACT_DBG_SPI_WRITE_TWO_BYTES:
			LL_SPI_WriteByte(DataTable[REG_DBG]);
			break;

		case ACT_DBG_PULSE:
			DIAG_GenerateTrapezoidWave();
			break;

		case ACT_DBG_SWITCH_POWER:
			GPIO_SetState(GPIO_VCC_48, true);
			DELAY_MS(500);
			GPIO_SetState(GPIO_VCC_48, false);
			GPIO_SetState(GPIO_VCC_24, true);
			DELAY_MS(500);
			GPIO_SetState(GPIO_VCC_24,false);
			break;

		case ACT_DBG_SWITCH_RELAY:
			for(Int16U i=1; i<=I_CHANNEL_4; i++)
			{
				LL_SetCurrentChannel(i);
				DELAY_MS(1000);
			}
			LL_SetCurrentChannel(I_CHANNEL_0);
			break;

		case ACT_DBG_DAC_WRITE:
			{
				Int16U DACRaw =(Int16U) DataTable[REG_DBG];
				LL_WriteDAC(DACRaw);
			}
			break;

		case ACT_DBG_48V_ON:
			GPIO_SetState(GPIO_VCC_48, true);
			break;

		case ACT_DBG_48V_OFF:
			GPIO_SetState(GPIO_VCC_48, false);
			break;

		case ACT_DBG_24V_ON:
			GPIO_SetState(GPIO_VCC_24, true);
			break;

		case ACT_DBG_24V_OFF:
			GPIO_SetState(GPIO_VCC_24, false);
			break;

		case ACT_DBG_SYNC:
			LL_Sync(true);
			DELAY_US(1000);
			LL_Sync(false);
			break;

		default:
			return false;
	}

	return true;
}
//------------------------------------------------

void DIAG_GenerateTrapezoidWave()
{
	/*for (Int16U i = 0; i < REGLTR_PulseSamples.TotalSamples; ++i)
	{
		LL_WriteDAC(MEASURE_ConvertUset(REGLTR_GetSetpoint(i)));
		DELAY_US(TIMER15_uS);
	}*/
}
//------------------------------------------------
