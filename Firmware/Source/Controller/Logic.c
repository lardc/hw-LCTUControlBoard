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
#include "Measurement.h"
#include "RingBuffer.h"

// Variables
//
static Int64U Timeout = 0;
Int16U LOGIC_ChannelNumber = 0;
Int16U RelaySwitchTimer = 0;
static Int16U ForcedCh = 0;
// Forward functions
//
void LOGIC_StopProcess();
void LOGIC_SwitchChannels(float Ig);
void LOGIC_SingleSw(float Ig);
void LOGIC_TestLoadRelaySwitch();
// Functions
//

void LOGIC_HandleMeasurement()
{
	static float UgResult, UpotResult, IgResult;

	if(CONTROL_State == DS_InProcess)
	{
		if(!CONTROL_IsSafetyOk())
			LOGIC_StopProcess();

		switch(CONTROL_SubState)
		{
			case SS_Init:
				UgResult = UpotResult = IgResult = 0.0f;
				ForcedCh = DataTable[REG_DIAG_FORCE_CHANNEL];

				switch(CONTROL_MeasureType)
				{
					case MT_Rth:
						if(ForcedCh && ForcedCh != I_CHANNEL_1 && ForcedCh != I_CHANNEL_2 && ForcedCh != I_CHANNEL_3)
						{
							CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
							return;
						}

						GPIO_SetState(GPIO_VCC_48, true);

						LL_SetCurrentChannel(ForcedCh ? ForcedCh : I_CHANNEL_1);
						LOGIC_ChannelNumber = ForcedCh ? ForcedCh : I_CHANNEL_1;
						RelaySwitchTimer = DataTable[REG_RELAY_SW_TIMER_RTH];
						break;

					case MT_Iges:
						if(ForcedCh && (ForcedCh < I_CHANNEL_0 || ForcedCh > I_CHANNEL_4))
						{
							CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
							return;
						}

						if (DataTable[REG_WORK_VOLTAGE_IGES] < 0)
							LL_SetNegativePolarity(true);
						GPIO_SetState(GPIO_VCC_48, true);

						LL_SetCurrentChannel(ForcedCh ? ForcedCh : I_CHANNEL_4);
						LOGIC_ChannelNumber = ForcedCh ? ForcedCh : I_CHANNEL_4;
						RelaySwitchTimer = DataTable[REG_RELAY_SW_TIMER_IGES];
						break;

					case MT_Ugeth:
						if(ForcedCh && ForcedCh != I_CHANNEL_1 && ForcedCh != I_CHANNEL_0)
						{
							CONTROL_SwitchToProblem(PROBLEM_WRONG_SELECTED_RELAY);
							return;
						}

						GPIO_SetState(GPIO_VCC_24, true);
						RelaySwitchTimer = DataTable[REG_RELAY_SW_TIMER_UGETH];

						if(ForcedCh)
						{
							LL_SetCurrentChannel(ForcedCh);
							LOGIC_ChannelNumber = ForcedCh;
						}
						else if((DataTable[REG_WORK_CURRENT_UGETH] * CONVERSION_REDUC_THOUSAND)	< DataTable[REG_RANGE_I_0])
						{
							LL_SetCurrentChannel(I_CHANNEL_1);
							LOGIC_ChannelNumber = I_CHANNEL_1;
						}
						else
						{
							LL_SetCurrentChannel(I_CHANNEL_0);
							LOGIC_ChannelNumber = I_CHANNEL_0;
						}
						break;

					case MT_ST_Upot:
						GPIO_SetState(GPIO_VCC_24, true);
						RelaySwitchTimer = DataTable[REG_REGLTR_TIMER] + DataTable[REG_ST_UPOT_FLATTOP_DURATION];
						LL_SetCurrentChannel(I_CHANNEL_0);
						LOGIC_ChannelNumber = I_CHANNEL_0;
						break;

					case MT_ST_TestLoad:
						GPIO_SetState(GPIO_VCC_48, true);
						RelaySwitchTimer = DataTable[REG_REGLTR_TIMER] + DataTable[REG_ST_TL_FLATTOP_DURATION];
						LL_SetSelfTestLoad(true);
						LOGIC_TestLoadRelaySwitch();
						break;
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
				LL_Sync(true);
				float TimeoutTime = (RelaySwitchTimer > DataTable[REG_REGLTR_TIMER]) ?
								RelaySwitchTimer : DataTable[REG_REGLTR_TIMER];
				Timeout = CONTROL_TimeCounter + TimeoutTime;

				if(CONTROL_MeasureType == MT_Ugeth)
					CONTROL_SetDeviceSubState(SS_RegulatorProcessUgeth);
				else if(CONTROL_MeasureType == MT_ST_Upot || CONTROL_MeasureType == MT_ST_TestLoad)
					CONTROL_SetDeviceSubState(SS_RegulatorProcessSelfTest);
				else
					CONTROL_SetDeviceSubState(SS_RegulatorProcess);
				break;

			case SS_RegulatorProcess:
				if(CONTROL_TimeCounter > Timeout)
				{
					UgResult = Sample.Ug;
					UpotResult = Sample.UPot;
					IgResult = Sample.Ig;
					if(IsMeasureOk)
						ForcedCh ? CONTROL_SetDeviceSubState(SS_FinishProcess) : LOGIC_SwitchChannels(IgResult);
				}
				break;

			case SS_RegulatorProcessSelfTest:
				if(CONTROL_TimeCounter > Timeout)
					if(IsMeasureOk)
					{
						IgResult = Sample.Ig;
						DataTable[REG_OP_RESULT] = OPRESULT_OK;
						CONTROL_SetDeviceSubState(SS_FinishProcess);
					}
				break;

			case SS_RegulatorProcessUgeth:
				if(CONTROL_TimeCounter > (Timeout + DataTable[REG_CURRENT_FLATTOP_DURATION]))
				{
					UgResult = Sample.Ug;
					UpotResult = Sample.UPot;
					IgResult = Sample.Ig;
					if(IsMeasureOk)
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
			case SS_CurrentErr:
				LOGIC_StopProcess();
				CONTROL_SwitchToProblem(PROBLEM_CURRENT_OUT_OF_RANGE);
				break;

			case SS_VoltageNoCurrentErr:
				LOGIC_StopProcess();
				CONTROL_SwitchToProblem(PROBLEM_VOLTAGE_LIMIT_NO_CURRENT);
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
						case MT_Rth:
							DataTable[REG_THERM_RESIS] = (IgResult != 0.0f) ? (UgResult / IgResult) : 0.0f;
							DataTable[REG_DIAG_CURRENT] = IgResult;
							DataTable[REG_DIAG_VOLTAGE] = UgResult;
							DataTable[REG_DIAG_POT_VOLTAGE] = UpotResult;
							break;
						case MT_Iges:
							DataTable[REG_DIAG_VOLTAGE] = UgResult;
							DataTable[REG_DIAG_POT_VOLTAGE] = UpotResult;
							if(RINGBUF_GetIcesAvgCount() >= ICES_AVG_BUF_SIZE)
							{
								DataTable[REG_IGES_RESULT] = RINGBUF_GetIcesAvg();
								DataTable[REG_DIAG_CURRENT] = IgResult;
							}
							else
								CONTROL_SwitchToProblem(PROBLEM_NEED_MORE_SAMPLES);
							break;

						case MT_Ugeth:
							DataTable[REG_DIAG_CURRENT] = IgResult;
							DataTable[REG_UGE_TH] = UgResult;
							DataTable[REG_DIAG_VOLTAGE] = UgResult;
							DataTable[REG_DIAG_POT_VOLTAGE] = UpotResult;
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
	LL_SetCurrentChannel(I_CHANNEL_0);
}
//------------------------------------------

void LOGIC_SwitchChannels(float Ig)
{
	switch(LOGIC_ChannelNumber)
	{
		case I_CHANNEL_0:
			LOGIC_SingleSw(Ig);
			break;

		case I_CHANNEL_1:
			LOGIC_SingleSw(Ig);
			break;

		case I_CHANNEL_2:
			LOGIC_SingleSw(Ig);
			break;

		case I_CHANNEL_3:
			LOGIC_SingleSw(Ig);
			break;

		case I_CHANNEL_4:
			CONTROL_SetDeviceSubState(SS_FinishProcess);
			break;
	}
}
//------------------------------------------

void LOGIC_SingleSw(float Ig)
{
	if(Ig < DataTable[REG_RANGE_I_0 + LOGIC_ChannelNumber - 1])
	{
		LL_SetCurrentChannel(LOGIC_ChannelNumber + 1);
		Timeout = CONTROL_TimeCounter + RelaySwitchTimer;
		LOGIC_ChannelNumber++;
	}
	else
		CONTROL_SetDeviceSubState(SS_FinishProcess);
}
//------------------------------------------

void LOGIC_TestLoadRelaySwitch()
{
	float CalcCurrent = (DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD] * 0.001) /  DataTable[REG_ST_TESTLOAD_RESIS];
	if(CalcCurrent > DataTable[REG_RANGE_I_0])
	{
		LL_SetCurrentChannel(I_CHANNEL_0);
		LOGIC_ChannelNumber = I_CHANNEL_0;
		return;
	}
	else if(CalcCurrent > DataTable[REG_RANGE_I_1])
	{
		LL_SetCurrentChannel(I_CHANNEL_1);
		LOGIC_ChannelNumber = I_CHANNEL_1;
		return;
	}
	else if(CalcCurrent > DataTable[REG_RANGE_I_2])
	{
		LL_SetCurrentChannel(I_CHANNEL_2);
		LOGIC_ChannelNumber = I_CHANNEL_2;
		return;
	}
	else if(CalcCurrent > DataTable[REG_RANGE_I_3])
	{
		LL_SetCurrentChannel(I_CHANNEL_3);
		LOGIC_ChannelNumber = I_CHANNEL_3;
		return;
	}
	else
	{
		LL_SetCurrentChannel(I_CHANNEL_4);
		LOGIC_ChannelNumber = I_CHANNEL_4;
	}
}
//------------------------------------------
