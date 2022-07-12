#ifndef LOGIC_H_
#define LOGIC_H_

// Includes
#include "Measurement.h"

typedef enum __TestType
{
	TT_DUT = 0,
	TT_SelfTest = 1
} TestType;

// Variables
extern float VoltageTarget;
extern float CurrentCutOff;
extern bool IsImpulse;

// Functions
bool LOGIC_RegulatorCycle(MeasureSample Sample, Int16U* Fault);
void LOGIC_StopProcess();
void LOGIC_StartPrepare(TestType Type);
void LOGIC_LoggingProcess(MeasureSample Sample);
float LOGIC_GetAverageVoltage();
float LOGIC_GetLastSampledVoltage();
void LOGIC_SetVolatgeRange();
void LOGIC_HandleFan(bool IsImpulse);
void CONTROL_HandleExternalLamp(bool IsImpulse);
void LOGIC_HarwarePrepare(TestType Type);
void LOGIC_HarwareDefaultState();
void LOGIC_OSCSync(bool State, Int16U CheckDelay);
void LOGIC_PAUSync(bool State, Int16U CheckDelay);

#endif /* LOGIC_H_ */
