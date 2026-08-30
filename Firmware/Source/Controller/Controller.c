// Header
#include "Controller.h"
//

// Includes
#include "Board.h"
#include "Delay.h"
#include "DataTable.h"
#include "DeviceProfile.h"
#include "Global.h"
#include "LowLevel.h"
#include "SysConfig.h"
#include "math.h"
#include "BCCIxParams.h"
#include "InitConfig.h"
#include "Diagnostic.h"
#include "Logic.h"
#include "JSONDescription.h"
#include "SaveToFlash.h"
#include "Constraints.h"

// Defines
//
#define CT_SAVE_TIMEOUT		1800000 // в мс

// Types
//
typedef void (*FUNC_AsyncDelegate)();
//

// Variables
//
volatile DeviceState CONTROL_State = DS_None;
volatile DeviceSubState CONTROL_SubState = SS_None;
static Boolean CycleActive = false;
volatile MeasureType CONTROL_MeasureType = MT_Ices;

volatile Int64U CONTROL_TimeCounter = 0;
static Int64U CT_SaveTimer = 0;					 // Последняя отметка времени автосохранения
volatile Boolean RequestSaveToFlash = FALSE;
static Boolean PendingAutoSelfTest = false;

volatile Int16U CONTROL_ExtInfoCounter = 0;
volatile float CONTROL_ExtInfoData[VALUES_EXT_INFO_SIZE] = {0};

Int16U CONTROL_Values_Counter = 0;
float CONTROL_RegulatorIces[VALUES_DEBUG_RGLTR_SIZE] = {0};
float CONTROL_RegulatorUce[VALUES_DEBUG_RGLTR_SIZE] = {0};
float CONTROL_RegulatorSetpoint[VALUES_DEBUG_RGLTR_SIZE] = {0};
float CONTROL_RegulatorCorrection[VALUES_DEBUG_RGLTR_SIZE] = {0};
float CONTROL_RegulatorError[VALUES_DEBUG_RGLTR_SIZE] = {0};
float CONTROL_DACRaw[VALUES_DEBUG_RGLTR_SIZE] = {0};


// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SwitchToFault(Int16U Reason);
void Delay_mS(uint32_t Delay);
void CONTROL_WatchDogUpdate();
void CONTROL_ResetToDefaultState();
void CONTROL_ResetData();
bool CONTROL_IsSafetyOk();
void CONTROL_InitStoragePointers();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U EPIndexes[FEP_COUNT] = {EP16_ExtInfoData, EP16_RegulatorUg, EP16_RegulatorIg,
			EP16_RegulatorSetpoint, EP16_RegulatorCorrection, EP16_RegulatorError, EP16_DACRaw};

	Int16U EPSized[FEP_COUNT] = {VALUES_EXT_INFO_SIZE, VALUES_DEBUG_RGLTR_SIZE, VALUES_DEBUG_RGLTR_SIZE,
			VALUES_DEBUG_RGLTR_SIZE, VALUES_DEBUG_RGLTR_SIZE, VALUES_DEBUG_RGLTR_SIZE, VALUES_DEBUG_RGLTR_SIZE};

	pInt16U EPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_ExtInfoCounter, (pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter,
		(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter};

	pFloat32 EPDatas[FEP_COUNT] = {(pFloat32)CONTROL_ExtInfoData, (pFloat32)CONTROL_RegulatorUce, (pFloat32)CONTROL_RegulatorIces,
		(pFloat32)CONTROL_RegulatorSetpoint, (pFloat32)CONTROL_RegulatorCorrection, (pFloat32)CONTROL_RegulatorError, (pFloat32)CONTROL_DACRaw};
	// Конфигурация сервиса работы DataTable и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация DataTable
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);
	// Инициализация функций связанных с CAN NodeID
	Int16U NodeID = DataTable[REG_CFG_NODE_ID] ? DataTable[REG_CFG_NODE_ID] : CAN_SLAVE_NID;
	DT_SaveFirmwareInfo(NodeID, 0);
	INITCFG_ConfigCAN(NodeID);
	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive, NodeID);
	DEVPROFILE_InitFEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	// Инициализация указателей на счетчики и сами счетчики
	CONTROL_InitStoragePointers();
	STF_LoadCounters();


	CONTROL_ResetToDefaultState();

	if(DataTable[REG_USE_SELFTEST] == YES)
	{
		PendingAutoSelfTest = true;
		CONTROL_SetDeviceState(DS_InProcess);
		CONTROL_SetDeviceSubState(SS_Activation);
	}
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_ResetData();
	CONTROL_SetDeviceState(DS_None);
	CONTROL_SetDeviceSubState(SS_None);
}
//------------------------------------------

void CONTROL_ResetData()
{
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;
	
	DataTable[REG_ICES_RESULT] = 0;
	DataTable[REG_DIAG_CURRENT] = 0;
	DataTable[REG_DIAG_VOLTAGE] = 0;
	DataTable[REG_SELFTEST_STEP] = 0;

	CONTROL_Values_Counter = 0;

	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//------------------------------------------

void CONTROL_Idle()
{
	//Обработка логики мастер-команд
	LOGIC_HandleMeasurement();

	if(PendingAutoSelfTest && CONTROL_State == DS_Ready)
	{
		PendingAutoSelfTest = false;
		CONTROL_StartMeasure(MT_ST_TestLoad);
	}

	if(CONTROL_State != DS_InProcess)
	{
		if(RequestSaveToFlash)
		{
			RequestSaveToFlash = FALSE;
			STF_SaveDiagData();
		}
		else if(DataTable[REG_CNT_ACTIVE] && (CONTROL_TimeCounter - CT_SaveTimer) >= CT_SAVE_TIMEOUT)
		{
			STF_SaveCounterData();
			CT_SaveTimer = CONTROL_TimeCounter;
		}
	}

	DEVPROFILE_ProcessRequests();
	CONTROL_WatchDogUpdate();
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
				CONTROL_SetDeviceState(DS_InProcess);
				CONTROL_SetDeviceSubState(SS_Activation);
			}
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;
			
		case ACT_DISABLE_POWER:
			if(CONTROL_State == DS_Ready)
			{
				LOGIC_Deactivate();
				CONTROL_SetDeviceState(DS_None);
				CONTROL_SetDeviceSubState(SS_None);
			}
			else if(CONTROL_State != DS_None)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;
			
		case ACT_FAULT_CLEAR:
			if(CONTROL_State == DS_Fault)
			{
				CONTROL_SetDeviceState(DS_None);
				DataTable[REG_FAULT_REASON] = DF_NONE;
			}
			break;
			
		case ACT_WARNING_CLEAR:
			DataTable[REG_WARNING] = 0;
			break;
			
		case ACT_START_MEASURE_ICES:
			if(CONTROL_State == DS_Ready)
				CONTROL_StartMeasure(MT_Ices);
			else
				*pUserError = ERR_DEVICE_NOT_READY;
			break;

		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
	}
	
	return true;
}
//------------------------------------------

void CONTROL_StartMeasure(MeasureType Type)
{
	CONTROL_MeasureType = Type;
	CONTROL_ResetData();
	if(CONTROL_IsSafetyOk())
	{
		CONTROL_SetDeviceState(DS_InProcess);
		CONTROL_SetDeviceSubState(SS_Init);
	}
}
//------------------------------------------

bool CONTROL_IsSafetyOk()
{
	if(LL_IsSafetyOk() || DataTable[REG_SAFETY_MUTE])
		return true;
	else
	{
		if(CONTROL_State == DS_InProcess)
		{
			LOGIC_StopProcess();
			LL_SetStateRelay(RELAY_HV_OUT, false);
			LL_SetStateRelay(RELAY_LCAU_HV_OUT, false);

			CONTROL_SetDeviceState(DS_Ready);
			CONTROL_SetDeviceSubState(SS_None);

			DataTable[REG_PROBLEM] = PROBLEM_SAFETY;
			DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
		}
		return false;
	}
}
//-----------------------------------------------

void CONTROL_InitJSONPointers()
{
	UsetMin = USET_MININAL;
	UsetMax = USET_MAXIMUM;

	Imeas0Min = DataTable[REG_RANGE_I_0] * 1000.0f;
	Imeas0Max = IMEAS_HIGHEST;

	Imeas1Min = DataTable[REG_RANGE_I_1] * 1000.0f;
	Imeas1Max = DataTable[REG_RANGE_I_0] * 1000.0f;

	Imeas2Min = DataTable[REG_RANGE_I_2] * 1000.0f;
	Imeas2Max = DataTable[REG_RANGE_I_1] * 1000.0f;

	Imeas3Min = DataTable[REG_RANGE_I_3] * 1000000.0f;
	Imeas3Max = DataTable[REG_RANGE_I_2] * 1000000.0f;

	Imeas4Min = DataTable[REG_RANGE_I_4] * 1000000.0f;
	Imeas4Max = DataTable[REG_RANGE_I_3] * 1000000.0f;

	JSON_AssignPointer(0, &UsetMin);
	JSON_AssignPointer(1, &UsetMax);

	JSON_AssignPointer(2, &Imeas0Min);
	JSON_AssignPointer(3, &Imeas0Max);

	JSON_AssignPointer(4, &Imeas1Min);
	JSON_AssignPointer(5, &Imeas1Max);

	JSON_AssignPointer(6, &Imeas2Min);
	JSON_AssignPointer(7, &Imeas2Max);

	JSON_AssignPointer(8, &Imeas3Min);
	JSON_AssignPointer(9, &Imeas3Max);

	JSON_AssignPointer(10, &Imeas4Min);
	JSON_AssignPointer(11, &Imeas4Max);
}
//------------------------------------------

void CONTROL_InitStoragePointers()
{
	for (Int16U i = 0; i < COMMUTATION_TABLE_SIZE; ++i)
		STF_AssignCounterPointer(i, (Int32U)&CycleCounters[i]);

	STF_AssignPointer(0, (Int32U)&DataTable[REG_DEV_STATE]);
	STF_AssignPointer(1, (Int32U)&DataTable[REG_FAULT_REASON]);
	STF_AssignPointer(2, (Int32U)&DataTable[REG_DISABLE_REASON]);
	STF_AssignPointer(3, (Int32U)&DataTable[REG_WARNING]);
	STF_AssignPointer(4, (Int32U)&DataTable[REG_PROBLEM]);
	STF_AssignPointer(5, (Int32U)&DataTable[REG_OP_RESULT]);
	STF_AssignPointer(6, (Int32U)&DataTable[REG_DEV_SUBSTATE]);
	STF_AssignPointer(7, (Int32U)&DataTable[REG_ICES_RESULT]);
	STF_AssignPointer(8, (Int32U)&DataTable[REG_DIAG_CURRENT]);
	STF_AssignPointer(9, (Int32U)&DataTable[REG_DIAG_VOLTAGE]);
	STF_AssignPointer(10, (Int32U)&DataTable[REG_SELFTEST_STEP]);
	STF_AssignPointer(11, (Int32U)&DataTable[REG_DEBUG_SCALING_COEF]);
	STF_AssignPointer(12, (Int32U)CONTROL_RegulatorIces);
	STF_AssignPointer(13, (Int32U)CONTROL_RegulatorUce);
	STF_AssignPointer(14, (Int32U)CONTROL_RegulatorSetpoint);
	STF_AssignPointer(15, (Int32U)CONTROL_RegulatorCorrection);
	STF_AssignPointer(16, (Int32U)CONTROL_RegulatorError);
	STF_AssignPointer(17, (Int32U)CONTROL_DACRaw);
}
//------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_SetDeviceSubState(SS_None);
	CONTROL_SetDeviceState(DS_Fault);
	DataTable[REG_FAULT_REASON] = Reason;
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
}
//------------------------------------------

void CONTROL_SwitchToProblem(Int16U Reason)
{
	CONTROL_SetDeviceSubState(SS_None);
	CONTROL_SetDeviceState(DS_Ready);
	DataTable[REG_PROBLEM] = Reason;
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
}
//------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState)
{
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
}
//------------------------------------------

void CONTROL_SetDeviceSubState(DeviceSubState NewSubState)
{
	CONTROL_SubState = NewSubState;
	DataTable[REG_DEV_SUBSTATE] = NewSubState;
}
//------------------------------------------

void Delay_mS(uint32_t Delay)
{
	uint64_t Counter = (uint64_t)CONTROL_TimeCounter + Delay;
	while(Counter > CONTROL_TimeCounter)
		CONTROL_WatchDogUpdate();
}
//------------------------------------------

void CONTROL_WatchDogUpdate()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------
