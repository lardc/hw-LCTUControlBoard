// Includes
#include "Board.h"
#include "Logic.h"
#include "ConvertUtils.h"
#include "DataTable.h"
#include "DiscreteOpAmp.h"
#include "LowLevel.h"
#include "Math.h"
#include "Delay.h"

// Definitions
#define MAF_BUFFER_LENGTH				128
#define MAF_BUFFER_INDEX_MASK			MAF_BUFFER_LENGTH - 1
//

// Variables
float VoltageTarget, VoltageSetpoint, RegulatorPcoef, RegulatorIcoef, VoltageThreshold;
float RegulatorAlowedError, dV;
float  Qi;
bool VoltageRange = VOLTAGE_RANGE_0;
Int16U RegulatorPulseCounter = 0;
Int16U PulsePointsQuantity = 0;
volatile Int64U LOGIC_PowerOnCounter = 0;
volatile Int64U LOGIC_BetweenPulsesDelay = 0;
volatile Int64U LOGIC_TestTime = 0;
volatile Int16U RingBufferIndex = 0;
Int16U FollowingErrorCounter = 0;
Int16U FollowingErrorCounterMax = 0;

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
void LOGIC_StartPrepare()
{
	LOGIC_CacheVariables();
	LOGIC_SetVolatgeRange();
	CU_LoadConvertParams(VoltageRange);

	LL_OutputCommutationControl(true);
}
//-----------------------------

void LOGIC_CacheVariables()
{
	VoltageSetpoint = DataTable[REG_VOLTAGE_SETPOINT];
	VoltageThreshold = DataTable[REG_VOLTAGE_RANGE_THRESHOLD];
	PulsePointsQuantity = DataTable[REG_PULSE_WIDTH] * 1000 / TIMER6_uS;
	RegulatorPcoef = (float)DataTable[REG_REGULATOR_Kp] / 1000;
	RegulatorIcoef = (float)DataTable[REG_REGULATOR_Ki] / 1000;
	dV = VoltageSetpoint / DataTable[REG_PULSE_FRONT_WIDTH] * TIMER6_uS / 1000;
	RegulatorAlowedError = (float)DataTable[REG_REGULATOR_ALOWED_ERR] / 10;
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];

	LOGIC_ClearVariables();
}
//-----------------------------

bool LOGIC_RegulatorCycle(MeasureSample Sample, Int16U* Fault)
{
	float RegulatorError, Qp, RegulatorOut;
	bool Finished = false;

	// Формирование линейно нарастающего фронта импульса напряжения
	if(VoltageTarget < VoltageSetpoint)
		VoltageTarget += dV;
	else
		VoltageTarget = VoltageSetpoint;

	RegulatorError = (RegulatorPulseCounter == 0) ? 0 : (VoltageTarget - Sample.Voltage);

	if(fabsf(RegulatorError) < RegulatorAlowedError)
	{
		if(FollowingErrorCounter)
			FollowingErrorCounter--;
	}
	else
		FollowingErrorCounter++;

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


	if(!DataTable[REG_FOLLOWING_ERR_MUTE] && FollowingErrorCounter >= FollowingErrorCounterMax)
	{
		if(Sample.Current >= DISOPAMP_CURRENT_MAX)
			*Fault = DF_OUTPUT_SHORT;
		else
			*Fault = DF_FOLLOWING_ERROR;

		Finished = true;
	}
	else
	{
		RegulatorPulseCounter++;
		Finished = (RegulatorPulseCounter >= PulsePointsQuantity) ? true : false;
	}

	return Finished;
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

		CONTROL_RegulatorErr[LocalCounter] = (Int16S)(Error * 10);
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
		CONTROL_ValuesVoltage[LocalCounter] = (Int16U)(Sample.Voltage * 10);
		CONTROL_ValuesCurrent[LocalCounter] = (Int16U)(Sample.Current);

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
	DELAY_MS(20);
	LL_OutputCommutationControl(false);
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
}
//-----------------------------

void LOGIC_SetVolatgeRange()
{
	if(VoltageSetpoint > VoltageThreshold)
		VoltageRange = VOLTAGE_RANGE_1;
	else
		VoltageRange = VOLTAGE_RANGE_0;

	LL_VoltageRangeSet(VoltageRange);
}
//-----------------------------
