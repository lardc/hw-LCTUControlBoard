#ifndef LOGIC_H_
#define LOGIC_H_

// Includes
#include "Measurement.h"

typedef enum __TestType
{
	TT_DUT = 0,
	TT_SelfTest
} TestType;

typedef enum __RegulatorState
{
	RS_InProcess = 1,
	RS_Finished = 2,
	RS_FollowingError = 3,
	RS_PAU_Sync = 4
} RegulatorState;

// Variables
extern float VoltageTarget;
extern float CurrentCutOff;
extern bool IsImpulse;
extern volatile Int16U RingBufferIndex;
extern Int16U PAUSyncReceiveCounter;
//
extern float RingBuffer_Voltage[MAF_BUFFER_LENGTH];
extern float RingBuffer_Current[MAF_BUFFER_LENGTH];

// Functions
RegulatorState LOGIC_RegulatorCycle(MeasureSample Sample);
void LOGIC_StopProcess();
void LOGIC_StartPrepare();
void LOGIC_LoggingProcess(MeasureSample Sample, float RegulatorErr);
void LOGIC_SetVolatgeRange();
void LOGIC_HandleFan(bool IsImpulse);
void CONTROL_HandleExternalLamp(bool IsImpulse);
void LOGIC_HarwarePrepare(TestType Type);
void LOGIC_HarwareDefaultState();
void LOGIC_ResetOutputRegisters();

#endif /* LOGIC_H_ */
