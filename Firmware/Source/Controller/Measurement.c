// Includes
#include "Measurement.h"
#include "ConvertUtils.h"
#include "Board.h"
#include "DiscreteOpAmp.h"
#include "Global.h"
#include "LowLevel.h"
#include "Logic.h"


// Variables
Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
Int16U MEASURE_ADC_VoltageRaw[ADC_DMA_BUFF_SIZE];

// Functions prototypes
Int16U MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize);
Int16U MEASURE_DMAExtract(Int16U* InputArray);
void MEASURE_StartNewSampling();
float MEASURE_ExtractAveragedDatas(float* Buffer, Int16U BufferLength);
float MEASURE_GetLastSampledData(float* InputBuffer);

// Functions
//
MeasureSample MEASURE_Sample()
{
	MeasureSample Sample;

	Sample.Voltage = CU_ADCtoV(MEASURE_DMAExtract(&MEASURE_ADC_VoltageRaw[1]));
	Sample.Current = CU_ADCtoI(MEASURE_DMAExtract(&MEASURE_ADC_CurrentRaw[1]));
	MEASURE_StartNewSampling();

	return Sample;
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

Int16U MEASURE_DMAExtract(Int16U* InputArray)
{
	return MEASURE_DMAExtractX(InputArray, ADC_DMA_BUFF_SIZE - 1);
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
	DMA_TransferCompleteReset(DMA2, DMA_TRANSFER_COMPLETE);
	ADC_SamplingStart(ADC1);
	ADC_SamplingStart(ADC3);
}
//-----------------------------------------------

float MEASURE_GetAverageVoltage()
{
	return MEASURE_ExtractAveragedDatas(&RingBuffer_Voltage[0], MAF_BUFFER_LENGTH);
}
//-----------------------------

float MEASURE_GetAverageCurrent()
{
	float Current = MEASURE_ExtractAveragedDatas(&RingBuffer_Current[1], MAF_BUFFER_LENGTH);
	return (Current >= LCTU_CURRENT_MIN) ? Current : 0;
}
//-----------------------------

float MEASURE_GetLastSampledVoltage()
{
	return MEASURE_GetLastSampledData(&RingBuffer_Voltage[0]);
}
//-----------------------------

float MEASURE_GetLastSampledData(float* InputBuffer)
{
	Int16U Index;

	Index = RingBufferIndex - 1;
	Index &= MAF_BUFFER_INDEX_MASK;

	return *(InputBuffer + Index);
}
//-----------------------------

float MEASURE_ExtractAveragedDatas(float* Buffer, Int16U BufferLength)
{
	float Temp = 0;

	for(int i = 0; i < BufferLength; i++)
		Temp += *(Buffer + i) / BufferLength;

	return Temp;
}
//-----------------------------


