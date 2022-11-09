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
		case SS_ST_StartPrepare:
			DataTable[REG_TEST_VOLTAGE] = 0;
			CONTROL_SetDeviceState(DS_SelfTest, SS_ST_StartPulse);
			break;

		case SS_ST_StartPulse:
			LL_PowerSupply(false);
			DELAY_MS(DataTable[REG_PS_PREPARE_TIME]);

			if(!DataTable[REG_TEST_VOLTAGE])
				DataTable[REG_TEST_VOLTAGE] = DataTable[REG_VOLTAGE_RANGE_THRESHOLD];
			else
				DataTable[REG_TEST_VOLTAGE] = LCTU_VOLTAGE_MAX;

			LOGIC_StartPrepare();

			CONTROL_SetDeviceState(DS_SelfTest, SS_ST_Pulse);
			CONTROL_StartProcess();
			break;

		case SS_ST_PulseCheck:
			IsImpulse = false;

			float MeasuredCurrent = MEASURE_GetAverageCurrent();
			float MeasuredVoltage = MEASURE_GetAverageVoltage();

			Current = MeasuredVoltage / ST_LOAD_RESISTANCE;
			Err = fabsf((Current - MeasuredCurrent) / MeasuredCurrent * 100);

			if(Err >= ST_ALOWED_ERROR || !MeasuredCurrent)
			{
				DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_FAIL;
				CONTROL_SwitchToFault(DF_CURRENT_MEASURING);
			}
			else
				if(DataTable[REG_TEST_VOLTAGE] < LCTU_VOLTAGE_MAX)
					CONTROL_SetDeviceState(DS_SelfTest, SS_ST_WaitPause);
				else
				{
					LL_PowerSupply(true);
					LL_SelfTestCommutationControl(false);

					DataTable[REG_SELF_TEST_OP_RESULT] = OPRESULT_OK;
					CONTROL_SetDeviceState(DS_InProcess, SS_CheckPAU);
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

