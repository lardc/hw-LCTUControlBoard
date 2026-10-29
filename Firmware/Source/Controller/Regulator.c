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
#include "math.h"

// Variables
Int16U REGLTR_MemBuffUce[ADC_SEQ_LENGTH];
Int16U REGLTR_MemBuffIces[ADC_SEQ_LENGTH];
static float Kp, Ki, Qi = 0, FollowingErrThreshold, VoltagErrThreshold;
static Int16U FollowingErrLimit, VoltagErrLimit, VoltageErrCount, ScalingCoef, ScalingCounter;
static Int16U FollowingErrorCounter = 0;
static Int16U MaxCurrentErrCount = 0;
static Int16U MaxCurrentErrLimit = 0;
static float PulseAmplitude, RiseRate, MaxCurrentA = 0;
static float PreTriggerAmplitude = 0;
float RawSetPoint = 0;
float VoltStep = 0, Qp = 0;
float RegulatorError = 0;
RegulatorState RegState = RS_None;
static volatile bool FallDone = false;
static bool FallRampDone = false;
static Int32U FallHoldTicks = 0;
volatile bool IsMeasureOk = false;

volatile SamplingResult Sample = {0};

// Forward functions
SamplingResult REGLTR_GetSample();
void REGLTR_StoreRegulatorDebug(float Uce, float Ices, float Setpoint, float Correction, float Error, float DACRaw);
Int32U REGLTR_CorrectionLogDACPoint();
void RGLTR_ErrorCheck();
Int16U REGLTR_GetScalingCoef();
static bool REGLTR_IsPulseSubState();
static void REGLTR_AbortWithFall(DeviceSubState ErrorSubState);

// Functions
void REGLTR_Process()
{
	if (!REGLTR_IsPulseSubState())
		return;
	Int32U DACSetpoint;
	Sample = REGLTR_GetSample();

	switch(RegState)
	{
		case RS_Rise:
			RawSetPoint += VoltStep;
			if(RawSetPoint >= (PulseAmplitude - VoltStep))
			{
				RawSetPoint = PulseAmplitude;
				Qi = 0;
				RegState = RS_FlatTop;
			}
			DACSetpoint = REGLTR_CorrectionLogDACPoint();
			LL_WriteDAC24(DACSetpoint);
			break;

		case RS_Fall:
			// Спад без участия регулятора: линейное снижение от уровня начала спада до уровня предимпульса,
			// выдержка на этом уровне, затем перевод в 0
			if(!FallDone)
			{
				if(!FallRampDone)
				{
					float FallStep = (VoltStep > 0.0f) ? VoltStep : RawSetPoint;
					RawSetPoint -= FallStep;
					if(RawSetPoint <= (PreTriggerAmplitude + FallStep))
					{
						if(RawSetPoint > PreTriggerAmplitude || VoltStep <= 0.0f)
							RawSetPoint = PreTriggerAmplitude;
						FallRampDone = true;
					}
				}
				else if(FallHoldTicks > 0)
					--FallHoldTicks;
				else
				{
					RawSetPoint = 0;
					FallDone = true;
				}
			}
			RegulatorError = 0;
			DACSetpoint = (RawSetPoint > 0.0f) ? MEASURE_ConvertUset(RawSetPoint) : 0;
			REGLTR_StoreRegulatorDebug(Sample.Uce, Sample.Ices, RawSetPoint, 0, 0, (float)DACSetpoint);
			LL_WriteDAC24(DACSetpoint);
			break;

		case RS_FlatTop:
		default:
			DACSetpoint = REGLTR_CorrectionLogDACPoint();
			LL_WriteDAC24(DACSetpoint);
			break;
	}
}
//-----------------------------------------

void REGLTR_StartFall()
{
	// Уровень начала спада с учётом накопленной коррекции, чтобы не было скачка на выходе
	float Level = RawSetPoint + Qp + Qi;
	RawSetPoint = (Level > 0.0f) ? Level : 0.0f;
	FallRampDone = (RawSetPoint <= PreTriggerAmplitude);
	FallHoldTicks = ((Int32U)DataTable[REG_PULSE_FALL_HOLD_DURATION] * 1000u) / TIMER15_uS;
	FallDone = false;
	RegState = RS_Fall;
}
//-----------------------------------------

bool REGLTR_IsFallDone()
{
	return (RegState == RS_Fall) && FallDone;
}
//-----------------------------------------

void REGLTR_Init()
{
	IsMeasureOk = false;
	Qi = FollowingErrorCounter = VoltageErrCount = 0;
	MaxCurrentErrCount = 0;
	RINGBUF_ResetIcesAvg();
	RINGBUF_ResetUceAvg();
	FollowingErrThreshold = (CONTROL_MeasureType == MT_ST_TestLoad) ?
								DataTable[REG_RGLTR_ST_ERR_THRESH] : DataTable[REG_RGLTR_FOLLOWING_ERR_THRESH];
	FollowingErrLimit = DataTable[REG_RGLTR_FOLLOWING_ERR_LIMIT];
	VoltagErrThreshold = DataTable[REG_VOLTAGE_ERR_THRESH];
	VoltagErrLimit = DataTable[REG_VOLTAGE_ERR_COUNT_LIMIT];
	MaxCurrentErrLimit = (Int16U)DataTable[REG_MAX_CURRENT_ERR_COUNT_LIMIT];
	MaxCurrentA = DataTable[REG_MAX_CURRENT_ICES] * 0.001f;
	PreTriggerAmplitude = DataTable[REG_PRETRIGGER_VOLTAGE];
	RawSetPoint = PreTriggerAmplitude;
	RegState = RS_Rise;
	FallDone = false;
	RegulatorError = 0;

	switch(CONTROL_MeasureType)
	{
		case MT_Ices:
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_ICES];
			break;

		case MT_ST_TestLoad:
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD];
			break;
	}

	Int32U PulseRiseMs = (Int32U)DataTable[REG_PULSE_RISE_DURATION];
	if (PulseRiseMs == 0u)
		PulseRiseMs = 1u;

	RiseRate = (PulseAmplitude - PreTriggerAmplitude) / (float)PulseRiseMs;
	VoltStep = RiseRate * TIMER15_uS * 0.001f;

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

Int32U REGLTR_CorrectionLogDACPoint()
{
	RGLTR_ErrorCheck();

	if(RegState == RS_Fall)
		return MEASURE_ConvertUset(RawSetPoint);

	Qp = RegulatorError * Kp;
	Qi += RegulatorError * Ki;

	float SetPoint = RawSetPoint + Qp + Qi;
	Int32U DACPoint = MEASURE_ConvertUset(SetPoint);

	REGLTR_StoreRegulatorDebug(Sample.Uce, Sample.Ices, RawSetPoint, Qp + Qi, RegulatorError, (float)DACPoint);

	return DACPoint;
}
//-----------------------------------------

void RGLTR_ErrorCheck()
{
	switch(RegState)
	{
		case RS_FlatTop:
			RegulatorError = RawSetPoint - Sample.Uce;
			float RelativeAmplitudeAbsErr = fabsf(PulseAmplitude - Sample.Uce) / PulseAmplitude;

			if (CONTROL_MeasureType == MT_Ices && Sample.Ices > MaxCurrentA)
			{
				MaxCurrentErrCount++;
				if (MaxCurrentErrCount > MaxCurrentErrLimit)
				{
					REGLTR_AbortWithFall(SS_MaxCurrentErr);
					break;
				}
			}
			else
				MaxCurrentErrCount = 0;

			if(RelativeAmplitudeAbsErr <= VoltagErrThreshold)
			{
				if(CONTROL_MeasureType == MT_Ices)
				{
					RINGBUF_AddNewSampleIces(Sample.Ices);
					RINGBUF_AddNewSampleUce(Sample.Uce);
				}
				IsMeasureOk = true;
				VoltageErrCount = 0;
			}
			else
			{
				VoltageErrCount++;
				if(VoltageErrCount > VoltagErrLimit)
					REGLTR_AbortWithFall(SS_VoltageErr);
			}
			break;

		case RS_Rise:
		default:
			RegulatorError = RawSetPoint - Sample.Uce;
			break;
	}

	if(RegState == RS_Fall)
		return;

	if(RawSetPoint > 0.0f)
	{
		float RegulatorRelativeAbsError = fabsf(RegulatorError) / RawSetPoint;
		if(RegulatorRelativeAbsError > FollowingErrThreshold)
		{
			if(FollowingErrorCounter < FollowingErrLimit)
				FollowingErrorCounter++;
			else
				REGLTR_AbortWithFall(SS_FollowingErr);
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

	float FallingPart;
	if(RiseRate > 0.0f)
	{
		RisingPart = (PulseAmplitude - PreTriggerAmplitude) / RiseRate;
		FallingPart = RisingPart + DataTable[REG_PULSE_FALL_HOLD_DURATION];
	}
	else
		RisingPart = FallingPart = 0;
	switch(CONTROL_MeasureType)
	{
		case MT_Ices:
			SumTicks = (RisingPart + FallingPart + DataTable[REG_PULSE_DURATION]) * MsToMks / TIMER15_uS;
			break;

		case MT_ST_TestLoad:
			SumTicks = (RisingPart + FallingPart + DataTable[REG_ST_TL_FLATTOP_DURATION]) * MsToMks / TIMER15_uS;
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

static bool REGLTR_IsPulseSubState()
{
	return CONTROL_SubState == SS_RegulatorProcess
			|| CONTROL_SubState == SS_RegulatorProcessSelfTest
			|| CONTROL_SubState == SS_FollowingErr
			|| CONTROL_SubState == SS_VoltageErr
			|| CONTROL_SubState == SS_CurrentErr
			|| CONTROL_SubState == SS_MaxCurrentErr;
}
//-----------------------------------------

static void REGLTR_AbortWithFall(DeviceSubState ErrorSubState)
{
	DataTable[REG_DIAG_CURRENT] = Sample.Ices;
	DataTable[REG_DIAG_VOLTAGE] = Sample.Uce;
	CONTROL_SetDeviceSubState(ErrorSubState);
	if(RegState != RS_Fall)
		REGLTR_StartFall();
}
//-----------------------------------------

void REGLTR_StartProcess()
{
	DMA_ChannelEnable(DMA2_Channel1, true);
	DMA_ChannelEnable(DMA2_Channel5, true);

	TIM_Start(TIM15);
}
//------------------------------------

void REGLTR_StopProcess()
{
	LL_WriteDAC24(0);
	TIM_Stop(TIM15);

	DMA_ChannelEnable(DMA2_Channel1, false);
	DMA_ChannelEnable(DMA2_Channel5, false);
}
//------------------------------------
