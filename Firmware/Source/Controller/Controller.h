﻿#ifndef __CONTROLLER_H
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
	SS_PS_WaitingStart,
	SS_PS_WaitingOn,
	SS_PS_WaitingOff,
	SS_WaitPause,
	SS_StartPulse,
	SS_Pulse,

	SS_ST_Sync = 10,
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
extern volatile Int16U CONTROL_VoltageCounter;
extern volatile Int16U CONTROL_CurrentCounter;
extern volatile Int16U CONTROL_RegulatorErr_Counter;
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
void CONTROL_StartProcess();
void CONTROL_ForceStopProcess();

#endif // __CONTROLLER_H
