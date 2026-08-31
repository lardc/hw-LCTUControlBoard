// Header
#include "Controller.h"

// Includes
#include "SysConfig.h"
#include "Measurement.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Regulator.h"
#include "LowLevel.h"
#include "Board.h"
#include "Utils.h"
#include "Logic.h"
#include "RingBuffer.h"
#include "Interrupts.h"
#include "math.h"

// Variables
Int16U REGLTR_MemBuffUce[ADC_SEQ_LENGTH];
Int16U REGLTR_MemBuffIces[ADC_SEQ_LENGTH];
static float Kp, Ki, Qi = 0, FollowingErrThreshold, VoltagErrThreshold;
static Int16U FollowingErrLimit, VoltagErrLimit, VoltageErrCount, ScalingCoef, ScalingCounter;
static Int16U FollowingErrorCounter = 0;
static Int16U MaxCurrentErrCount = 0;
static Int16U MaxCurrentErrLimit = 0;
static float PulseAmplitude, RiseRate, MaxCurrentA = 0.0f;
static Int32U PulseElapsedTicks = 0;
static Int32U PulseTimeoutTicks = 0;
static Int32U SyncDelayTicks = 0;
float RawSetPoint = 0;
float VoltStep = 0, Qp = 0;
float RegulatorError = 0;
RegulatorState RegState = RS_None;
volatile bool IsMeasureOk = false;

volatile SamplingResult Sample = {0};

// Forward functions
SamplingResult REGLTR_GetSample();
void REGLTR_StoreRegulatorDebug(float Uce, float Ices, float Setpoint, float Correction, float Error, float DACRaw);
Int16U REGLTR_CorrectionLogDACPoint();
void RGLTR_ErrorCheck();
Int16U REGLTR_GetScalingCoef();

// Functions
void REGLTR_Process()
{
	if (CONTROL_SubState != SS_RegulatorProcess
			&& CONTROL_SubState != SS_RegulatorProcessSelfTest)
		return;

	if(PulseElapsedTicks >= PulseTimeoutTicks)
	{
		REGLTR_StopProcess();
		return;
	}

	Int16U DACSetpoint;
	Sample = REGLTR_GetSample();
	PulseElapsedTicks++;

	if (!LL_IsSyncOn() && PulseElapsedTicks >= SyncDelayTicks)
		LL_SyncOSC(true);

	switch(RegState)
	{
		case RS_Rise:
			RawSetPoint += VoltStep;
			if(RawSetPoint >= PulseAmplitude)
			{
				RawSetPoint = PulseAmplitude;
				RegState = RS_FlatTop;
			}
			DACSetpoint = REGLTR_CorrectionLogDACPoint();
			LL_WriteDAC(DACSetpoint,0);
			break;

		case RS_FlatTop:
		default:
			DACSetpoint = REGLTR_CorrectionLogDACPoint();
			LL_WriteDAC(DACSetpoint,0);
			break;
	}

	if(PulseElapsedTicks >= PulseTimeoutTicks)
		REGLTR_StopProcess();
}
//-----------------------------------------

void REGLTR_Init()
{
	IsMeasureOk = false;
	Qi = FollowingErrorCounter = VoltageErrCount = 0;
	MaxCurrentErrCount = 0;
	RINGBUF_ResetIcesAvg();
	FollowingErrThreshold = (CONTROL_MeasureType == MT_ST_TestLoad) ?
								DataTable[REG_RGLTR_ST_ERR_THRESH] : DataTable[REG_RGLTR_FOLLOWING_ERR_THRESH];
	FollowingErrLimit = DataTable[REG_RGLTR_FOLLOWING_ERR_LIMIT];
	VoltagErrThreshold = DataTable[REG_VOLTAGE_ERR_THRESH];
	VoltagErrLimit = DataTable[REG_VOLTAGE_ERR_COUNT_LIMIT];
	MaxCurrentErrLimit = (Int16U)DataTable[REG_MAX_CURRENT_ERR_COUNT_LIMIT];
	MaxCurrentA = DataTable[REG_MAX_CURRENT_ICES] * 0.001f;
	RawSetPoint = 0;
	RegState = RS_Rise;
	RegulatorError = 0;

	switch(CONTROL_MeasureType)
	{
		case MT_Ices:
			PulseAmplitude = ABS(DataTable[REG_WORK_VOLTAGE_ICES]);
			break;

		case MT_ST_TestLoad:
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD] * 0.001f;
			break;
	}

	Int32U PulseRiseMs = (Int32U)DataTable[REG_PULSE_RISE_DURATION];
	Int32U PulseDurMs;
	if (PulseRiseMs == 0u)
		PulseRiseMs = 1u;
	if (CONTROL_MeasureType == MT_ST_TestLoad)
		PulseDurMs = (Int32U)DataTable[REG_ST_PULSE_DURATION];
	else
		PulseDurMs = (Int32U)DataTable[REG_PULSE_DURATION];

	RiseRate = PulseAmplitude / (float)PulseRiseMs;
	VoltStep = RiseRate * TIMER15_uS * 0.001f;
	PulseElapsedTicks = 0;
	PulseTimeoutTicks = ((PulseRiseMs + PulseDurMs) * 1000u) / TIMER15_uS;
	SyncDelayTicks = ((PulseRiseMs + (Int32U)DataTable[REG_SYNC_DELAY_AFTER_FLAT]) * 1000u) / TIMER15_uS;

	Kp = DataTable[REG_RGLTR_Kp];
	Ki = DataTable[REG_RGLTR_Ki];

	for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
	{
		REGLTR_MemBuffUce[i] = 0;
		REGLTR_MemBuffIces[i] = 0;
	}
	DataTable[REG_DEBUG_SCALING_COEF] = ScalingCoef = REGLTR_GetScalingCoef();
	if(DataTable[REG_SCALING_MUTE])
		ScalingCoef = ScalingCounter = 1;
}
//-----------------------------------------

Int16U REGLTR_CorrectionLogDACPoint()
{
	RGLTR_ErrorCheck();

	Qp = RegulatorError * Kp;
	Qi += RegulatorError * Ki;

	float SetPoint = RawSetPoint + Qp + Qi;
	Int16U DACPoint = MEASURE_ConvertUset(SetPoint);

	REGLTR_StoreRegulatorDebug(Sample.Uce, Sample.Ices, RawSetPoint, Qp + Qi, RegulatorError, (float)DACPoint);

	return DACPoint;
}
//-----------------------------------------

void RGLTR_ErrorCheck()
{
	float VoltageErr;

	switch(RegState)
	{
		case RS_FlatTop:
			RegulatorError = RawSetPoint - Sample.Uce;
			VoltageErr = ABS(PulseAmplitude - Sample.Uce) / PulseAmplitude;

			if (CONTROL_MeasureType == MT_Ices && ABS(Sample.Ices) > MaxCurrentA)
			{
				MaxCurrentErrCount++;
				if (MaxCurrentErrCount > MaxCurrentErrLimit)
				{
					CONTROL_SetDeviceSubState(SS_MaxCurrentErr);
					break;
				}
			}
			else
				MaxCurrentErrCount = 0;

			if(VoltageErr < VoltagErrThreshold)
			{
				if(CONTROL_MeasureType == MT_Ices)
					RINGBUF_AddNewSampleIces(Sample.Ices);
				IsMeasureOk = true;
				VoltageErrCount = 0;
			}
			else
			{
				VoltageErrCount++;
				if(VoltageErrCount > VoltagErrLimit)
					CONTROL_SetDeviceSubState(SS_VoltageErr);
			}
			break;

		case RS_Rise:
		default:
			RegulatorError = RawSetPoint - Sample.Uce;
			break;
	}

	if(RawSetPoint > 0.0f)
	{
		float absError = ABS(RegulatorError) / RawSetPoint;
		if(absError > FollowingErrThreshold)
		{
			if(FollowingErrorCounter < FollowingErrLimit)
				FollowingErrorCounter++;
			else
				CONTROL_SetDeviceSubState(SS_FollowingErr);
		}
		else
			FollowingErrorCounter = 0;
	}
}
//-----------------------------------------

SamplingResult REGLTR_GetSample()
{
	SamplingResult t = {0};
	float sumUce= 0, sumIces = 0;

	for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
	{
		sumUce += REGLTR_MemBuffUce[i];
		sumIces += REGLTR_MemBuffIces[i];
	}

	float avgUce = (float)sumUce / ADC_SEQ_LENGTH;
	float avgIces = (float)sumIces / ADC_SEQ_LENGTH;

	t.Uce = MEASURE_Uce(avgUce);
	t.Ices = MEASURE_Ices(avgIces, LOGIC_ChannelNumber);
	return t;
}
//-----------------------------------------

void REGLTR_StoreRegulatorDebug(float Uce, float Ices, float Setpoint, float Correction, float Error, float DACRaw)
{
	if(ScalingCounter > 1)
		ScalingCounter--;
	else
	{
		ScalingCounter = ScalingCoef;
		if(CONTROL_Values_Counter < VALUES_DEBUG_RGLTR_SIZE)
		{
			CONTROL_RegulatorUce[CONTROL_Values_Counter] = Uce;
			CONTROL_RegulatorIces[CONTROL_Values_Counter] = Ices;
			CONTROL_RegulatorSetpoint[CONTROL_Values_Counter] = Setpoint;
			CONTROL_RegulatorCorrection[CONTROL_Values_Counter] = Correction;
			CONTROL_RegulatorError[CONTROL_Values_Counter] = Error;
			CONTROL_DACRaw[CONTROL_Values_Counter] = DACRaw;
			++CONTROL_Values_Counter;
		}
	}
}
//-----------------------------------------

Int16U REGLTR_GetScalingCoef()
{
	Int16U Coef = 0;
	Int16U MsToMks = 1000;
	float RisingPart, SumTicks = 0;
	RisingPart = PulseAmplitude / RiseRate;
	switch(CONTROL_MeasureType)
	{
		case MT_Ices:
			SumTicks = (RisingPart + DataTable[REG_PULSE_DURATION]) * MsToMks / TIMER15_uS;
			break;

		case MT_ST_TestLoad:
			SumTicks = (RisingPart + DataTable[REG_ST_TL_FLATTOP_DURATION]) * MsToMks / TIMER15_uS;
			break;

		default:
			break;
	}
	if (SumTicks > (float)VALUES_DEBUG_RGLTR_SIZE)
		Coef = (Int16U)ceil(SumTicks / (float)VALUES_DEBUG_RGLTR_SIZE);
	else
		Coef = 1;
	ScalingCounter = Coef;
	return Coef;
}
//-----------------------------------------

void REGLTR_StartProcess()
{
	INT_UcapAdcReady = false;
	ADC_SamplingStop(ADC1);

	DMA_ChannelEnable(DMA2_Channel1, true);
	DMA_ChannelEnable(DMA2_Channel5, true);

	TIM_Start(TIM15);
}
//------------------------------------

void REGLTR_StopProcess()
{
	LL_WriteDAC(0,0);
	TIM_Stop(TIM15);

	DMA_ChannelEnable(DMA2_Channel1, false);
	DMA_ChannelEnable(DMA2_Channel5, false);

	INT_UcapAdcReady = true;
}
//------------------------------------

bool REGLTR_IsPulseElapsed()
{
	return PulseElapsedTicks >= PulseTimeoutTicks;
}
//------------------------------------

bool REGLTR_IsSyncDelayElapsed()
{
	return PulseElapsedTicks >= SyncDelayTicks;
}
//------------------------------------
