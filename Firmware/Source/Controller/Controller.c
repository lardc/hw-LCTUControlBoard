﻿// Header
#include "Controller.h"
//
// Includes
#include "Board.h"
#include "Delay.h"
#include "DeviceProfile.h"
#include "Interrupts.h"
#include "LowLevel.h"
#include "SysConfig.h"
#include "DebugActions.h"
#include "Diagnostic.h"
#include "Logic.h"
#include "BCCIxParams.h"
#include "DiscreteOpAmp.h"
#include "SelfTest.h"
#include "Measurement.h"
#include "BCCIMHighLevel.h"
#include "PAU.h"

// Definitions
//
#define PS_FULL_CHARGE_TIME			2000	// мс
#define PS_RECHARGE_TIME			50		// мс

// Types
//
typedef void (*FUNC_AsyncDelegate)();

// Variables
//
volatile DeviceState CONTROL_State = DS_None;
volatile DeviceSubState CONTROL_SubState = SS_None;
static Boolean CycleActive = false;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U	CONTROL_PulseToPulseTime = 0;
volatile Int64U	CONTROL_PowerOnCounter = 0;
volatile Int64U	CONTROL_Timeout = 0;
volatile Int16U CONTROL_ValuesCounter = 0;
volatile float CONTROL_ValuesVoltage[VALUES_x_SIZE];
volatile float CONTROL_ValuesCurrent[VALUES_x_SIZE];
volatile float CONTROL_RegulatorErr[VALUES_x_SIZE];
volatile Int16U CONTROL_FloatEP_Counter = 0;
volatile Boolean CONTROL_PowerOnState = false;
//

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_StopProcess();
void CONTROL_SaveTestResult();
void CONTROL_SelfTest();
void CONTROL_Commutation(TestType Type, bool State);
bool CONTROL_IsSafetyEvent();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[FEP_COUNT] = {EP_VOLTAGE, EP_CURRENT, EP_REGULATOR_ERR};
	Int16U EPSized[FEP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_ValuesCounter, (pInt16U)&CONTROL_ValuesCounter, (pInt16U)&CONTROL_ValuesCounter};
	pFloat32 EPDatas[FEP_COUNT] = {(pFloat32)CONTROL_ValuesVoltage, (pFloat32)CONTROL_ValuesCurrent, (pFloat32)CONTROL_RegulatorErr};

	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_NID, 0);
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitFEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaultState();
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_SetDeviceState(DS_None, SS_None);

	LOGIC_ResetOutputRegisters();
	DISOPAMP_SetVoltage(0);
	LOGIC_HarwareDefaultState();
}
//------------------------------------------

void CONTROL_Idle()
{
	CONTROL_LogicProcess();

	DEVPROFILE_ProcessRequests();
	CONTROL_UpdateWatchDog();
}
//------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case ACT_ENABLE_POWER:
			if(CONTROL_State == DS_None)
			{
				CONTROL_Timeout = CONTROL_TimeCounter + DataTable[REG_PS_FIRST_START_TIME];
				CONTROL_SetDeviceState(DS_InProcess, SS_PowerSupply);
			}
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_DISABLE_POWER:
			if(CONTROL_State == DS_Ready || (CONTROL_State == DS_InProcess && CONTROL_SubState == SS_PowerSupply))
			{
				LL_PowerSupply(false);
				CONTROL_SetDeviceState(DS_None, SS_None);
			}
			else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_START_TEST:
			if (CONTROL_State == DS_Ready)
			{
				LOGIC_ResetOutputRegisters();
				// Проверка безопасности до запуска
				if(CONTROL_IsSafetyEvent())
				{
					DataTable[REG_PROBLEM] = PROBLEM_SAFETY;
					DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
				}
				else
					CONTROL_SetDeviceState(DS_InProcess, SS_IdleTimeCheck);
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_STOP_TEST:
			if (CONTROL_State == DS_InProcess)
				CONTROL_ForceStopProcess(PROBLEM_FORCED_STOP);
			break;

		case ACT_START_SELF_TEST:
			if(CONTROL_State == DS_Ready)
			{
				LOGIC_ResetOutputRegisters();
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_InProcess, SS_StartSelfTest);
			}
			else
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_CLR_FAULT:
			if(PAU_ClearFault() && CONTROL_State == DS_Fault)
				CONTROL_ResetToDefaultState();
			break;

		case ACT_CLR_WARNING:
			if(PAU_ClearWarning())
				DataTable[REG_WARNING] = WARNING_NONE;
			break;

		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
			
	}
	return true;
}
//-----------------------------------------------

void CONTROL_ForceStopProcess(Int16U Problem)
{
	LOGIC_StopProcess();
	LOGIC_HarwareDefaultState();

	DataTable[REG_PROBLEM] = Problem;
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;

	CONTROL_SetDeviceSubState(SS_PostPulseDelayInit);
}
//-----------------------------------------------

void CONTROL_LogicProcess()
{
	Int16U PAU_State;

	if(CONTROL_State == DS_InProcess)
	{
		switch(CONTROL_SubState)
		{
			case SS_PowerSupply:
				if(CONTROL_TimeCounter >= CONTROL_Timeout)
				{
					LL_ExtIndicationControl(false);
					LL_PowerSupply(true);
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				}
				else
				{
					LL_ExtIndicationControl(true);

					if(CONTROL_TimeCounter > CONTROL_PowerOnCounter)
					{
						if(CONTROL_PowerOnState)
						{
							CONTROL_PowerOnCounter = CONTROL_TimeCounter + DataTable[REG_PWRON_PERIOD] - DataTable[REG_PWRON_TIME];
							CONTROL_PowerOnState = false;
						}
						else
						{
							CONTROL_PowerOnCounter = CONTROL_TimeCounter + DataTable[REG_PWRON_TIME];
							CONTROL_PowerOnState = true;
						}
					}

					LL_PowerSupply(CONTROL_PowerOnState);
				}
				break;

			case SS_StartSelfTest:
				if(DataTable[REG_SELF_TEST_ACTIVE])
				{
					CONTROL_Commutation(TT_SelfTest, true);
					DELAY_MS(COMMUTATION_DELAY);

					CONTROL_SetDeviceState(DS_SelfTest, SS_ST_StartPrepare);
				}
				else
					CONTROL_SetDeviceState(DS_InProcess, SS_CheckPAU);
				break;

			case SS_CheckPAU:
				// Обновление состояния PAU
				if(!PAU_UpdateState(&PAU_State))
					CONTROL_SwitchToFault(DF_INTERFACE);
				else
				{
					switch(PAU_State)
					{
						case PS_Ready:
							CONTROL_SetDeviceState(DS_Ready, SS_None);
							break;

						case PS_Fault:
							CONTROL_SwitchToFault(DF_PAU);
							break;

						default:
							if(DataTable[REG_PAU_EMULATED])
								CONTROL_SetDeviceState(DS_Ready, SS_None);
							else
								CONTROL_SwitchToFault(DF_PAU_ABNORMAL_STATE);
							break;
					}
				}
				break;

			case SS_IdleTimeCheck:
				if(CONTROL_TimeCounter > CONTROL_PulseToPulseTime)
					CONTROL_SetDeviceSubState(SS_PAUConfig);
				break;

			case SS_PAUConfig:
				if(LOGIC_PAUConfigProcess())
				{
					CONTROL_Timeout = CONTROL_TimeCounter + DataTable[REG_PS_PREPARE_TIME];
					CONTROL_SetDeviceSubState(SS_CapChargeStop);
				}
				break;

			case SS_CapChargeStop:
				LL_PowerSupply(false);

				if(CONTROL_TimeCounter >= CONTROL_Timeout)
				{
					CONTROL_Timeout = CONTROL_TimeCounter + COMMUTATION_DELAY;
					CONTROL_SetDeviceSubState(SS_CommutationEnable);
				}
				break;

			case SS_CommutationEnable:
				CONTROL_Commutation(TT_DUT, true);

				if(CONTROL_TimeCounter >= CONTROL_Timeout)
					CONTROL_SetDeviceSubState(SS_StartPulse);
				break;

			case SS_StartPulse:
				LOGIC_StartPrepare();
				CONTROL_StartProcess();
				CONTROL_SetDeviceSubState(SS_Pulse);
				break;

			case SS_PostPulseDelayInit:
				CONTROL_Timeout = CONTROL_TimeCounter + POST_PULSE_DELAY;
				CONTROL_SetDeviceSubState(SS_PostPulseDelay);
				break;

			case SS_PostPulseDelay:
				IsImpulse = false;

				if(CONTROL_TimeCounter >= CONTROL_Timeout)
				{
					CONTROL_Timeout = CONTROL_TimeCounter + COMMUTATION_DELAY;
					CONTROL_SetDeviceSubState(SS_CommutationDisable);
				}
				break;

			case SS_CommutationDisable:
				CONTROL_Commutation(TT_DUT, false);

				if(CONTROL_TimeCounter >= CONTROL_Timeout)
				{
					if(CONTROL_State != DS_Fault)
					{
						LL_PowerSupply(true);
						CONTROL_SetDeviceSubState(SS_SaveResult);
					}
					else
						CONTROL_SetDeviceSubState(SS_None);
				}
				break;

			case SS_SaveResult:
				if(!DataTable[REG_PROBLEM])
					CONTROL_SaveTestResult();
				else
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				break;

			default:
				break;
		}
	}

	if(CONTROL_State == DS_SelfTest)
		SELFTEST_Process();
}
//-----------------------------------------------

bool CONTROL_IsSafetyEvent()
{
	return LL_SafetyGetState() && DataTable[REG_SAFETY_ACTIVE];
}
//-----------------------------------------------

void CONTROL_HighPriorityProcess()
{
	MeasureSample Sample;
	RegulatorState RegulatorResult;

	if(CONTROL_SubState == SS_Pulse || CONTROL_SubState == SS_ST_Pulse)
	{
		Sample = MEASURE_Sample();

		RegulatorResult = LOGIC_RegulatorCycle(Sample);

		if(CONTROL_IsSafetyEvent())
		{
			CONTROL_ForceStopProcess(PROBLEM_SAFETY);
			return;
		}
		else if(RegulatorResult == RS_FollowingError)
		{
			DataTable[REG_OP_RESULT] = OPRESULT_FAIL;

			if(Sample.Current >= DISOPAMP_CURRENT_MAX)
				DataTable[REG_PROBLEM] = PROBLEM_OUTPUT_SHORT;
			else
			{
				LOGIC_StopProcess();
				LOGIC_HarwareDefaultState();

				CONTROL_SwitchToFault(DF_FOLLOWING_ERROR);
				return;
			}
		}

		if(RegulatorResult == RS_Finished || RegulatorResult == RS_FollowingError)
			CONTROL_StopProcess();
	}
}
//-----------------------------------------------

void CONTROL_StartProcess()
{
	MEASURE_DMABuffersClear();
	TIM_Start(TIM6);

	IsImpulse = true;
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	LOGIC_StopProcess();
	CONTROL_PulseToPulseTime = CONTROL_TimeCounter + DataTable[REG_AFTER_PULSE_PAUSE];

	LOGIC_HarwareDefaultState();

	if(CONTROL_State == DS_InProcess)
		CONTROL_SetDeviceSubState(SS_PostPulseDelayInit);
	else
		CONTROL_SetDeviceState(DS_SelfTest, SS_ST_PulseCheck);
}
//-----------------------------------------------

void CONTROL_Commutation(TestType Type, bool State)
{
	(Type == TT_SelfTest) ? LL_SelfTestCommutationControl(State) : LL_OutputCommutationControl(State);
}
//-----------------------------------------------

void CONTROL_SaveTestResult()
{
	float Current;
	Int16U PAU_State;
	static Int64U Timeout = 0;

	// Обновление состояния PAU
	if(!PAU_UpdateState(&PAU_State))
		CONTROL_SwitchToFault(DF_INTERFACE);
	else
	{
		switch(PAU_State)
		{
		case PS_Ready:
			if(!PAU_ReadMeasuredData(&Current))
				CONTROL_SwitchToFault(DF_INTERFACE);
			else
			{
				DataTable[REG_RESULT_CURRENT] = Current;
				DataTable[REG_RESULT_VOLTAGE] = MEASURE_GetAverageVoltage();
				DataTable[REG_OP_RESULT] = OPRESULT_OK;

				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			break;

		case PS_InProcess:
			if(!Timeout)
				Timeout = CONTROL_TimeCounter + PAU_WAIT_READY_TIMEOUT;

			if(CONTROL_TimeCounter >= Timeout)
				CONTROL_SwitchToFault(DF_PAU_ABNORMAL_STATE);
			break;

		case PS_Fault:
			CONTROL_SwitchToFault(DF_PAU);
			break;

		default:
			if(DataTable[REG_PAU_EMULATED])
			{
				DataTable[REG_RESULT_CURRENT] = MEASURE_GetAverageCurrent();
				DataTable[REG_RESULT_VOLTAGE] = MEASURE_GetAverageVoltage();
				DataTable[REG_OP_RESULT] = OPRESULT_OK;

				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			else
				CONTROL_SwitchToFault(DF_PAU_ABNORMAL_STATE);
			break;
		}

		if(PAU_State != PS_InProcess)
			Timeout = 0;
	}
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	if(Reason == DF_INTERFACE)
	{
		BHLError Error = BHL_GetError();
		DataTable[REG_PAU_ERROR_CODE] = Error.ErrorCode;
		DataTable[REG_PAU_FUNCTION] = Error.Func;
		DataTable[REG_PAU_EXT_DATA] = Error.ExtData;
	}

	CONTROL_SetDeviceState(DS_Fault, SS_PostPulseDelayInit);
	DataTable[REG_FAULT_REASON] = Reason;
}
//------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;

	CONTROL_SetDeviceSubState(NewSubState);
}
//------------------------------------------

void CONTROL_SetDeviceSubState(DeviceSubState NewSubState)
{
	CONTROL_SubState = NewSubState;
	DataTable[REG_SUB_STATE] = NewSubState;
}
//------------------------------------------

void CONTROL_UpdateWatchDog()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------

