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
volatile Int64U	CONTROL_PowerSupplyDelay = 0;
volatile Int16U CONTROL_VoltageCounter = 0;
volatile Int16U CONTROL_CurrentCounter = 0;
volatile Int16U CONTROL_RegulatorErr_Counter = 0;
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
void CONTROL_ResetOutputRegisters();
void CONTROL_SaveTestResult();
void CONTROL_ClearTestResult();
bool CONTROL_PowerSupplyWaiting(Int16U Time);
void CONTROL_SelfTest();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[EP_COUNT] = {EP_VOLTAGE, EP_CURRENT, EP_REGULATOR_ERR};
	Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[EP_COUNT] = {(pInt16U)&CONTROL_VoltageCounter, (pInt16U)&CONTROL_CurrentCounter, (pInt16U)&CONTROL_RegulatorErr_Counter};
	pFloat32 EPDatas[EP_COUNT] = {(pFloat32)CONTROL_ValuesVoltage, (pFloat32)CONTROL_ValuesCurrent, (pFloat32)CONTROL_RegulatorErr};

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

void CONTROL_ResetOutputRegisters()
{
	DataTable[REG_FAULT_REASON] = DF_NONE;
	DataTable[REG_DISABLE_REASON] = DF_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;
	//
	DataTable[REG_RESULT_VOLTAGE] = 0;
	DataTable[REG_RESULT_CURRENT] = 0;
	//
	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_ResetOutputRegisters();
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
				CONTROL_SetDeviceState(DS_InProcess, SS_PS_WaitingStart);
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_DISABLE_POWER:
			if(CONTROL_State == DS_Ready)
			{
				LL_PowerSupply(false);
				LOGIC_StopProcess();
				CONTROL_SetDeviceState(DS_None, SS_None);
			}
			else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_START_PROCESS:
			if (CONTROL_State == DS_Ready)
			{
				CONTROL_ResetOutputRegisters();
				LOGIC_StartPrepare(TT_DUT);

				CONTROL_SetDeviceState(DS_InProcess, SS_WaitPause);
			}
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
	CONTROL_ResetOutputRegisters();
	LOGIC_HarwareDefaultState();

	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;

	CONTROL_SetDeviceState(DS_Ready, SS_None);

	IsImpulse = false;
}
//-----------------------------------------------

void CONTROL_LogicProcess()
{
	if(CONTROL_State == DS_InProcess)
	{
		switch(CONTROL_SubState)
		{
			case SS_WaitPause:
				if(CONTROL_TimeCounter > CONTROL_PulseToPulseTime)
					CONTROL_SetDeviceState(DS_InProcess, SS_PS_WaitingOff);
				break;

			case SS_PS_WaitingOff:
				if(CONTROL_PowerSupplyWaiting(DataTable[REG_PS_PREPARE_TIME]))
					CONTROL_SetDeviceState(DS_InProcess, SS_StartPulse);
				else
					LL_PowerSupply(false);
				break;

			case SS_StartPulse:
				CONTROL_SetDeviceState(DS_InProcess, SS_Pulse);
				CONTROL_StartProcess();
				break;

			case SS_PS_WaitingStart:
				LL_PowerSupply(true);
				if(CONTROL_PowerSupplyWaiting(DataTable[REG_PS_FIRST_START_TIME]))
				{
					if(DataTable[REG_SELF_TEST_ACTIVE])
						CONTROL_SetDeviceState(DS_SelfTest, SS_None);
					else
						CONTROL_SetDeviceState(DS_Ready, SS_None);
				}
				break;

			case SS_PS_WaitingOn:
				if(CONTROL_PowerSupplyWaiting(DataTable[REG_PS_PREPARE_TIME]))
				{
					LL_PowerSupply(true);
					CONTROL_SetDeviceState(DS_Ready, SS_ST_Sync);
				}
				break;

			default:
				break;
		}
	}

	if(CONTROL_State == DS_SelfTest)
		SELFTEST_Process();
}
//-----------------------------------------------

bool CONTROL_PowerSupplyWaiting(Int16U Time)
{
	if(!CONTROL_PowerSupplyDelay)
		CONTROL_PowerSupplyDelay = CONTROL_TimeCounter + Time;
	else
	{
		if(CONTROL_TimeCounter >= CONTROL_PowerSupplyDelay)
		{
			CONTROL_PowerSupplyDelay = 0;
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
		LOGIC_LoggingProcess(Sample);

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
				CONTROL_SwitchToFault(DF_FOLLOWING_ERROR);
				return;
			}
		}

		if(RegulatorResult == RS_Finished || RegulatorResult == RS_FollowingError)
		{
			CONTROL_StopProcess();
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
	LOGIC_OSCSync(true);
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	LOGIC_StopProcess();
	CONTROL_PulseToPulseTime = CONTROL_TimeCounter + DataTable[REG_AFTER_PULSE_PAUSE];

	LOGIC_HarwareDefaultState();

	if(CONTROL_State == DS_InProcess)
		CONTROL_SetDeviceState(DS_InProcess, SS_PS_WaitingOn);
	else
		CONTROL_SetDeviceState(DS_SelfTest, SS_ST_PulseCheck);

	IsImpulse = false;
}
//-----------------------------------------------

void CONTROL_SaveTestResult()
{
	DataTable[REG_RESULT_VOLTAGE] = LOGIC_GetAverageVoltage();
	DataTable[REG_OP_RESULT] = OPRESULT_OK;
}
//-----------------------------------------------

void CONTROL_ClearTestResult()
{
	DataTable[REG_RESULT_VOLTAGE] = 0;
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_SetDeviceState(DS_Fault, SS_None);
	DataTable[REG_FAULT_REASON] = Reason;
}
//------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState)
{
	CONTROL_State = NewState;
	CONTROL_SubState = NewSubState;
	DataTable[REG_DEV_STATE] = NewState;
	DataTable[REG_SUB_STATE] = NewSubState;
}
//------------------------------------------

void CONTROL_UpdateWatchDog()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------

