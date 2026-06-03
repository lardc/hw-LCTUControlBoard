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
static bool SyncIsOn = false;
static Int16U SelfTestStepIdx = 0;
static bool SelfTestStartDelayDone = false;

// Forward functions
//
void LOGIC_StopProcess();
static IChannel LOGIC_SelectChannelByMaxCurrent(float ImaxA);
static bool LOGIC_SelectIcesChannel();
static bool LOGIC_SetupSelfTestStep(Int16U StepIdx, float* ExpectedCurrentA);
static bool LOGIC_IsSelfTestStepOk(float MeasuredCurrentA, float ExpectedCurrentA);

// Functions
//
void LOGIC_HandleMeasurement()
{
	static float UceResult, IcesResult;
	static float SelfTestExpectedCurrentA = 0.0f;

	if(CONTROL_State == DS_InProcess)
	{
		float Ucap;

		if(!CONTROL_IsSafetyOk())
			return;

		Ucap = MEASURE_Ucap();

		switch(CONTROL_SubState)
		{
			case SS_Activation:
				// Активация по ТТ: Rdis open, Rcon close; Rss - после порога Ucap.
				LL_SetStateRelay(RELAY_RDIS, false);
				LL_SetStateRelay(RELAY_RCON, true);
				if (Ucap >= DataTable[REG_U_CAP_ACTIVATE_RSS])
					GPIO_SetState(GPIO_RSS, true);
				if (Ucap >= DataTable[REG_U_CAP_READY])
				{
					CONTROL_SetDeviceState(DS_Ready);
					CONTROL_SetDeviceSubState(SS_None);
					DataTable[REG_OP_RESULT] = OPRESULT_OK;
				}
				break;

			case SS_Deactivation:
				LOGIC_StopProcess();
				LL_SetStateRelay(RELAY_RMES1, false);
				LL_SetStateRelay(RELAY_RMES2, false);
				LL_SetStateRelay(RELAY_RMES3, false);
				LL_SetStateRelay(RELAY_RMES4, false);
				LL_SetStateRelay(RELAY_RMES5, true);
				LL_SetStateRelay(RELAY_ROUT_LCAU, false);
				GPIO_SetState(GPIO_RSS, false);
				Timeout = CONTROL_TimeCounter + DataTable[REG_DEACT_ROUT_DELAY];
				CONTROL_SetDeviceSubState(SS_DeactivationWaitRout);
				break;

			case SS_DeactivationWaitRout:
				if(CONTROL_TimeCounter > Timeout)
				{
					LL_SetStateRelay(RELAY_ROUT_LCTU, false);
					LL_SetStateRelay(RELAY_RCON, false);
					Timeout = CONTROL_TimeCounter + DataTable[REG_DEACT_RCON_DELAY];
					CONTROL_SetDeviceSubState(SS_DeactivationWaitRcon);
				}
				break;

			case SS_DeactivationWaitRcon:
				if(CONTROL_TimeCounter > Timeout)
				{
					LL_SetStateRelay(RELAY_RDIS, true);
					CONTROL_SetDeviceState(DS_None);
					CONTROL_SetDeviceSubState(SS_None);
					DataTable[REG_OP_RESULT] = OPRESULT_OK;
				}
				break;

			case SS_Preparation:
				if (Ucap < DataTable[REG_U_CAP_READY])
				{
					CONTROL_SwitchToProblem(PROBLEM_CAP_VOLTAGE_LOW);
					break;
				}
				LL_SetStateRelay(RELAY_ROUT_LCAU, true);
				if (CONTROL_MeasureType == MT_ST_TestLoad)
					LL_SetStateRelay(RELAY_ROUT_LCTU, false);
				else
					LL_SetStateRelay(RELAY_ROUT_LCTU, true);
				CONTROL_SetDeviceSubState(SS_Init);
				break;

			case SS_Init:
				UceResult = IcesResult = 0.0f;
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
						Timeout = CONTROL_TimeCounter + DataTable[REG_RELAY_SW_TIMER_ICES];
						CONTROL_SetDeviceSubState(SS_InitialRelayPause);
						break;

					case MT_ST_TestLoad:
						SelfTestStepIdx = 0;
						SelfTestStartDelayDone = false;
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
						if (!SelfTestStartDelayDone)
						{
							SelfTestStartDelayDone = true;
							if (!LOGIC_SetupSelfTestStep(SelfTestStepIdx, &SelfTestExpectedCurrentA))
							{
								CONTROL_SwitchToProblem(PROBLEM_SELFTEST_FAILED);
								break;
							}
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
				if (!SyncIsOn && CONTROL_TimeCounter > SyncDelayTimeout)
				{
					LL_Sync(true);
					SyncIsOn = true;
				}

				if(CONTROL_TimeCounter > Timeout)
				{
					UceResult = Sample.Uce;
					IcesResult = Sample.Ices;
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
						IcesResult = Sample.Ices;
						if(!LOGIC_IsSelfTestStepOk(IcesResult, SelfTestExpectedCurrentA))
						{
							CONTROL_SwitchToProblem(PROBLEM_SELFTEST_FAILED);
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
							if (!LOGIC_SetupSelfTestStep(SelfTestStepIdx, &SelfTestExpectedCurrentA))
							{
								CONTROL_SwitchToProblem(PROBLEM_SELFTEST_FAILED);
								break;
							}
							CONTROL_SetDeviceSubState(SS_ConfigPulse);
						}
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

void LOGIC_StopProcess()
{
	REGLTR_StopProcess();
	LL_Sync(false);
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
		float ImaxA = DataTable[REG_MAX_CURRENT_ICES] * CONVERSION_REDUC_THOUSAND;
		LOGIC_ChannelNumber = LOGIC_SelectChannelByMaxCurrent(ImaxA);
	}

	LL_SetCurrentChannel(LOGIC_ChannelNumber);
	return true;
}
//------------------------------------------

static bool LOGIC_SetupSelfTestStep(Int16U StepIdx, float* ExpectedCurrentA)
{
	if (ExpectedCurrentA == 0)
		return false;

	DataTable[REG_SELFTEST_STEP] = StepIdx + 1;

	LL_SetStateRelay(RELAY_RST1, false);
	LL_SetStateRelay(RELAY_RST2, false);

	switch (StepIdx)
	{
		case 0:
			LL_SetStateRelay(RELAY_RST1, true);
			LOGIC_ChannelNumber = I_CHANNEL_1;
			*ExpectedCurrentA = 10e-6f;
			break;
		case 1:
			LL_SetStateRelay(RELAY_RST1, true);
			LOGIC_ChannelNumber = I_CHANNEL_1;
			*ExpectedCurrentA = 100e-6f;
			break;
		case 2:
			LL_SetStateRelay(RELAY_RST2, true);
			LOGIC_ChannelNumber = I_CHANNEL_2;
			*ExpectedCurrentA = 1e-3f;
			break;
		case 3:
			LL_SetStateRelay(RELAY_RST2, true);
			LOGIC_ChannelNumber = I_CHANNEL_3;
			*ExpectedCurrentA = 10e-3f;
			break;
		case 4:
			LL_SetStateRelay(RELAY_RST2, true);
			LOGIC_ChannelNumber = I_CHANNEL_4;
			*ExpectedCurrentA = 100e-3f;
			break;
		case 5:
			LL_SetStateRelay(RELAY_RST2, true);
			LOGIC_ChannelNumber = I_CHANNEL_5;
			*ExpectedCurrentA = 300e-3f;
			break;
		default:
			return false;
	}

	LL_SetCurrentChannel(LOGIC_ChannelNumber);
	return true;
}
//------------------------------------------

static bool LOGIC_IsSelfTestStepOk(float MeasuredCurrentA, float ExpectedCurrentA)
{
	const float tolerance = 0.1f;
	float absMeasured = ABS(MeasuredCurrentA);
	float diff = ABS(absMeasured - ExpectedCurrentA);
	return diff <= (ExpectedCurrentA * tolerance);
}
//------------------------------------------
