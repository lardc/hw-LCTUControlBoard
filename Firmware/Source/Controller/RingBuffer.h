#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

// Include
#include "stdinc.h"
#include "SysConfig.h"

// Defines
#define ICES_AVG_WINDOW_US			20000
#define ICES_AVG_BUF_SIZE			((Int16U)(ICES_AVG_WINDOW_US / (Int32U)TIMER15_uS))
#define UCE_AVG_WINDOW_US			20000
#define UCE_AVG_BUF_SIZE			((Int16U)(UCE_AVG_WINDOW_US / (Int32U)TIMER15_uS))

// Functions
//
void RINGBUF_ResetIcesAvg();
void RINGBUF_AddNewSampleIces(float Ig);
float RINGBUF_GetIcesAvg();
Int16U RINGBUF_GetIcesAvgCount();

void RINGBUF_ResetUceAvg();
void RINGBUF_AddNewSampleUce(float Uce);
float RINGBUF_GetUceAvg();
Int16U RINGBUF_GetUceAvgCount();

#endif // _RINGBUFFER_H
