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
	SS_None 				= 0,
	SS_Init					= 1,
	SS_Wait48VPause			= 2,
	SS_ConfigPulse			= 3,
	SS_RegulatorProcess		= 4,
	SS_FollowingErr			= 6,
	SS_VoltageErr			= 7,
	SS_MaxCurrentErr		= 8,
	SS_FinishProcess		= 10,
	SS_GetResults 			= 11,
	SS_RegulatorProcessSelfTest = 12,
	SS_Activation			= 13,
	SS_Preparation			= 14,
	SS_Deactivation			= 15,
	SS_DeactivationWaitRout	= 16,
	SS_DeactivationWaitRcon	= 17,
} DeviceSubState;

typedef enum __MeasureType
{
	MT_Ices			= 1,
	MT_ST_TestLoad	= 4,
} MeasureType;

// Variables
extern volatile Int64U CONTROL_TimeCounter;

extern volatile DeviceState CONTROL_State;
extern volatile DeviceSubState CONTROL_SubState;
extern volatile MeasureType CONTROL_MeasureType;

extern volatile Int16U CONTROL_ExtInfoCounter;
extern volatile float CONTROL_ExtInfoData[];

extern Int16U CONTROL_Values_Counter;
extern float CONTROL_RegulatorIg[];
extern float CONTROL_RegulatorUg[];
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
bool CONTROL_IsSafetyOk();
void CONTROL_WatchDogUpdate();
void CONTROL_InitJSONPointers();

#endif // __CONTROLLER_H
