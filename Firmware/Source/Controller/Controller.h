#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
#include "stdinc.h"

// Types
typedef enum __DeviceState
{
	DS_None 			= 0,
	DS_Fault 			= 1,
	DS_Disabled 		= 2,
	DS_Ready 			= 3,
	DS_InProcess		= 4,
} DeviceState;

typedef enum __DeviceSubState
{
	SS_None 			= 0,
	SS_Init,
	SS_InitialRelayPause,
	SS_ConfigPulse,
	SS_RegulatorProcess,
	SS_FollowingErr,
	SS_VoltageErr,
	SS_MaxCurrentErr,
	SS_CurrentErr,
	SS_FinishProcess,
	SS_GetResults,
	SS_RegulatorProcessSelfTest,
	SS_Activation,
} DeviceSubState;

typedef enum __MeasureType
{
	MT_Ices			= 1,
	MT_ST_TestLoad	= 4,
} MeasureType;

// Variables
extern volatile Int64U CONTROL_TimeCounter;
extern volatile Boolean RequestSaveToFlash;

extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern volatile MeasureType CONTROL_MeasureType;

extern volatile Int16U CONTROL_ExtInfoCounter;
extern volatile float CONTROL_ExtInfoData[];

extern Int16U CONTROL_Values_Counter;
extern float CONTROL_RegulatorIces[];
extern float CONTROL_RegulatorUce[];
extern float CONTROL_RegulatorSetpoint[];
extern float CONTROL_RegulatorCorrection[];
extern float CONTROL_RegulatorError[];
extern float CONTROL_DACRaw[];

// Functions
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SetDeviceState(DeviceState NewState);
void CONTROL_SetDeviceSubState(DeviceSubState NewSubState);
void CONTROL_SwitchToProblem(Int16U Reason);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_StartMeasure(MeasureType Type);
bool CONTROL_IsSafetyOk();
void CONTROL_WatchDogUpdate();
void CONTROL_InitJSONPointers();

#endif // __CONTROLLER_H
