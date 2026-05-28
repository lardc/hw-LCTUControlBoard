// Header
#include "StorageDescription.h"
#include "Global.h"

// Variables
const RecordDescription StorageDescription[] = {0};

#define SIZE_OF_POINTERS (sizeof(StorageDescription) / sizeof(StorageDescription[0]))
Int32U TablePointers[SIZE_OF_POINTERS ? SIZE_OF_POINTERS : 1] = {0};
const Int16U StorageSize = SIZE_OF_POINTERS;

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

