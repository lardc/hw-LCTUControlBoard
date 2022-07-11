#ifndef LOGIC_H_
#define LOGIC_H_

// Includes
#include "Measurement.h"

// Variables
extern float VoltageTarget;
extern float CurrentCutOff;

// Functions
bool LOGIC_RegulatorCycle(float Voltage, Int16U* Fault);
void LOGIC_StopProcess();
void LOGIC_StartPrepare();
void LOGIC_LoggingProcess(MeasureSample Sample);
float LOGIC_GetAverageVoltage();
float LOGIC_GetLastSampledVoltage();
void LOGIC_SetVolatgeRange();

#endif /* LOGIC_H_ */
