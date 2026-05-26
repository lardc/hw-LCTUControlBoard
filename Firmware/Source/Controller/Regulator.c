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
Int16U REGLTR_MemBuffUg[ADC_SEQ_LENGTH];
Int16U REGLTR_MemBuffUPot[ADC_SEQ_LENGTH];
Int16U REGLTR_MemBuffIg[ADC_SEQ_LENGTH];
static float Kp, Ki, KiI, KpI, Qi = 0, FollowingErrThreshold, VoltagErrThreshold, CurrentErrThreshold;
static Int16U FollowingErrLimit, VoltagErrLimit, VoltageErrCount, CurrentErrLimit,CurrentErrCount ,ScalingCoef, ScalingCounter;
static Int16U FollowingErrorCounterUpot = 0,FollowingErrorCounter = 0;;
static float PulseAmplitude, DesiredCurrent, RiseRate;
float RawSetPoint = 0;
float VoltStep = 0, Qp = 0;
float RegulatorError = 0, RegulatorErrorUpot = 0;
RegulatorState RegState = RS_None;
volatile bool IsMeasureOk = false;

volatile SamplingResult Sample = {0};

// Forward functions
SamplingResult REGLTR_GetSample();
void REGLTR_StoreRegulatorDebug(float Ug, float UPot, float Ig, float Setpoint, float Correction, float Error, float DACRaw);
Int16U REGLTR_CorrectionLogDACPoint();
void RGLTR_ErrorCheck();
Int16U REGLTR_GetScalingCoef();

// Functions
void REGLTR_Process()
{
	if (CONTROL_SubState != SS_RegulatorProcess && CONTROL_SubState != SS_RegulatorProcessUgeth
			&& CONTROL_SubState != SS_RegulatorProcessSelfTest)
		return;

	Int16U DACSetpoint;
	Sample = REGLTR_GetSample();

	switch(RegState)
	{
		case RS_Rise:
			{
				RawSetPoint += VoltStep;
				if(CONTROL_MeasureType == MT_Ugeth)
				{
					if(Sample.Ig >= DesiredCurrent)
						RegState = RS_FlatTopUgeth;
					else if(RawSetPoint > PulseAmplitude)
						CONTROL_SetDeviceSubState(SS_VoltageNoCurrentErr);
				}
				else
				{
					if(RawSetPoint >= PulseAmplitude)
					{
						RawSetPoint = PulseAmplitude;
						RegState = RS_FlatTop;
					}
				}
				DACSetpoint = REGLTR_CorrectionLogDACPoint();
				LL_WriteDAC(DACSetpoint);
			}
			break;

		case RS_FlatTopUgeth:
		case RS_FlatTop:
		default:
			DACSetpoint = REGLTR_CorrectionLogDACPoint();
			LL_WriteDAC(DACSetpoint);
			break;
	}
}
//-----------------------------------------

void REGLTR_Init()
{
	IsMeasureOk = false;
	Qi = FollowingErrorCounter = VoltageErrCount = CurrentErrCount = FollowingErrorCounterUpot = 0;
	RINGBUF_ResetIgesAvg();
	FollowingErrThreshold = (CONTROL_MeasureType == MT_ST_Upot || CONTROL_MeasureType == MT_ST_TestLoad) ?
								DataTable[REG_RGLTR_ST_ERR_THRESH ] : DataTable[REG_RGLTR_FOLLOWING_ERR_THRESH];
	FollowingErrLimit = DataTable[REG_RGLTR_FOLLOWING_ERR_LIMIT];
	VoltagErrThreshold = DataTable[REG_VOLTAGE_ERR_THRESH];
	VoltagErrLimit = DataTable[REG_VOLTAGE_ERR_COUNT_LIMIT];
	CurrentErrThreshold = DataTable[REG_CURRENT_ERR_THRESH];
	CurrentErrLimit = DataTable[REG_CURRENT_ERR_COUNT_LIMIT];
	RawSetPoint = 0;
	RegState = RS_Rise;
	RegulatorError = 0;
	RegulatorErrorUpot = 0;

	switch(CONTROL_MeasureType)
	{
		case MT_Iges:
			RiseRate = DataTable[REG_SLEW_RATE_IGES];
			PulseAmplitude = ABS(DataTable[REG_WORK_VOLTAGE_IGES]) * CONVERSION_REDUC_THOUSAND;
			break;

		case MT_Rth:
			RiseRate = DataTable[REG_SLEW_RATE_RTH];
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_RTH] * CONVERSION_REDUC_THOUSAND;
			break;

		case MT_Ugeth:
			RiseRate = DataTable[REG_SLEW_RATE_UGETH];
			PulseAmplitude = DataTable[REG_MAX_VOLTAGE_UGETH];
			DesiredCurrent = DataTable[REG_WORK_CURRENT_UGETH] * CONVERSION_REDUC_THOUSAND;
			break;

		case MT_ST_Upot:
			RiseRate = DataTable[REG_SLEW_RATE_ST_UPOT];
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_ST_UPOT] * CONVERSION_REDUC_THOUSAND;
			break;

		case MT_ST_TestLoad:
			RiseRate = DataTable[REG_SLEW_RATE_ST_TESTLOAD];
			PulseAmplitude = DataTable[REG_WORK_VOLTAGE_ST_TESTLOAD] * CONVERSION_REDUC_THOUSAND;
			break;

	}
	VoltStep = RiseRate * TIMER15_uS * CONVERSION_REDUC_THOUSAND;

	Kp = DataTable[REG_RGLTR_Kp];
	Ki = DataTable[REG_RGLTR_Ki];
	KpI = DataTable[REG_CURRENT_RGLTR_Kp];
	KiI = DataTable[REG_CURRENT_RGLTR_Ki];

	for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
	{
		REGLTR_MemBuffUg[i] = 0;
		REGLTR_MemBuffUPot[i] = 0;
		REGLTR_MemBuffIg[i] = 0;
	}
	DataTable[REG_DEBUG_SCALING_COEF] = ScalingCoef = REGLTR_GetScalingCoef();
	if(DataTable[REG_SCALING_MUTE])
		ScalingCoef = ScalingCounter = 1;
}
//-----------------------------------------

Int16U REGLTR_CorrectionLogDACPoint()
{
	RGLTR_ErrorCheck();

	Qp = RegulatorError * (RegState == RS_FlatTopUgeth ? KpI : Kp);
	Qi += RegulatorError * (RegState == RS_FlatTopUgeth ? KiI : Ki);

	float SetPoint = RawSetPoint + Qp + Qi;
	Int16U DACPoint = MEASURE_ConvertUset(SetPoint);

	REGLTR_StoreRegulatorDebug(Sample.Ug, Sample.UPot, Sample.Ig, RawSetPoint, Qp + Qi, RegulatorError, (float)DACPoint);

	return DACPoint;
}
//-----------------------------------------

void RGLTR_ErrorCheck()
{
	float CurrentErr, VoltageErr;
	switch(RegState)
	{
		case RS_FlatTopUgeth:
			{
				RegulatorError = DesiredCurrent - Sample.Ig;
				// Расчет метрологической ошибки по току
				CurrentErr = ABS(RegulatorError) / DesiredCurrent;
				if(CurrentErr < CurrentErrThreshold)
				{
					IsMeasureOk = true;
					CurrentErrCount = 0;
				}
				else
				{
					CurrentErrCount++;
					if(CurrentErrCount > CurrentErrLimit)
						CONTROL_SetDeviceSubState(SS_CurrentErr);
				}
			}
			break;

		case RS_FlatTop:
			{
				if(CONTROL_MeasureType == MT_ST_Upot)
					RegulatorErrorUpot = RawSetPoint - Sample.UPot;
				RegulatorError = RawSetPoint - Sample.Ug;
				// Расчет ошибки по напряжению
				VoltageErr = ABS(PulseAmplitude - Sample.Ug) / PulseAmplitude;

				if(VoltageErr < VoltagErrThreshold)
				{
					if(CONTROL_MeasureType == MT_Iges)
						RINGBUF_AddNewSampleIges(Sample.Ig);
					IsMeasureOk = true;
					VoltageErrCount = 0;
				}
				else
				{
					VoltageErrCount++;
					if(VoltageErrCount > VoltagErrLimit)
						CONTROL_SetDeviceSubState(SS_VoltageErr);
				}
			}
			break;

		case RS_Rise:
		default:
			if(CONTROL_MeasureType == MT_Ugeth || CONTROL_MeasureType == MT_ST_Upot)
				RegulatorErrorUpot = RawSetPoint - Sample.UPot;
			RegulatorError = RawSetPoint - Sample.Ug;
			break;
	}
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

	if(RegulatorErrorUpot && RegState != RS_FlatTopUgeth)
	{
		absError = ABS(RegulatorErrorUpot) / RawSetPoint;
		if(absError > FollowingErrThreshold)
			{
				if(FollowingErrorCounterUpot < FollowingErrLimit)
					FollowingErrorCounterUpot++;
				else
					CONTROL_SetDeviceSubState(SS_FollowingErr);
			}
			else
				FollowingErrorCounterUpot = 0;
	}
}
//-----------------------------------------

SamplingResult REGLTR_GetSample()
{
	SamplingResult t = {0};
	float sumUg = 0, sumUPot = 0, sumIg = 0;

	for (Int16U i = 0; i < ADC_SEQ_LENGTH; ++i)
	{
		sumUg   += REGLTR_MemBuffUg[i];
		sumUPot += REGLTR_MemBuffUPot[i];
		sumIg   += REGLTR_MemBuffIg[i];
	}

	float avgUg   = (float)sumUg   / ADC_SEQ_LENGTH;
	float avgUPot = (float)sumUPot / ADC_SEQ_LENGTH;
	float avgIg   = (float)sumIg   / ADC_SEQ_LENGTH;

	t.Ug   = MEASURE_Ug(avgUg);
	t.UPot = MEASURE_UPot(avgUPot);
	t.Ig   = MEASURE_I(avgIg, LOGIC_ChannelNumber);
	return t;
}
//-----------------------------------------

void REGLTR_StoreRegulatorDebug(float Ug, float UPot, float Ig, float Setpoint, float Correction, float Error, float DACRaw)
{
	if(ScalingCounter > 1)
		ScalingCounter--;
	else
	{
		ScalingCounter = ScalingCoef;
		if(CONTROL_Values_Counter < VALUES_DEBUG_RGLTR_SIZE)
		{
			CONTROL_RegulatorUg[CONTROL_Values_Counter] = Ug;
			CONTROL_RegulatorUpot[CONTROL_Values_Counter] = UPot;
			CONTROL_RegulatorIg[CONTROL_Values_Counter] = Ig;
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
	float FirstRelayTimer, FollowingRelaysTimer , RisingPart, SumTicks = 0;
	RisingPart = PulseAmplitude / RiseRate;
	switch(CONTROL_MeasureType)
	{
		case MT_Rth:
			FirstRelayTimer = (DataTable[REG_RELAY_SW_TIMER_RTH] > DataTable[REG_REGLTR_TIMER]) ?
								DataTable[REG_RELAY_SW_TIMER_RTH] : DataTable[REG_REGLTR_TIMER];
			FollowingRelaysTimer = DataTable[REG_RELAY_SW_TIMER_RTH];
			SumTicks = (RisingPart + FirstRelayTimer + 2 * FollowingRelaysTimer) * MsToMks / TIMER15_uS;
			break;

		case MT_Iges:
			FirstRelayTimer =(DataTable[REG_RELAY_SW_TIMER_IGES] > DataTable[REG_REGLTR_TIMER]) ?
							   DataTable[REG_RELAY_SW_TIMER_IGES] : DataTable[REG_REGLTR_TIMER];
			FollowingRelaysTimer = DataTable[REG_RELAY_SW_TIMER_IGES];
			SumTicks = (RisingPart + FirstRelayTimer + 2 * FollowingRelaysTimer) * MsToMks / TIMER15_uS;
			break;

		case MT_Ugeth:
			FirstRelayTimer = (DataTable[REG_RELAY_SW_TIMER_UGETH] > DataTable[REG_REGLTR_TIMER]) ?
								DataTable[REG_RELAY_SW_TIMER_UGETH] : DataTable[REG_REGLTR_TIMER];
			FollowingRelaysTimer = DataTable[REG_RELAY_SW_TIMER_UGETH];
			SumTicks = (RisingPart + FirstRelayTimer + FollowingRelaysTimer) * MsToMks / TIMER15_uS;
			break;

		case MT_ST_Upot:
			FirstRelayTimer = DataTable[REG_ST_UPOT_FLATTOP_DURATION] + DataTable[REG_REGLTR_TIMER];
			SumTicks = FirstRelayTimer * MsToMks / TIMER15_uS;
			break;

		case MT_ST_TestLoad:
			FirstRelayTimer = DataTable[REG_ST_TL_FLATTOP_DURATION] + DataTable[REG_REGLTR_TIMER];
			SumTicks = FirstRelayTimer * MsToMks / TIMER15_uS;
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
	DMA_ChannelEnable(DMA1_Channel1, true);
	DMA_ChannelEnable(DMA2_Channel1, true);
	DMA_ChannelEnable(DMA2_Channel5, true);

	TIM_Start(TIM15);
}
//------------------------------------

void REGLTR_StopProcess()
{
	LL_WriteDAC(0);
	TIM_Stop(TIM15);

	DMA_ChannelEnable(DMA1_Channel1, false);
	DMA_ChannelEnable(DMA2_Channel1, false);
	DMA_ChannelEnable(DMA2_Channel5, false);
}
//------------------------------------
