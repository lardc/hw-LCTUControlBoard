// Header
#include "StorageDescription.h"

RecordDescription StorageDescription[] =
{
	{"REG_DEV_STATE",			DT_Float, 1},
	{"REG_FAULT_REASON",		DT_Float, 1},
	{"REG_DISABLE_REASON",		DT_Float, 1},
	{"REG_WARNING",				DT_Float, 1},
	{"REG_PROBLEM",				DT_Float, 1},
	{"REG_OP_RESULT",			DT_Float, 1},
	{"REG_DEV_SUBSTATE",		DT_Float, 1},

	{"REG_ICES_RESULT",			DT_Float, 1},

	{"REG_DIAG_CURRENT",		DT_Float, 1},
	{"REG_DIAG_VOLTAGE",		DT_Float, 1},
	{"REG_SELFTEST_STEP",		DT_Float, 1},
	{"REG_DEBUG_SCALING_COEF",	DT_Float, 1},
};

Int32U TablePointers[sizeof(StorageDescription) / sizeof(StorageDescription[0])] = {0};
const Int16U StorageSize = sizeof(StorageDescription) / sizeof(StorageDescription[0]);

const CounterDescription CounterStorageDescription[] =
{
	{"0. Relay Rcon"},
	{"1. Relay RoutLCAU"},
	{"2. Relay Rdis"},
	{"3. Relay RoutLCTU"},
	{"4. Relay Rst1"},
	{"5. Relay Rst2"},
	{"6. Relay Rmes1"},
	{"7. Relay Rmes2"},
	{"8. Relay Rmes3"},
	{"9. Relay Rmes4"},
	{"10. Relay Rmes5"},
};
CounterData CounterTablePointers[sizeof(CounterStorageDescription) / sizeof(CounterStorageDescription[0])] = {0};
const Int16U CounterStorageSize = sizeof(CounterStorageDescription) / sizeof(CounterStorageDescription[0]);
