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
#include "Measurement.h"

// Variables
//
static Int64U Timeout = 0;
static Int64U SyncDelayTimeout = 0;
Int16U LOGIC_ChannelNumber = 0;
static Int16U ForcedCh = 0;
static Int16U SelfTestStepIdx = 0;

// Forward functions
//
static IChannel LOGIC_SelectChannelByMaxCurrent(float ImaxA);
static bool LOGIC_SelectIcesChannel();
static bool LOGIC_SetupSelfTestStep(Int16U StepIdx, float* ExpectedCurrentA);
static void LOGIC_ErrorHandler(DeviceSubState SubState);

// Functions
//
void LOGIC_HandleMeasurement()
{
	static float UceResult, IcesResult;
	static float SelfTestExpectedCurrentA = 0.0f;

	if(CONTROL_State == DS_InProcess)
	{
		if(!CONTROL_IsSafetyOk())
			return;

		float Ucap = DataTable[REG_U_BAT] = MEASURE_Ucap();

		switch(CONTROL_SubState)
		{
			case SS_Activation:
				LL_SetStateRelay(RELAY_LCAU_DISCHARGE_DISABLE, true);
				LL_SetStateRelay(RELAY_LCAU_INPUT_CONTACTOR, true);
				if(Ucap >= DataTable[REG_U_CAP_ACTIVATE_RSS])
					LL_LCAU_SoftStart(false);

				if(Ucap >= DataTable[REG_U_CAP_READY])
				{
					CONTROL_SetDeviceState(DS_Ready);
					CONTROL_SetDeviceSubState(SS_None);
				}
				break;

			case SS_Init:
				if(Ucap < DataTable[REG_U_CAP_READY])
				{
					CONTROL_SwitchToFault(DF_CAP_VOLTAGE_LOW);
					break;
				}

				LL_SetStateRelay(RELAY_LCAU_HV_OUT, true);
				if(CONTROL_MeasureType == MT_ST_TestLoad)
					LL_SetStateRelay(RELAY_HV_OUT, false);
				else
					LL_SetStateRelay(RELAY_HV_OUT, true);

				UceResult = IcesResult = 0.0f;
				ForcedCh = DataTable[REG_DIAG_FORCE_CHANNEL];
				SyncDelayTimeout = 0;

				switch(CONTROL_MeasureType)
				{
					case MT_Ices:
						if(!LOGIC_SelectIcesChannel())
						{
							CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
							return;
						}
						Timeout = CONTROL_TimeCounter + DataTable[REG_RELAY_SW_TIMER_ICES];
						CONTROL_SetDeviceSubState(SS_InitialRelayPause);
						break;

					case MT_ST_TestLoad:
						SelfTestStepIdx = 0;
						Timeout = CONTROL_TimeCounter + DataTable[REG_DEACT_ROUT_DELAY];
						CONTROL_SetDeviceSubState(SS_InitialRelayPause);
						break;

					default:
						CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
						return;
				}
				break;

			case SS_InitialRelayPause:
				if(CONTROL_TimeCounter > Timeout)
				{
					if (CONTROL_MeasureType == MT_ST_TestLoad)
					{
						if (!LOGIC_SetupSelfTestStep(SelfTestStepIdx, &SelfTestExpectedCurrentA))
						{
							DataTable[REG_DIAG_CURRENT] = Sample.Ices;
							DataTable[REG_DIAG_VOLTAGE] = Sample.Uce;
							LOGIC_StopProcess();
							CONTROL_SwitchToFault(DF_SELFTEST_FAILED);
							break;
						}
					}
					CONTROL_SetDeviceSubState(SS_ConfigPulse);
				}
				break;

			case SS_ConfigPulse:
				REGLTR_Init();
				REGLTR_StartProcess();
				SyncDelayTimeout = CONTROL_TimeCounter + DataTable[REG_PULSE_RISE_DURATION] + DataTable[REG_SYNC_DELAY_AFTER_FLAT];
				{
					float TimeoutTime;
					if (CONTROL_MeasureType == MT_ST_TestLoad)
						TimeoutTime = DataTable[REG_PULSE_RISE_DURATION] + DataTable[REG_ST_PULSE_DURATION];
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
				if (!LL_IsSyncOn() && CONTROL_TimeCounter > SyncDelayTimeout)
					LL_SyncOSC(true);

				if(CONTROL_TimeCounter > Timeout)
				{
					UceResult = Sample.Uce;
					IcesResult = Sample.Ices;
					if(IsMeasureOk)
						CONTROL_SetDeviceSubState(SS_FinishProcess);
				}
				break;

			case SS_RegulatorProcessSelfTest:
				if (!LL_IsSyncOn() && CONTROL_TimeCounter > SyncDelayTimeout)
					LL_SyncOSC(true);

				if(IsMeasureOk && CONTROL_TimeCounter > Timeout)
				{
					IcesResult = Sample.Ices;
					if(ABS(ABS(IcesResult) - SelfTestExpectedCurrentA) > (SelfTestExpectedCurrentA * DataTable[REG_ST_CURRENT_ERR_THRESH]))
					{
						CONTROL_SetDeviceSubState(SS_CurrentErr);
						break;
					}
					if(SelfTestStepIdx >= 5)
					{
						DataTable[REG_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceSubState(SS_FinishProcess);
					}
					else
					{
						SelfTestStepIdx++;
						if(!LOGIC_SetupSelfTestStep(SelfTestStepIdx, &SelfTestExpectedCurrentA))
						{
							DataTable[REG_DIAG_CURRENT] = Sample.Ices;
							DataTable[REG_DIAG_VOLTAGE] = Sample.Uce;
							LOGIC_StopProcess();
							CONTROL_SwitchToFault(DF_SELFTEST_FAILED);
							break;
						}
						CONTROL_SetDeviceSubState(SS_ConfigPulse);
					}
				}
				break;

			case SS_FollowingErr:
			case SS_VoltageErr:
			case SS_CurrentErr:
			case SS_MaxCurrentErr:
				LOGIC_ErrorHandler(CONTROL_SubState);
				break;

			case SS_FinishProcess:
				LOGIC_StopProcess();
				CONTROL_SetDeviceState(DS_Ready);
				CONTROL_SetDeviceSubState(SS_None);

				switch(CONTROL_MeasureType)
				{
					case MT_Ices:
						DataTable[REG_DIAG_VOLTAGE] = UceResult;
						DataTable[REG_ICES_RESULT] = IcesResult;
						DataTable[REG_DIAG_CURRENT] = IcesResult;
						DataTable[REG_OP_RESULT] = OPRESULT_OK;
						break;

					default:
						break;
				}
				break;

			default:
				break;
		}
	}
}
//------------------------------------------

void LOGIC_Deactivate()
{
	LOGIC_StopProcess();

	LL_SetStateRelay(RELAY_RMES1_NC, false);
	LL_SetStateRelay(RELAY_RMES2, false);
	LL_SetStateRelay(RELAY_RMES3, false);
	LL_SetStateRelay(RELAY_RMES4, false);
	LL_SetStateRelay(RELAY_RMES5, false);

	LL_SetStateRelay(RELAY_HV_OUT, false);
	LL_SetStateRelay(RELAY_LCAU_HV_OUT, false);
	LL_SetStateRelay(RELAY_LCAU_INPUT_CONTACTOR, false);

	LL_SetStateRelay(RELAY_LCAU_DISCHARGE_DISABLE, false);
	LL_LCAU_SoftStart(true);
}
//------------------------------------------

void LOGIC_StopProcess()
{
	REGLTR_StopProcess();
	LL_SyncOSC(false);
	LL_SetCurrentChannel(I_CHANNEL_1);
	DataTable[REG_SELFTEST_STEP] = 0;
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
		float ImaxA = DataTable[REG_MAX_CURRENT_ICES] * 0.001f;
		LOGIC_ChannelNumber = LOGIC_SelectChannelByMaxCurrent(ImaxA);
	}

	LL_SetCurrentChannel(LOGIC_ChannelNumber);
	return true;
}
//------------------------------------------

static bool LOGIC_SetupSelfTestStep(Int16U StepIdx, float* ExpectedCurrentA)
{
	float Resistance;

	DataTable[REG_SELFTEST_STEP] = StepIdx + 1;

	LL_SetStateRelay(RELAY_SELFTEST1_7MEG, false);
	LL_SetStateRelay(RELAY_SELFTEST2_700MEG, false);

	switch (StepIdx)
	{
		case 0:
			LL_SetStateRelay(RELAY_SELFTEST1_7MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_1;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_7MOHM];
			break;
		case 1:
			LL_SetStateRelay(RELAY_SELFTEST1_7MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_1;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_7MOHM];
			break;
		case 2:
			LL_SetStateRelay(RELAY_SELFTEST2_700MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_2;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_700MOHM];
			break;
		case 3:
			LL_SetStateRelay(RELAY_SELFTEST2_700MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_3;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_700MOHM];
			break;
		case 4:
			LL_SetStateRelay(RELAY_SELFTEST2_700MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_4;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_700MOHM];
			break;
		case 5:
			LL_SetStateRelay(RELAY_SELFTEST2_700MEG, true);
			LOGIC_ChannelNumber = I_CHANNEL_5;
			Resistance = DataTable[REG_ST_TESTLOAD_RESIS_700MOHM];
			break;
		default:
			return false;
	}

	*ExpectedCurrentA = (DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD] * 0.001f) / Resistance;

	LL_SetCurrentChannel(LOGIC_ChannelNumber);
	return true;
}
//------------------------------------------

static void LOGIC_ErrorHandler(DeviceSubState SubState)
{
	Int16U FaultReason, ProblemReason;

	switch(SubState)
	{
		case SS_FollowingErr:
			FaultReason = DF_FOLLOWING_ERROR;
			ProblemReason = PROBLEM_FOLLOWING_ERROR;
			break;

		case SS_VoltageErr:
			FaultReason = DF_VOLTAGE_OUT_OF_RANGE;
			ProblemReason = PROBLEM_VOLTAGE_OUT_OF_RANGE;
			break;

		case SS_CurrentErr:
			FaultReason = DF_CURRENT_OUT_OF_RANGE;
			ProblemReason = PROBLEM_SELFTEST_FAILED;
			break;

		case SS_MaxCurrentErr:
			LOGIC_StopProcess();
			CONTROL_SwitchToProblem(PROBLEM_MAX_CURRENT_EXCEEDED);
			return;

		default:
			return;
	}

	LOGIC_StopProcess();

	if(CONTROL_MeasureType == MT_ST_TestLoad)
	{
		DataTable[REG_DIAG_CURRENT] = Sample.Ices;
		DataTable[REG_DIAG_VOLTAGE] = Sample.Uce;
		CONTROL_SwitchToFault(FaultReason);
	}
	else
		CONTROL_SwitchToProblem(ProblemReason);
}
//------------------------------------------
