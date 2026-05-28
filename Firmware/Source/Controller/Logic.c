// Header
//
#include "Logic.h"

// Includes
//
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "Board.h"
#include "LowLevel.h"
#include "Regulator.h"
#include "RingBuffer.h"

// Variables
//
static Int64U Timeout = 0;
static Int64U SyncDelayTimeout = 0;
Int16U LOGIC_ChannelNumber = 0;
static Int16U ForcedCh = 0;
static bool SyncIsOn = false;

// Forward functions
//
void LOGIC_StopProcess();
static IChannel LOGIC_SelectChannelByMaxCurrent(float ImaxA);
static bool LOGIC_SelectIcesChannel();
static void LOGIC_TestLoadRelaySwitch();

// Functions
//
void LOGIC_HandleMeasurement()
{
	static float UgResult, IgResult;

	if(CONTROL_State == DS_InProcess)
	{
		if(!CONTROL_IsSafetyOk())
			LOGIC_StopProcess();

		switch(CONTROL_SubState)
		{
			case SS_Init:
				UgResult = IgResult = 0.0f;
				ForcedCh = DataTable[REG_DIAG_FORCE_CHANNEL];
				SyncIsOn = false;
				SyncDelayTimeout = 0;

				switch(CONTROL_MeasureType)
				{
					case MT_Ices:
						if(!LOGIC_SelectIcesChannel())
						{
							CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
							return;
						}

						if(DataTable[REG_WORK_VOLTAGE_ICES] < 0)
							LL_SetNegativePolarity(true);
						GPIO_SetState(GPIO_VCC_48, true);
						break;

					case MT_ST_TestLoad:
						GPIO_SetState(GPIO_VCC_48, true);
						LL_SetSelfTestLoad(true);
						LOGIC_TestLoadRelaySwitch();
						break;

					default:
						CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
						return;
				}
				Timeout = CONTROL_TimeCounter + TIME_INIT_48V_TIMER;
				CONTROL_SetDeviceSubState(SS_Wait48VPause);
				break;

			case SS_Wait48VPause:
				if(CONTROL_TimeCounter > Timeout)
					CONTROL_SetDeviceSubState(SS_ConfigPulse);
				break;

			case SS_ConfigPulse:
				REGLTR_Init();
				REGLTR_StartProcess();
				SyncDelayTimeout = CONTROL_TimeCounter + DataTable[REG_PULSE_RISE_DURATION] + DataTable[REG_SYNC_DELAY_AFTER_FLAT];
				{
					float TimeoutTime;
					if (CONTROL_MeasureType == MT_ST_TestLoad)
						TimeoutTime = DataTable[REG_PULSE_RISE_DURATION] + DataTable[REG_ST_TL_FLATTOP_DURATION];
					else
						TimeoutTime = DataTable[REG_PULSE_RISE_DURATION] + DataTable[REG_PULSE_DURATION];
					Timeout = CONTROL_TimeCounter + TimeoutTime;
				}

				if(CONTROL_MeasureType == MT_ST_TestLoad)
					CONTROL_SetDeviceSubState(SS_RegulatorProcessSelfTest);
				else
					CONTROL_SetDeviceSubState(SS_RegulatorProcess);
				break;

			case SS_RegulatorProcess:
				if (!SyncIsOn && CONTROL_TimeCounter > SyncDelayTimeout)
				{
					LL_Sync(true);
					SyncIsOn = true;
				}

				if(CONTROL_TimeCounter > Timeout)
				{
					UgResult = Sample.Ug;
					IgResult = Sample.Ig;
					if(IsMeasureOk)
						CONTROL_SetDeviceSubState(SS_FinishProcess);
				}
				break;

			case SS_RegulatorProcessSelfTest:
				if (!SyncIsOn && CONTROL_TimeCounter > SyncDelayTimeout)
				{
					LL_Sync(true);
					SyncIsOn = true;
				}

				if(CONTROL_TimeCounter > Timeout)
					if(IsMeasureOk)
					{
						IgResult = Sample.Ig;
						DataTable[REG_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceSubState(SS_FinishProcess);
					}
				break;

			case SS_FollowingErr:
				LOGIC_StopProcess();
				CONTROL_SwitchToProblem(PROBLEM_FOLLOWING_ERROR);
				break;

			case SS_VoltageErr:
				LOGIC_StopProcess();
				CONTROL_SwitchToProblem(PROBLEM_VOLTAGE_OUT_OF_RANGE);
				break;

			case SS_MaxCurrentErr:
				LOGIC_StopProcess();
				CONTROL_SwitchToProblem(PROBLEM_MAX_CURRENT_EXCEEDED);
				break;

			case SS_FinishProcess:
				LOGIC_StopProcess();
				Timeout = CONTROL_TimeCounter + TIME_INIT_48V_TIMER;
				CONTROL_SetDeviceSubState(SS_GetResults);
				break;

			case SS_GetResults:
				if(CONTROL_TimeCounter > Timeout)
				{
					CONTROL_SetDeviceState(DS_Ready);
					CONTROL_SetDeviceSubState(SS_None);

					switch(CONTROL_MeasureType)
					{
						case MT_Ices:
							DataTable[REG_DIAG_VOLTAGE] = UgResult;
							DataTable[REG_ICES_RESULT] = IgResult;
							DataTable[REG_DIAG_CURRENT] = IgResult;
							DataTable[REG_OP_RESULT] = OPRESULT_OK;
							break;

						default:
							break;
					}
				}
				break;

			default:
				break;
		}
	}
}
//------------------------------------------

void LOGIC_StopProcess()
{
	REGLTR_StopProcess();
	GPIO_SetState(GPIO_VCC_48, false);
	GPIO_SetState(GPIO_VCC_24, false);
	LL_SetNegativePolarity(false);
	LL_SetSelfTestLoad(false);
	LL_Sync(false);
	LL_SetCurrentChannel(I_CHANNEL_1);
}
//------------------------------------------

static IChannel LOGIC_SelectChannelByMaxCurrent(float ImaxA)
{
	if(ImaxA > DataTable[REG_RANGE_I_0])
		return I_CHANNEL_5;
	if(ImaxA > DataTable[REG_RANGE_I_1])
		return I_CHANNEL_4;
	if(ImaxA > DataTable[REG_RANGE_I_2])
		return I_CHANNEL_3;
	if(ImaxA > DataTable[REG_RANGE_I_3])
		return I_CHANNEL_2;
	return I_CHANNEL_1;
}
//------------------------------------------

static bool LOGIC_SelectIcesChannel()
{
	if(ForcedCh)
	{
		if(ForcedCh < I_CHANNEL_1 || ForcedCh > I_CHANNEL_5)
			return false;
		LOGIC_ChannelNumber = ForcedCh;
	}
	else
	{
		float ImaxA = DataTable[REG_MAX_CURRENT_ICES] * CONVERSION_REDUC_THOUSAND;
		LOGIC_ChannelNumber = LOGIC_SelectChannelByMaxCurrent(ImaxA);
	}

	LL_SetCurrentChannel(LOGIC_ChannelNumber);
	return true;
}
//------------------------------------------

static void LOGIC_TestLoadRelaySwitch()
{
	float CalcCurrent = (DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD] * 0.001f) / DataTable[REG_ST_TESTLOAD_RESIS];
	LOGIC_ChannelNumber = LOGIC_SelectChannelByMaxCurrent(CalcCurrent);
	LL_SetCurrentChannel(LOGIC_ChannelNumber);
}
//------------------------------------------
