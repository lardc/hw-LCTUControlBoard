// Header
#include "Diagnostic.h"

// Includes
#include "DataTable.h"
#include "LowLevel.h"
#include "Controller.h"
#include "DebugActions.h"

// Functions
bool DIAG_HandleDiagnosticAction(uint16_t ActionID, uint16_t *pUserError)
{
	switch (ActionID)
	{
		case ACT_DBG_OPAMP_SET_VOLTAGE:
				DBGACT_OpAmpSetVoltage(DataTable[REG_DBG]);
			break;

		case ACT_DBG_POWER_SUPPLY:
			DBGACT_PowerSupplyControl(DataTable[REG_DBG]);
			break;

		case ACT_DBG_OSC_PULSE_SYNC:
			DBGACT_GenerateOscSync();
			break;

		case ACT_DBG_PAU_PULSE_SYNC:
			DBGACT_GeneratePAUSync();
			break;

		case ACT_DBG_VOLTAGE_RANGE_SET:
			DBGACT_VoltageRangeSet(DataTable[REG_DBG]);
			break;

		case ACT_DBG_PAU_SHUNT:
			DBGACT_PAU_Shunting(DataTable[REG_DBG]);
			break;

		case ACT_DBG_STST_COMM:
			DBGACT_SelfTestCommutationControl(DataTable[REG_DBG]);
			break;

		case ACT_DBG_OUTPUT_COMM:
			DBGACT_OutputCommutationControl(DataTable[REG_DBG]);
			break;

		case ACT_DBG_FAN_CTRL:
			DBGACT_FanControl(DataTable[REG_DBG]);
			break;

		case ACT_DBG_IND_CTRL:
			DBGACT_ExtIndicationControl(DataTable[REG_DBG]);
			break;

		default:
			return false;
	}

	return true;
}
