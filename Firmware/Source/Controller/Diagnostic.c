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
			LL_ExtIndication(DataTable[REG_DBG]);
			break;

		case ACT_DBG_SPI_WRITE_TWO_BYTES:
			LL_SPI_WriteByte(DataTable[REG_DBG]);
			break;

		case ACT_DBG_PULSE:
			DIAG_GenerateTrapezoidWave();
			break;

		case ACT_DBG_SWITCH_RELAY:
			for(Int16U i = RELAY_RMES1; i <= RELAY_RMES5; i++)
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

		case ACT_DBG_SYNC_OSC:
			LL_SyncOSC(DataTable[REG_DBG]);
			break;

		case ACT_DBG_FAN:
			GPIO_SetState(GPIO_SW_FAN, DataTable[REG_DBG]);
			break;

		case ACT_DBG_ST:
			if(DataTable[REG_DBG] == 0)
			{
				LL_SetStateRelay(RELAY_RST1, false);
				LL_SetStateRelay(RELAY_RST2, false);
			}
			else if(DataTable[REG_DBG] == 1)
			{
				LL_SetStateRelay(RELAY_RST2, false);
				LL_SetStateRelay(RELAY_RST1, true);
			}
			else if(DataTable[REG_DBG] == 2)
			{
				LL_SetStateRelay(RELAY_RST1, false);
				LL_SetStateRelay(RELAY_RST2, true);
			}
			else
				break;
			break;

		case ACT_DBG_LCTU_OUT:
			LL_SetStateRelay(RELAY_ROUT_LCTU, DataTable[REG_DBG]);
			break;

		case ACT_DBG_LCAU_OUT:
			LL_SetStateRelay(RELAY_ROUT_LCAU, DataTable[REG_DBG]);
			break;

		case ACT_DBG_CONT:
			LL_SetStateRelay(RELAY_RCON, DataTable[REG_DBG]);
			break;

		case ACT_DBG_DIS:
			LL_SetStateRelay(RELAY_RDIS, DataTable[REG_DBG]);
			break;

		case ACT_DBG_SOFT:
			GPIO_SetState(GPIO_RSS, DataTable[REG_DBG]);
			break;

		case ACT_DBG_SFTY_READ:
			DataTable[REG_DBG] = LL_SafetyState();
			break;

		case ACT_DBG_V_OUT_ADC_RAW_READ:
			{
				Int32U sum = 0;

				DMA_ChannelEnable(DMA2_Channel5, true);
				TIM_Start(TIM15);
				DELAY_MS(1);

				for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
					sum += REGLTR_MemBuffUce[i];
				DataTable[REG_DBG] = (Int16U)(sum / ADC_SEQ_LENGTH);

				TIM_Stop(TIM15);
				DMA_ChannelEnable(DMA2_Channel5, false);
			}
			break;

		case ACT_DBG_BAT_RAW_READ:
			TIM_Start(TIM15);
			DELAY_MS(1);
			DataTable[REG_DBG] = ADC1->DR;
			TIM_Stop(TIM15);
			break;

		case ACT_DBG_I_ADC_RAW_READ:
			{
				Int32U sum = 0;

				DMA_ChannelEnable(DMA1_Channel1, true);
				TIM_Start(TIM15);
				DELAY_MS(1);

				for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
					sum += REGLTR_MemBuffIces[i];
				DataTable[REG_DBG] = (Int16U)(sum / ADC_SEQ_LENGTH);

				TIM_Stop(TIM15);
				DMA_ChannelEnable(DMA1_Channel1, false);
			}
			break;
		case ACT_DBG_OPTIC:
			GPIO_InitPushPullOutput(GPIO_ALT_SPI_CLK);
			GPIO_InitPushPullOutput(GPIO_ALT_SPI_MOSI);

			GPIO_SetState(GPIO_SYNC, true);
			DELAY_MS(200);
			GPIO_SetState(GPIO_SYNC, false);
			DELAY_MS(100);

			GPIO_SetState(GPIO_ALT_SPI_CLK, true);
			DELAY_MS(200);
			GPIO_SetState(GPIO_ALT_SPI_CLK, false);
			DELAY_MS(100);

			GPIO_SetState(GPIO_ALT_SPI_MOSI, true);
			DELAY_MS(200);
			GPIO_SetState(GPIO_ALT_SPI_MOSI, false);
			DELAY_MS(100);

			GPIO_SetState(GPIO_SPI_NSS, false);
			DELAY_MS(200);
			GPIO_SetState(GPIO_SPI_NSS, true);
			DELAY_MS(100);

			GPIO_InitAltFunction(GPIO_ALT_SPI_CLK, AltFn_5);
			GPIO_InitAltFunction(GPIO_ALT_SPI_MOSI, AltFn_5);
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
