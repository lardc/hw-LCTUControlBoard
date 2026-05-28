#ifndef __MEASUREMENT_H
#define __MEASUREMENT_H

#include "stdinc.h"
#include "LowLevel.h"

// Functions
//
float MEASURE_Ucap(float SampleADC);
float MEASURE_Ug(float SampleADC);
float MEASURE_I(float SampleADC, IChannel Channel);
//
void MEASURE_ConvertIScope(pFloat32 InputArray, Int16U DataLength, IChannel Channel);
//
Int16U MEASURE_ConvertUset(float Uset);

#endif // __MEASUREMENT_H
