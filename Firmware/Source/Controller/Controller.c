// Header
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
Boolean DeviceIsPowered = false;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U	CONTROL_PulseToPulseTime = 0;
volatile Int16U CONTROL_ValuesCounter = 0;
volatile float CONTROL_ValuesVoltage[VALUES_x_SIZE];
volatile float CONTROL_ValuesCurrent[VALUES_x_SIZE];
volatile float CONTROL_RegulatorErr[VALUES_x_SIZE];
volatile Int16U CONTROL_FloatEP_Counter = 0;
//

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_StopProcess();
void CONTROL_SaveTestResult();
bool CONTROL_Delay(Int16U Time);
void CONTROL_SelfTest();
void CONTROL_Commutation(TestType Type, bool State);

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
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);
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
	LOGIC_ResetOutputRegisters();
	DISOPAMP_SetVoltage(0);
	LOGIC_HarwareDefaultState();

	CONTROL_SetDeviceState(DS_None, SS_None);
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
				CONTROL_SetDeviceState(DS_InProcess, SS_StartProcess);
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_DISABLE_POWER:
			if(CONTROL_State == DS_Ready)
			{
				LL_PowerSupply(false);
				DeviceIsPowered = false;

				CONTROL_SetDeviceState(DS_None, SS_None);
			}
			else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_START_PROCESS:
			if (CONTROL_State == DS_Ready)
				CONTROL_SetDeviceState(DS_InProcess, SS_IdleTimeCheck);
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_STOP_PROCESS:
			if (CONTROL_State == DS_InProcess)
				CONTROL_ForceStopProcess();
			break;

		case ACT_START_SELF_TEST:
			if(CONTROL_State == DS_None || CONTROL_State == DS_Ready)
			{
				LOGIC_ResetOutputRegisters();
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_NONE;
				CONTROL_SetDeviceState(DS_InProcess, SS_StartProcess);
			}
			else
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_CLR_FAULT:
			if (CONTROL_State == DS_Fault)
			{
				CONTROL_SetDeviceState(DS_None, SS_None);
				DataTable[REG_FAULT_REASON] = DF_NONE;
			}
			break;

		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = WARNING_NONE;
			break;

		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
			
	}
	return true;
}
//-----------------------------------------------

void CONTROL_ForceStopProcess()
{
	LOGIC_StopProcess();
	LOGIC_ResetOutputRegisters();
	LOGIC_HarwareDefaultState();

	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;

	CONTROL_SetDeviceSubState(SS_PostPulseDelay);
}
//-----------------------------------------------

void CONTROL_LogicProcess()
{
	switch(CONTROL_SubState)
	{
		case SS_StartProcess:
			LL_PowerSupply(true);

			if(DeviceIsPowered || CONTROL_Delay(DataTable[REG_PS_FIRST_START_TIME]))
			{
				DeviceIsPowered = true;

				if(DataTable[REG_SELF_TEST_ACTIVE])
				{
					CONTROL_Commutation(TT_SelfTest, true);
					DELAY_MS(COMMUTATION_DELAY);

					CONTROL_SetDeviceState(DS_SelfTest, SS_ST_StartPrepare);
				}
				else
					CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			break;

		case SS_IdleTimeCheck:
			if(CONTROL_TimeCounter > CONTROL_PulseToPulseTime)
				CONTROL_SetDeviceSubState(SS_CapChargeStop);
			break;

		case SS_CapChargeStop:
			LL_PowerSupply(false);

			if(CONTROL_Delay(DataTable[REG_PS_PREPARE_TIME]))
				CONTROL_SetDeviceSubState(SS_CommutationEnable);
			break;

		case SS_CommutationEnable:
			CONTROL_Commutation(TT_DUT, true);

			if(CONTROL_Delay(COMMUTATION_DELAY))
				CONTROL_SetDeviceSubState(SS_StartPulse);
			break;

		case SS_StartPulse:
			LOGIC_ResetOutputRegisters();
			LOGIC_StartPrepare();
			CONTROL_StartProcess();
			CONTROL_SetDeviceSubState(SS_Pulse);
			break;

		case SS_PostPulseDelay:
			IsImpulse = false;

			DELAY_US(POST_PULSE_DELAY);
			CONTROL_SetDeviceSubState(SS_CommutationDisable);
			break;

		case SS_CommutationDisable:
			CONTROL_Commutation(TT_DUT, false);

			if(CONTROL_Delay(COMMUTATION_DELAY))
			{
				if(CONTROL_State != DS_Fault)
				{
					LL_PowerSupply(true);
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				}
				else
					CONTROL_SetDeviceSubState(SS_None);
			}
			break;

		default:
			break;
	}

	if(CONTROL_State == DS_SelfTest)
		SELFTEST_Process();
}
//-----------------------------------------------

bool CONTROL_Delay(Int16U Time)
{
	static Int64U CONTROL_Delay = 0;

	if(!CONTROL_Delay)
		CONTROL_Delay = CONTROL_TimeCounter + Time;
	else
	{
		if(CONTROL_TimeCounter >= CONTROL_Delay)
		{
			CONTROL_Delay = 0;
			return true;
		}
	}

	return false;
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

		if(LL_SafetyGetState() && DataTable[REG_SAFETY_ACTIVE])
		{
			CONTROL_ForceStopProcess();
			DataTable[REG_WARNING] = WARNING_SAFETY;

			return;
		}
		else if(RegulatorResult == RS_FollowingError)
		{
			if(Sample.Current >= DISOPAMP_CURRENT_MAX)
				DataTable[REG_WARNING] = WARNING_OUTPUT_SHORT;
			else
			{
				LOGIC_StopProcess();
				LOGIC_HarwareDefaultState();

				CONTROL_SwitchToFault(DF_FOLLOWING_ERROR);
				return;
			}
		}

		if(RegulatorResult == RS_Finished || RegulatorResult == RS_FollowingError)
		{
			CONTROL_StopProcess();

			if(CONTROL_State == DS_InProcess)
				CONTROL_SaveTestResult();
		}
	}
}
//-----------------------------------------------

void CONTROL_StartProcess()
{
	MEASURE_DMABuffersClear();
	TIM_Start(TIM6);

	IsImpulse = true;
	LL_OscSyncSetState(true);
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	LOGIC_StopProcess();
	CONTROL_PulseToPulseTime = CONTROL_TimeCounter + DataTable[REG_AFTER_PULSE_PAUSE];

	LOGIC_HarwareDefaultState();

	if(CONTROL_State == DS_InProcess)
		CONTROL_SetDeviceSubState(SS_PostPulseDelay);
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
	DataTable[REG_RESULT_VOLTAGE] = MEASURE_GetAverageVoltage();
	DataTable[REG_RESULT_CURRENT] = MEASURE_GetAverageCurrent();
	DataTable[REG_OP_RESULT] = OPRESULT_OK;
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_SetDeviceState(DS_Fault, SS_PostPulseDelay);
	DataTable[REG_FAULT_REASON] = Reason;

	DeviceIsPowered = false;
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

