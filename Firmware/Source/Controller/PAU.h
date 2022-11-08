#ifndef CONTROLLER_PAU_H_
#define CONTROLLER_PAU_H_

// Includes
//
#include "stdinc.h"

// Definitions
//
#define PAU_CHANNEL_LCTU			1
#define PAU_CHANNEL_IGTU			2
//
#define PAU_CONFIG_TIMEOUT			1000
#define PAU_WAIT_READY_TIMEOUT		1500

// Types
//
typedef enum __PAUState
{
	PS_None = 0,
	PS_Fault = 1,
	PS_Disabled = 2,
	PS_Ready = 3,
	PS_InProcess = 4,
	PS_ConfigReady = 5
} PAUState;

// Functions
//
bool PAU_UpdateState(Int16U* Register);
bool PAU_Configure(Int16U Channel, float Range, float Time);
bool PAU_ClearFault();
bool PAU_ClearWarning();
bool PAU_ReadMeasuredData(float* Data);

#endif /* CONTROLLER_PAU_H_ */
