// Includes
#include "Measurement.h"
#include "ConvertUtils.h"
#include "Board.h"
#include "DiscreteOpAmp.h"
#include "Global.h"
#include "LowLevel.h"

// Variables
Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
Int16U MEASURE_ADC_VoltageRaw[ADC_DMA_BUFF_SIZE];

// Functions prototypes
Int16U MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize);
Int16U MEASURE_DMAExtractVoltage();
void MEASURE_StartNewSampling();

// Functions
//
float MEASURE_SampleVoltage()
{
	float Voltage;

	Voltage = CU_ADCtoV(MEASURE_DMAExtractVoltage());
	MEASURE_StartNewSampling();

	return Voltage;
}
//-----------------------------------------------

Int16U MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize)
{
	Int32U AverageData = 0;

	for(int i = 0; i < ArraySize; i++)
		AverageData += *(InputArray + i);

	return (Int16U)((float)AverageData / ArraySize);
}
//-----------------------------------------------

Int16U MEASURE_DMAExtractVoltage()
{
	return MEASURE_DMAExtractX(&MEASURE_ADC_VoltageRaw[1], ADC_DMA_BUFF_SIZE - 1);
}
//-----------------------------------------------

void MEASURE_DMABuffersClear()
{
	for(int i = 0; i < ADC_DMA_BUFF_SIZE; i++)
		MEASURE_ADC_VoltageRaw[i] = 0;
}
//-----------------------------------------------

void MEASURE_StartNewSampling()
{
	DMA_TransferCompleteReset(DMA1, DMA_TRANSFER_COMPLETE);
	ADC_SamplingStart(ADC1);
}
//-----------------------------------------------


