#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "stdinc.h"

extern volatile bool INT_UcapAdcReady;

void INT_ResetDMAFlags();

#endif // __INTERRUPTS_H
