// Header
//
#include "SelfTest.h"

// Includes
//
#include "Math.h"
#include "Logic.h"
#include "Controller.h"
#include "DataTable.h"
#include "Delay.h"
#include "LowLevel.h"

// Definitions
//
#define ST_VOLTAGE_POINTS			5
#define ST_LOAD_RESISTANCE			105		// êÎì
#define ST_ALOWED_ERROR				10		// %

// Functions
//
void SELFTEST_Process()
{
	float Current, Err;

	switch(CONTROL_SubState)
	{
	case SS_ST_Sync:
		LOGIC_PAUSync(true);
		LOGIC_PAUSync(true);
		LOGIC_PAUSync(false);
		LOGIC_PAUSync(false);

		DataTable[REG_VOLTAGE_SETPOINT] = 0;

		CONTROL_SetDeviceState(DS_SelfTest, SS_ST_StartPulse);
		break;

	case SS_ST_StartPulse:
		LL_PowerSupply(false);
		DELAY_MS(DataTable[REG_PS_PREPARE_TIME]);

		DataTable[REG_VOLTAGE_SETPOINT] += LCTU_VOLTAGE_MAX / ST_VOLTAGE_POINTS;
		if(DataTable[REG_VOLTAGE_SETPOINT] > LCTU_VOLTAGE_MAX)
			DataTable[REG_VOLTAGE_SETPOINT] = LCTU_VOLTAGE_MAX;

		LOGIC_StartPrepare(TT_SelfTest);

		CONTROL_SetDeviceState(DS_SelfTest, SS_ST_Pulse);
		CONTROL_StartProcess();
		break;

	case SS_ST_PulseCheck:
		Current = DataTable[REG_RESULT_VOLTAGE] / 10 / ST_LOAD_RESISTANCE * 1000;
		Err = fabsf((Current - DataTable[REG_RESULT_CURRENT]) / DataTable[REG_RESULT_CURRENT] * 100);

		if(Err >= ST_ALOWED_ERROR)
			CONTROL_SwitchToFault(DF_CURRENT_MEASURING);
		else
			if(DataTable[REG_VOLTAGE_SETPOINT] < LCTU_VOLTAGE_MAX)
				CONTROL_SetDeviceState(DS_SelfTest, SS_ST_WaitPause);
			else
			{
				LL_PowerSupply(true);
				LL_SelfTestCommutationControl(false);

				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
		break;

	case SS_ST_WaitPause:
		LL_PowerSupply(true);
		if(CONTROL_TimeCounter > CONTROL_PulseToPulseTime)
			CONTROL_SetDeviceState(DS_SelfTest, SS_ST_StartPulse);

	default:
		break;
	}
}
//-----------------------------------------------

