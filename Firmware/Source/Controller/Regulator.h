#ifndef __REGULATOR_H
#define __REGULATOR_H

// Include
#include "stdinc.h"


// Macro
//
#define ABS(a)				(((a) < 0) ? -(a) : (a))

// Types
//
typedef enum __RegulatorState
{
	RS_None 			= 0,
	RS_Rise 			= 1,
	RS_FlatTop 			= 2,
} RegulatorState;

typedef struct __SamplingResult
{
	float Uce, Ices;
} SamplingResult;

// Variables
//
extern volatile SamplingResult Sample;
extern volatile bool IsMeasureOk;

extern Int16U REGLTR_MemBuffUce[];
extern Int16U REGLTR_MemBuffIces[];

// Functions
//
void REGLTR_Process();
void REGLTR_Init();
float REGLTR_GetSetpoint(Int16U i);
void REGLTR_StartProcess();
void REGLTR_StopProcess();
bool REGLTR_IsPulseElapsed();
bool REGLTR_IsSyncDelayElapsed();
SamplingResult REGLTR_GetSample();

#endif // __REGULATOR_H
