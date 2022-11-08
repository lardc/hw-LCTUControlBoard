// Header
//
#include "PAU.h"

// Includes
//
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "BCCIMHighLevel.h"


// Functions
//
bool PAU_UpdateState(Int16U* Register)
{
	Int16U Warning;

	if(DataTable[REG_PAU_EMULATED])
		return true;

	if(BHL_ReadRegister(DataTable[REG_PAU_CAN_ID], REG_PAU_DEV_STATE, Register))
	{
		if(BHL_ReadRegister(DataTable[REG_PAU_CAN_ID], REG_PAU_WARNING, &Warning))
		{
			if(Warning)
				DataTable[REG_WARNING] = WARNING_PAU;

			return true;
		}
	}

	return false;
}
//--------------------------------------

bool PAU_Configure(Int16U Channel, float Range, float Time)
{
	if(DataTable[REG_PAU_EMULATED])
		return true;

	if(BHL_WriteRegister(DataTable[REG_PAU_CAN_ID], REG_PAU_CHANNEL, Channel))
		if(BHL_WriteRegisterFloat(DataTable[REG_PAU_CAN_ID], REG_PAU_RANGE, Range))
			if(BHL_WriteRegisterFloat(DataTable[REG_PAU_CAN_ID], REG_PAU_MEASUREMENT_TIME, Time))
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

