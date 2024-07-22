#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

// Includes
#include "SysConfig.h"
#include "ZwBase.h"

// Definitions
#define PULSE_ARR_MAX_LENGTH					300				// Количество точек оцифровки
#define MAF_BUFFER_LENGTH						64
#define MAF_BUFFER_INDEX_MASK					MAF_BUFFER_LENGTH - 1

// Types
typedef struct __MeasureSample
{
	float Current;
	float Voltage;
}MeasureSample;

// Variables
extern Int16U MEASURE_ADC_VoltageRaw[ADC_DMA_BUFF_SIZE];
extern Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];

// Functions
MeasureSample MEASURE_Sample();
void MEASURE_DMABuffersClear();
float MEASURE_GetLastSampledVoltage();
float MEASURE_GetAverageCurrent();
float MEASURE_GetAverageVoltage();

#endif /* MEASUREMENT_H_ */
