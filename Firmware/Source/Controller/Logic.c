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

// Definitions
#define MAF_BUFFER_LENGTH				128
#define MAF_BUFFER_INDEX_MASK			MAF_BUFFER_LENGTH - 1
//

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
Int16U PAUsyncDelayCounter = 0;

// Arrays
float RingBuffer_Voltage[MAF_BUFFER_LENGTH];
float RingBuffer_Current[MAF_BUFFER_LENGTH];

// Functions prototypes
void LOGIC_CacheVariables();
void LOGIC_SaveToRingBuffer(MeasureSample Sample);
float LOGIC_ExtractAveragedDatas(float* Buffer, Int16U BufferLength);
void LOGIC_SaveRegulatorErr(float Error);
void LOGIC_ClearVariables();
float LOGIC_GetLastSampledData(float* InputBuffer);

// Functions
//
void LOGIC_StartPrepare(TestType Type)
{
	LOGIC_HarwarePrepare(Type);

	LOGIC_CacheVariables();
	CU_LoadConvertParams(VoltageRange);
}
//-----------------------------

void LOGIC_HarwarePrepare(TestType Type)
{
	LOGIC_SetVolatgeRange();

	if(Type == TT_DUT)
		LL_OutputCommutationControl(true);
	else
		LL_SelfTestCommutationControl(true);
}
//-----------------------------

void LOGIC_HarwareDefaultState()
{
	LOGIC_OSCSync(false);
	LOGIC_PAUSync(false);
	LL_PAU_Shunting(true);

	if(CONTROL_State == DS_InProcess)
	{
		DELAY_MS(10);
		LL_OutputCommutationControl(false);
	}
}
//-----------------------------

void LOGIC_CacheVariables()
{
	VoltageSetpoint = DataTable[REG_VOLTAGE_SETPOINT];
	PulsePointsQuantity = DataTable[REG_PULSE_WIDTH] * 1000 / TIMER6_uS;
	RegulatorPcoef = DataTable[REG_REGULATOR_Kp] / 1000;
	RegulatorIcoef = DataTable[REG_REGULATOR_Ki] / 1000;
	dV = VoltageSetpoint / DataTable[REG_PULSE_FRONT_WIDTH] * TIMER6_uS / 1000;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALOWED_ERR];
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];

	LOGIC_ClearVariables();
}
//-----------------------------

RegulatorState LOGIC_RegulatorCycle(MeasureSample Sample)
{
	float RegulatorError, Qp, RegulatorOut;
	static Int16U PAUsyncDelayCounter = 0;
	RegulatorState Result = RS_InProcess;

	// Формирование линейно нарастающего фронта импульса напряжения
	if(VoltageTarget < VoltageSetpoint)
		VoltageTarget += dV;
	else
	{
		VoltageTarget = VoltageSetpoint;
		PAUsyncDelayCounter++;
	}

	if(!DataTable[REG_PAU_EMULATED] && PAUsyncDelayCounter >= DataTable[REG_PAU_SNC_DELAY] && CONTROL_State != DS_SelfTest)
	{
		LL_PAU_Shunting(false);
		LOGIC_PAUSync(true);
	}

	RegulatorError = (RegulatorPulseCounter == 0) ? 0 : (VoltageTarget - Sample.Voltage);

	if(fabsf(RegulatorError) < RegulatorAlowedError)
	{
		if(FollowingErrorCounter)
			FollowingErrorCounter--;
	}
	else
	{
		FollowingErrorCounter++;

		if(FollowingErrorCounter >= FollowingErrorCounterMax && !DataTable[REG_FOLLOWING_ERR_MUTE])
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

	LOGIC_SaveRegulatorErr(RegulatorError);

	RegulatorPulseCounter++;
	if (RegulatorPulseCounter >= PulsePointsQuantity)
		Result = RS_Finished;

	return Result;
}
//-----------------------------

void LOGIC_SaveRegulatorErr(float Error)
{
	static Int16U ScopeLogStep = 0, LocalCounter = 0;

	// Сброс локального счетчика в начале логгирования
	if (CONTROL_RegulatorErr_Counter == 0)
		LocalCounter = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_STEP])
	{
		ScopeLogStep = 0;

		CONTROL_RegulatorErr[LocalCounter] = Error;
		CONTROL_RegulatorErr_Counter = LocalCounter;

		++LocalCounter;
	}

	// Условие обновления глобального счетчика данных
	if (CONTROL_RegulatorErr_Counter < VALUES_x_SIZE)
		CONTROL_RegulatorErr_Counter = LocalCounter;

	// Сброс локального счетчика
	if (LocalCounter >= VALUES_x_SIZE)
		LocalCounter = 0;
}
//-----------------------------

float LOGIC_GetAverageVoltage()
{
	return LOGIC_ExtractAveragedDatas(&RingBuffer_Voltage[0], MAF_BUFFER_LENGTH);
}
//-----------------------------

float LOGIC_GetLastSampledVoltage()
{
	return LOGIC_GetLastSampledData(&RingBuffer_Voltage[0]);
}
//-----------------------------

float LOGIC_GetLastSampledData(float* InputBuffer)
{
	Int16U Index;

	Index = RingBufferIndex - 1;
	Index &= MAF_BUFFER_INDEX_MASK;

	return *(InputBuffer + Index);
}
//-----------------------------

float LOGIC_ExtractAveragedDatas(float* Buffer, Int16U BufferLength)
{
	float Temp = 0;

	for(int i = 0; i < BufferLength; i++)
		Temp += *(Buffer + i);

	return (Temp / BufferLength);
}
//-----------------------------

void LOGIC_SaveToRingBuffer(MeasureSample Sample)
{
	RingBuffer_Voltage[RingBufferIndex] = Sample.Voltage;
	RingBuffer_Voltage[RingBufferIndex] = Sample.Current;

	RingBufferIndex++;
	RingBufferIndex &= MAF_BUFFER_INDEX_MASK;
}
//-----------------------------

void LOGIC_LoggingProcess(MeasureSample Sample)
{
	static Int16U ScopeLogStep = 0, LocalCounter = 0;

	// Сброс локального счётчика в начале логгирования
	if (!CONTROL_VoltageCounter)
		LocalCounter = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_STEP])
	{
		CONTROL_ValuesVoltage[LocalCounter] = Sample.Voltage;
		CONTROL_ValuesCurrent[LocalCounter] = Sample.Current;

		ScopeLogStep = 0;
		++LocalCounter;
	}

	LOGIC_SaveToRingBuffer(Sample);

	// Условие обновления глобального счётчика данных
	if (CONTROL_VoltageCounter < VALUES_x_SIZE)
		CONTROL_CurrentCounter = CONTROL_VoltageCounter = LocalCounter;

	// Сброс локального счётчика
	if (LocalCounter >= VALUES_x_SIZE)
		LocalCounter = 0;
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
	LOGIC_TestTime = 0;
	FollowingErrorCounter = 0;
	PAUsyncDelayCounter = 0;
	IsImpulse = false;
}
//-----------------------------

void LOGIC_SetVolatgeRange()
{
	if(DataTable[REG_VOLTAGE_SETPOINT] > DataTable[REG_VOLTAGE_RANGE_THRESHOLD])
		VoltageRange = VOLTAGE_RANGE_1;
	else
		VoltageRange = VOLTAGE_RANGE_0;

	LL_VoltageRangeSet(VoltageRange);
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
	static Int64U ExternalLampTimeout = 0;

	if(DataTable[REG_LAMP_CTRL])
	{
		if(IsImpulse)
		{
			LL_ExtIndicationControl(true);
			ExternalLampTimeout = CONTROL_TimeCounter + EXT_LAMP_ON_STATE_TIME;
		}
		else
		{
			if(CONTROL_TimeCounter >= ExternalLampTimeout)
				LL_ExtIndicationControl(false);
		}
	}
}
//-----------------------------------------------

void LOGIC_PAUSync(bool State)
{
	LL_PAUSyncSetState(State);
}
//-----------------------------------------------

void LOGIC_OSCSync(bool State)
{
	LL_OscSyncSetState(State);
}
//-----------------------------------------------
