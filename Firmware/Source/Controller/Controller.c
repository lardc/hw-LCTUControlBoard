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

// Definitions
//
#define PS_FULL_CHARGE_TIME			2000	// ms
#define PS_RECHARGE_TIME			50		// ms

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
volatile Int16U CONTROL_ValuesVoltage[VALUES_x_SIZE];
volatile Int16U CONTROL_ValuesCurrent[VALUES_x_SIZE];
volatile Int16U CONTROL_RegulatorErr[VALUES_x_SIZE];
//

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_StopProcess();
void CONTROL_StartProcess();
void CONTROL_ResetOutputRegisters();
void CONTROL_SaveTestResult();
void CONTROL_ClearTestResult();
bool CONTROL_PowerSupplyWaiting(Int16U Time);

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[EP_COUNT] = {EP_VOLTAGE, EP_CURRENT, EP_REGULATOR_ERR};
	Int16U EPSized[EP_COUNT] = {VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};
	pInt16U EPCounters[EP_COUNT] = {(pInt16U)&CONTROL_VoltageCounter, (pInt16U)&CONTROL_CurrentCounter, (pInt16U)&CONTROL_RegulatorErr_Counter};
	pInt16U EPDatas[EP_COUNT] = {(pInt16U)CONTROL_ValuesVoltage, (pInt16U)CONTROL_ValuesCurrent, (pInt16U)CONTROL_RegulatorErr};

	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
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
				LOGIC_StartPrepare();

				CONTROL_SetDeviceState(DS_InProcess, SS_WaitPulseToPulseTime);
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_STOP_PROCESS:
			if (CONTROL_State == DS_InProcess)
			{
				LOGIC_StopProcess();
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
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

void CONTROL_LogicProcess()
{
	if(CONTROL_State == DS_InProcess)
	{
		switch(CONTROL_SubState)
		{
			case SS_WaitPulseToPulseTime:
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
				CONTROL_PowerSupplyWaiting(DataTable[REG_PS_FIRST_START_TIME]);
			case SS_PS_WaitingOn:
				LL_PowerSupply(true);

				if(CONTROL_PowerSupplyWaiting(DataTable[REG_PS_PREPARE_TIME]))
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				break;

			default:
				break;
		}
	}
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
	Int16U Fault = 0;

	if(CONTROL_SubState == SS_Pulse)
	{
		Sample = MEASURE_Sample();
		LOGIC_LoggingProcess(Sample);

		if(LOGIC_RegulatorCycle(Sample, &Fault))
		{
			CONTROL_StopProcess();

			if(Fault != DF_NONE)
				CONTROL_SwitchToFault(Fault);
			else
			{
				CONTROL_SaveTestResult();
				CONTROL_SetDeviceState(DS_InProcess, SS_PS_WaitingOn);
			}
		}
	}
}
//-----------------------------------------------

void CONTROL_StartProcess()
{
	MEASURE_DMABuffersClear();
	TIM_Start(TIM6);
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	LOGIC_StopProcess();
	CONTROL_PulseToPulseTime = CONTROL_TimeCounter + DataTable[REG_AFTER_PULSE_PAUSE];
}
//-----------------------------------------------

void CONTROL_SaveTestResult()
{
	DataTable[REG_RESULT_VOLTAGE] = (Int16U)(LOGIC_GetAverageVoltage() * 10);
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

void CONTROL_DelayMs(uint32_t Delay)
{
	uint64_t Counter = (uint64_t)CONTROL_TimeCounter + Delay;
	while(Counter > CONTROL_TimeCounter)
		CONTROL_UpdateWatchDog();
}
//------------------------------------------

void CONTROL_UpdateWatchDog()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------

