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
	{"0. Current Channel 0"},
	{"1. Current Channel 1"},
	{"2. Current Channel 2"},
	{"3. Current Channel 3"},
	{"4. Current Channel 4"},
	{"5. Current Channel 5"},
	{"6. Current Channel 6"},
	{"7. Current Channel 7"},
	{"8. Potential entry relay"},
	{"9. Self test relay"},
	{"10. Test load relay"},
	{"11. Polarity switch"},
};
CounterData CounterTablePointers[sizeof(CounterStorageDescription) / sizeof(CounterStorageDescription[0])] = {0};
const Int16U CounterStorageSize = sizeof(CounterStorageDescription) / sizeof(CounterStorageDescription[0]);

