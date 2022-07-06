#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

// Includes
#include "SysConfig.h"
#include "ZwBase.h"

// Definitions
#define PULSE_ARR_MAX_LENGTH					300				// Количество точек оцифровки

// Variables
extern Int16U MEASURE_ADC_VoltageRaw[ADC_DMA_BUFF_SIZE];
extern Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];

// Functions
float MEASURE_SampleVoltage();
void MEASURE_DMABuffersClear();

#endif /* MEASUREMENT_H_ */
