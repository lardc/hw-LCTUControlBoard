#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
//
#include "stdinc.h"
#include "Global.h"

// Types
//
typedef enum __DeviceState
{
	DS_None = 0,
	DS_Fault = 1,
	DS_Disabled = 2,
	DS_Ready = 3,
	DS_InProcess = 4,
	DS_SelfTest = 5
} DeviceState;

typedef enum __DeviceSubState
{
	SS_None = 0,
	SS_StartProcess,
	SS_IdleTimeCheck,
	SS_CapChargeStart,
	SS_CapChargeStop,
	SS_PAUConfig,
	SS_CommutationEnable,
	SS_CommutationDisable,
	SS_StartPulse,
	SS_Pulse,
	SS_PostPulseDelay,

	SS_ST_StartPrepare = 10,
	SS_ST_StartPulse,
	SS_ST_Pulse,
	SS_ST_WaitPause,
	SS_ST_PulseCheck
} DeviceSubState;

// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern Int64U CONTROL_LEDTimeout;
extern volatile Int64U	CONTROL_PulseToPulseTime;
extern volatile Int16U CONTROL_ValuesCounter;
extern volatile float CONTROL_ValuesVoltage[VALUES_x_SIZE];
extern volatile float CONTROL_ValuesCurrent[VALUES_x_SIZE];
extern volatile float CONTROL_RegulatorErr[VALUES_x_SIZE];


// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_HighPriorityProcess();
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_SetDeviceSubState(DeviceSubState NewSubState);
void CONTROL_StartProcess();
void CONTROL_ForceStopProcess();

#endif // __CONTROLLER_H
