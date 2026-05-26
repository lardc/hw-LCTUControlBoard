#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

// Include
#include "stdinc.h"
#include "SysConfig.h"

// Defines
#define IGES_AVG_WINDOW_US			20000
#define IGES_AVG_BUF_SIZE			((Int16U)(IGES_AVG_WINDOW_US / (Int32U)TIMER15_uS))

// Functions
//
void RINGBUF_ResetIgesAvg();
void RINGBUF_AddNewSampleIges(float Ig);
float RINGBUF_GetIgesAvg();
Int16U RINGBUF_GetIgesAvgCount();

#endif // _RINGBUFFER_H
