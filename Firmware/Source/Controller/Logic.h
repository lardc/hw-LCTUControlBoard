﻿#ifndef LOGIC_H_
#define LOGIC_H_

// Includes
#include "Measurement.h"

// Variables
extern float VoltageTarget;
extern float CurrentCutOff;
extern bool IsImpulse;

// Functions
bool LOGIC_RegulatorCycle(MeasureSample Sample, Int16U* Fault);
void LOGIC_StopProcess();
void LOGIC_StartPrepare();
void LOGIC_LoggingProcess(MeasureSample Sample);
float LOGIC_GetAverageVoltage();
float LOGIC_GetLastSampledVoltage();
void LOGIC_SetVolatgeRange();
void LOGIC_HandleFan(bool IsImpulse);
void CONTROL_HandleExternalLamp(bool IsImpulse);
void LOGIC_HarwarePrepare();
void LOGIC_HarwareDefaultState();
void LOGIC_OSCSync(bool State);

#endif /* LOGIC_H_ */
