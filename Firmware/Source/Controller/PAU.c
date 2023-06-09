// Header
//
#include "PAU.h"

// Includes
//
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "BCCIMHighLevel.h"

// Definitions
//
#define LCTU_SAMPLE_NUMBERS_DEF		1


// Functions
//
bool PAU_UpdateState(Int16U* Register)
{
	if(DataTable[REG_PAU_EMULATED])
		return true;

	if(BHL_ReadRegister(DataTable[REG_PAU_CAN_ID], REG_PAU_DEV_STATE, Register))
		return true;

	return false;
}
//--------------------------------------

bool PAU_Configure(Int16U Channel, float Range)
{
	if(DataTable[REG_PAU_EMULATED])
		return true;

	if(BHL_WriteRegister(DataTable[REG_PAU_CAN_ID], REG_PAU_CHANNEL, Channel))
		if(BHL_WriteRegisterFloat(DataTable[REG_PAU_CAN_ID], REG_PAU_RANGE, Range))
			if(BHL_WriteRegisterFloat(DataTable[REG_PAU_CAN_ID], REG_SAMPLES_NUMBER, LCTU_SAMPLE_NUMBERS_DEF))
				BHL_Call(DataTable[REG_PAU_CAN_ID], ACT_PAU_PULSE_CONFIG);
					return true;

	return false;
}
//--------------------------------------

bool PAU_ClearFault()
{
	return (DataTable[REG_PAU_EMULATED]) ? true : BHL_Call(DataTable[REG_PAU_CAN_ID], ACT_PAU_FAULT_CLEAR);
}
//--------------------------------------

bool PAU_ClearWarning()
{
	return (DataTable[REG_PAU_EMULATED]) ? true : BHL_Call(DataTable[REG_PAU_CAN_ID], ACT_PAU_WARNING_CLEAR);
}
//--------------------------------------

bool PAU_ReadMeasuredData(float* Data)
{
	return (DataTable[REG_PAU_EMULATED]) ? true : BHL_ReadRegisterFloat(DataTable[REG_PAU_CAN_ID], REG_PAU_RESULT_CURRENT, Data);
}
//--------------------------------------

