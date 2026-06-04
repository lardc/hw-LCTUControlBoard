#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

// Include
#include "stdinc.h"
#include "SysConfig.h"

// Defines
#define ICES_AVG_WINDOW_US			20000
#define ICES_AVG_BUF_SIZE			((Int16U)(ICES_AVG_WINDOW_US / (Int32U)TIMER15_uS))

// Functions
//
void RINGBUF_ResetIcesAvg();
void RINGBUF_AddNewSampleIces(float Ig);
float RINGBUF_GetIcesAvg();
Int16U RINGBUF_GetIcesAvgCount();

#endif // _RINGBUFFER_H
