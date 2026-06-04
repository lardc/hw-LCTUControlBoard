#ifndef __JSON_DESCRIPTION_H
#define __JSON_DESCRIPTION_H

#include "Global.h"
#include "FormatOutputJSON.h"

#define USET_MININAL	500		// в В
#define USET_MAXIMUM	7000

#define IMEAS_HIGHEST	300.0f	// в мА

extern float UsetMin, UsetMax;
extern float Imeas0Min, Imeas0Max;
extern float Imeas1Min, Imeas1Max;
extern float Imeas2Min, Imeas2Max;
extern float Imeas3Min, Imeas3Max;
extern float Imeas4Min, Imeas4Max;

#endif // __JSON_DESCRIPTION_H
