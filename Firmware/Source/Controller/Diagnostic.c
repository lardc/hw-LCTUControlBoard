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
			break;

		case ACT_DBG_SPI_WRITE_TWO_BYTES:
			LL_SPI_WriteByte(DataTable[REG_DBG]);
			break;

		case ACT_DBG_PULSE:
			DIAG_GenerateTrapezoidWave();
			break;

		case ACT_DBG_SWITCH_RELAY:
			for(Int16U i = RELAY_RCON; i < RELAY_COUNT; i++)
			{
				LL_SetStateRelay((RelayId)i, true);
				DELAY_MS(200);
				LL_SetStateRelay((RelayId)i, false);
				DELAY_MS(100);
			}
			LL_SetRelaySafeState();
			break;

		case ACT_DBG_DAC_WRITE:
			{
				Int16U DACRaw =(Int16U) DataTable[REG_DBG];
				LL_WriteDAC(DACRaw);
			}
			break;

		case ACT_DBG_SYNC:
			LL_Sync(true);
			DELAY_US(1000);
			LL_Sync(false);
			break;

		case ACT_DBG_FAN:
			break;
		case ACT_DBG_ST:
			break;
		case ACT_DBG_LCTU_OUT:
			break;
		case ACT_DBG_LCAU_OUT:
			break;
		case ACT_DBG_CONT:
			break;
		case ACT_DBG_DIS:
			break;
		case ACT_DBG_SOFT:
			break;
		case ACT_DBG_SFTY_READ:
			break;
		case ACT_DBG_V_OUT_READ:
			break;
		case ACT_DBG_V_BAT_READ:
			break;
		case ACT_DBG_I_ADC_READ:
			break;
		case ACT_DBG_OPTIC:
			break;

		default:
			return false;
	}

	return true;
}
//------------------------------------------------

void DIAG_GenerateTrapezoidWave()
{
	float PulseAmplitude = ABS(DataTable[REG_WORK_VOLTAGE_ICES]) * CONVERSION_REDUC_THOUSAND;
	Int32U RiseMs = (Int32U)DataTable[REG_PULSE_RISE_DURATION];
	Int32U FlatMs = (Int32U)DataTable[REG_PULSE_DURATION];
	Int32U RiseSteps, FlatSteps;

	if (RiseMs == 0u)
		RiseMs = 1u;

	RiseSteps = (RiseMs * 1000u) / TIMER15_uS;
	FlatSteps = (FlatMs * 1000u) / TIMER15_uS;

	if (RiseSteps == 0u)
		RiseSteps = 1u;

	for (Int32U i = 1; i <= RiseSteps; ++i)
	{
		float setPoint = PulseAmplitude * ((float)i / (float)RiseSteps);
		LL_WriteDAC(MEASURE_ConvertUset(setPoint));
		DELAY_US(TIMER15_uS);
	}

	for (Int32U i = 0; i < FlatSteps; ++i)
	{
		LL_WriteDAC(MEASURE_ConvertUset(PulseAmplitude));
		DELAY_US(TIMER15_uS);
	}

	LL_WriteDAC(0);
}
//------------------------------------------------
