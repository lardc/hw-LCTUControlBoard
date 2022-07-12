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
	SS_PS_WaitingStart = 1,
	SS_PS_WaitingOn = 2,
	SS_PS_WaitingOff = 3,
	SS_WaitPause= 4,
	SS_StartPulse = 5,
	SS_Pulse = 6,

	SS_ST_Sync = 10,
	SS_ST_StartPulse = 11,
	SS_ST_Pulse = 12,
	SS_ST_WaitPause = 13,
	SS_ST_PulseCheck = 14
} DeviceSubState;

// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern Int64U CONTROL_LEDTimeout;
extern volatile Int64U	CONTROL_PulseToPulseTime;
extern volatile Int16U CONTROL_VoltageCounter;
extern volatile Int16U CONTROL_CurrentCounter;
extern volatile Int16U CONTROL_RegulatorErr_Counter;
extern volatile Int16U CONTROL_ValuesVoltage[VALUES_x_SIZE];
extern volatile Int16U CONTROL_ValuesCurrent[VALUES_x_SIZE];
extern volatile Int16U CONTROL_RegulatorErr[VALUES_x_SIZE];


// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_HighPriorityProcess();
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_StartProcess();

#endif // __CONTROLLER_H
