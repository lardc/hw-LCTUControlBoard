#ifndef __INITCONFIG_H
#define __INITCONFIG_H

#include "stdinc.h"

//Functions
//
Boolean INITCFG_SysClk();
void INITCFG_IO();
void INITCFG_UART();
void INITCFG_ADC();
void INITCFG_Timer7();
void INITCFG_Timer15();
void INITCFG_WatchDog();
void INITCFG_ConfigCAN(Int16U NodeID);
void INITCFG_SPI();
void INITCFG_DMA();

#endif //__INITCONFIG_H
