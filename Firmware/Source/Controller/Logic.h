#ifndef CONTROLLER_LOGIC_H_
#define CONTROLLER_LOGIC_H_

// Includes
//
#include "stdinc.h"

// Variables
//
extern Int16U LOGIC_ChannelNumber;

//Functions
//
void LOGIC_HandleMeasurement();
void LOGIC_Deactivate();
void LOGIC_StopProcess();

#endif // CONTROLLER_LOGIC_H_
