#ifndef __DIAGNOSTIC_H
#define __DIAGNOSTIC_H

// Include
#include "stdinc.h"

// Definitions
//
#define DIAG_PULSE_BUFFER_SIZE 2000

// Variables
//
extern Int16U DIAG_PulseDataBuffer[DIAG_PULSE_BUFFER_SIZE];

// Functions
//
bool DIAG_HandleDiagnosticAction(Int16U ActionID, Int16U *pUserError);
void DIAG_GenerateTrapezoidWave();

#endif // __DIAGNOSTIC_H
