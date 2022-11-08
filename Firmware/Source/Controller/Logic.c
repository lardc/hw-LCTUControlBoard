// Includes
#include "Board.h"
#include "Logic.h"
#include "ConvertUtils.h"
#include "DataTable.h"
#include "DiscreteOpAmp.h"
#include "LowLevel.h"
#include "Math.h"
#include "Delay.h"
#include "Controller.h"
#include "PAU.h"


// Variables
float VoltageTarget, VoltageSetpoint, RegulatorPcoef, RegulatorIcoef, VoltageThreshold;
float RegulatorAlowedError, dV;
float  Qi;
bool VoltageRange = VOLTAGE_RANGE_0;
bool IsImpulse = false;
Int16U RegulatorPulseCounter = 0;
Int16U PulsePointsQuantity = 0;
volatile Int64U LOGIC_PowerOnCounter = 0;
volatile Int64U LOGIC_BetweenPulsesDelay = 0;
volatile Int64U LOGIC_TestTime = 0;
volatile Int16U RingBufferIndex = 0;
Int16U FollowingErrorCounter = 0;
Int16U FollowingErrorCounterMax = 0;
Int16U PAUSyncDelayCounter = 0;
Int16U PAUSyncTransmitCounter = 0;
Int16U PAUSyncReceiveCounter = 0;
Int16U OSCSyncTimeStart = 0;
bool PAUSyncGenerateFlag = false;

// Arrays
float RingBuffer_Voltage[MAF_BUFFER_LENGTH];
float RingBuffer_Current[MAF_BUFFER_LENGTH];

// Functions prototypes
void LOGIC_CacheVariables();
void LOGIC_SaveToRingBuffer(MeasureSample Sample);
void LOGIC_ClearVariables();

// Functions
//
void LOGIC_StartPrepare()
{
	LOGIC_SetVolatgeRange();
	LOGIC_CacheVariables();
	CU_LoadConvertParams(VoltageRange);
}
//-----------------------------

void LOGIC_HarwareDefaultState()
{
	LL_OscSyncSetState(false);
	LL_PAUSyncSetState(false);
	LL_PAU_Shunting(true);
}
//-----------------------------

void LOGIC_CacheVariables()
{
	VoltageSetpoint = DataTable[REG_VOLTAGE_SETPOINT];
	PulsePointsQuantity = DataTable[REG_PULSE_WIDTH] * 1000 / TIMER6_uS;
	RegulatorPcoef = DataTable[REG_REGULATOR_Kp];
	RegulatorIcoef = DataTable[REG_REGULATOR_Ki];
	dV = VoltageSetpoint / DataTable[REG_PULSE_FRONT_WIDTH] * TIMER6_uS / 1000;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALOWED_ERR];
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];
	OSCSyncTimeStart = (PulsePointsQuantity > DataTable[REG_OSC_SYNC_WIDTH]) ? PulsePointsQuantity - DataTable[REG_OSC_SYNC_WIDTH] : 0;

	LOGIC_ClearVariables();
}
//-----------------------------

RegulatorState LOGIC_RegulatorCycle(MeasureSample Sample)
{
	float RegulatorError, Qp, RegulatorOut;
	static Int16U PAUSyncDelayCounter = 0;
	RegulatorState Result = RS_InProcess;

	// Формирование линейно нарастающего фронта импульса напряжения
	if(VoltageTarget < VoltageSetpoint)
	{
		PAUSyncDelayCounter = 0;
		VoltageTarget += dV;
	}
	else
	{
		VoltageTarget = VoltageSetpoint;
		PAUSyncDelayCounter++;

		if(!DataTable[REG_PAU_EMULATED])
		{
			if(PAUSyncDelayCounter >= DataTable[REG_PAU_SNC_DELAY] && CONTROL_State != DS_SelfTest && !PAUSyncGenerateFlag)
			{
				LL_PAU_Shunting(false);
				LL_PAUSyncFlip();

				PAUSyncGenerateFlag = true;
			}
		}
	}

	RegulatorError = (RegulatorPulseCounter == 0) ? 0 : (VoltageTarget - Sample.Voltage);

	if(fabsf(RegulatorError / VoltageTarget * 100) < RegulatorAlowedError)
	{
		if(FollowingErrorCounter)
			FollowingErrorCounter--;
	}
	else
	{
		FollowingErrorCounter++;

		if(FollowingErrorCounter >= FollowingErrorCounterMax && !DataTable[REG_FOLLOWING_ERR_MUTE] && Result == RS_InProcess)
			Result = RS_FollowingError;
	}

	Qi += RegulatorError * RegulatorIcoef;

	if(Qi > DataTable[REG_REGULATOR_QI_MAX])
		Qi = DataTable[REG_REGULATOR_QI_MAX];

	if(Qi < (-1) * DataTable[REG_REGULATOR_QI_MAX])
		Qi = (-1) * DataTable[REG_REGULATOR_QI_MAX];

	Qp = RegulatorError * RegulatorPcoef;

	RegulatorOut = VoltageTarget + Qp +Qi;

	if(RegulatorOut > DSIOPAMP_STACK_VOLTAGE_MAX)
		RegulatorOut = DSIOPAMP_STACK_VOLTAGE_MAX;

	DISOPAMP_SetVoltage(RegulatorOut);

	LOGIC_LoggingProcess(Sample, RegulatorError);

	RegulatorPulseCounter++;

	if(RegulatorPulseCounter >= OSCSyncTimeStart)
		LL_OscSyncSetState(true);

	if (RegulatorPulseCounter >= PulsePointsQuantity && Result == RS_InProcess)
	{
		LL_OscSyncSetState(false);
		FollowingErrorCounter = 0;
		Result = RS_Finished;
	}

	return Result;
}
//-----------------------------

void LOGIC_SaveToRingBuffer(MeasureSample Sample)
{
	RingBuffer_Voltage[RingBufferIndex] = Sample.Voltage;
	RingBuffer_Current[RingBufferIndex] = Sample.Current;

	RingBufferIndex++;
	RingBufferIndex &= MAF_BUFFER_INDEX_MASK;
}
//-----------------------------

void LOGIC_LoggingProcess(MeasureSample Sample, float RegulatorErr)
{
	static Int16U ScopeLogStep = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_STEP] && CONTROL_ValuesCounter < VALUES_x_SIZE)
	{
		ScopeLogStep = 0;

		CONTROL_ValuesVoltage[CONTROL_ValuesCounter] = Sample.Voltage;
		CONTROL_ValuesCurrent[CONTROL_ValuesCounter] = Sample.Current;
		CONTROL_RegulatorErr[CONTROL_ValuesCounter] = RegulatorErr;
		CONTROL_ValuesCounter++;
	}

	LOGIC_SaveToRingBuffer(Sample);
}
//-----------------------------

void LOGIC_StopProcess()
{
	TIM_Stop(TIM6);
	DISOPAMP_SetVoltage(0);
}
//-----------------------------

void LOGIC_ClearVariables()
{
	for(int i = 0; i < MAF_BUFFER_LENGTH; i++)
		RingBuffer_Voltage[i] = 0;

	RingBufferIndex = 0;
	Qi = 0;
	RegulatorPulseCounter = 0;
	VoltageTarget = 0;
	FollowingErrorCounter = 0;
	LOGIC_TestTime = 0;
	PAUSyncDelayCounter = 0;
	PAUSyncReceiveCounter = 0;
	PAUSyncGenerateFlag = 0;
}
//-----------------------------

void LOGIC_SetVolatgeRange()
{
	if(DataTable[REG_VOLTAGE_SETPOINT] > DataTable[REG_VOLTAGE_RANGE_THRESHOLD])
	{
		LL_VoltageRangeSet(VOLTAGE_RANGE_1);
		VoltageRange = VOLTAGE_RANGE_1;
	}
	else
	{
		LL_VoltageRangeSet(VOLTAGE_RANGE_0);
		VoltageRange = VOLTAGE_RANGE_0;
	}
}
//-----------------------------

void LOGIC_HandleFan(bool IsImpulse)
{
	static uint32_t IncrementCounter = 0;
	static uint64_t FanOnTimeout = 0;

	if(DataTable[REG_FAN_CTRL])
	{
		// Увеличение счётчика в простое
		if (!IsImpulse)
			IncrementCounter++;

		// Включение вентилятора
		if ((IncrementCounter > ((uint32_t)DataTable[REG_FAN_OPERATE_PERIOD] * 1000)) || IsImpulse)
		{
			IncrementCounter = 0;
			FanOnTimeout = CONTROL_TimeCounter + ((uint32_t)DataTable[REG_FAN_OPERATE_TIME] * 1000);
			LL_FanControl(true);
		}

		// Отключение вентилятора
		if (FanOnTimeout && (CONTROL_TimeCounter > FanOnTimeout))
		{
			FanOnTimeout = 0;
			LL_FanControl(false);
		}
	}
}
//-----------------------------------------------

void CONTROL_HandleExternalLamp(bool IsImpulse)
{
	static Int64U ExternalLampCounter = 0;

	if(DataTable[REG_LAMP_CTRL])
	{
		if(CONTROL_State == DS_Fault)
		{
			if(++ExternalLampCounter > TIME_FAULT_LED_BLINK)
			{
				LL_ToggleExtIndication();
				ExternalLampCounter = 0;
			}
		}
		else
			{
				if(IsImpulse)
				{
					LL_ExtIndicationControl(true);
					ExternalLampCounter = CONTROL_TimeCounter + EXT_LAMP_ON_STATE_TIME;
				}
				else
				{
					if(CONTROL_TimeCounter >= ExternalLampCounter)
						LL_ExtIndicationControl(false);
				}
			}
	}
}
//-----------------------------------------------

void LOGIC_ResetOutputRegisters()
{
	DataTable[REG_FAULT_REASON] = DF_NONE;
	DataTable[REG_DISABLE_REASON] = DF_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;
	//
	DataTable[REG_RESULT_VOLTAGE] = 0;
	DataTable[REG_RESULT_CURRENT] = 0;
	//
	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//------------------------------------------

bool LOGIC_PAUConfigProcess()
{
	Int16U PAU_State;
	static Int64U Timeout = 0;

	// Обновление состояния PAU
	if(!PAU_UpdateState(&PAU_State))
		CONTROL_SwitchToFault(DF_INTERFACE);

	switch(PAU_State)
	{
	case PS_Ready:
		if(!PAU_Configure(PAU_CHANNEL_LCTU, DataTable[REG_CURRENT_CUT_OFF], DataTable[REG_PULSE_WIDTH]))
			CONTROL_SwitchToFault(DF_INTERFACE);

		Timeout = 0;
		break;

	case PS_InProcess:
		if(!Timeout)
			Timeout = CONTROL_TimeCounter + PAU_CONFIG_TIMEOUT;
		else
			if(CONTROL_TimeCounter >= Timeout)
				CONTROL_SwitchToFault(DF_PAU_CONFIG_TIMEOUT);
		break;

	case PS_ConfigReady:
		return true;
		break;

	case PS_Fault:
		CONTROL_SwitchToFault(DF_PAU);
		break;

	default:
		if(DataTable[REG_PAU_EMULATED])
			return true;
		break;
	}

	return false;
}
//------------------------------------------
