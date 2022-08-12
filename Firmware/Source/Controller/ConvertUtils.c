// Includes
//
#include "ConvertUtils.h"
#include "Global.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Measurement.h"

// Definitions
#define DAC_MAX_VAL		0x0FFF

// Structs
typedef struct __DisOpAmpConvertParams
{
	float K;
	Int16S B;
}DisOpAmpConvertParams;

typedef struct __MeasurementConvertParams
{
	float P2;
	float P1;
	float P0;
	float K;
	float B;
}MeasurementConvertParams;

// Variables
DisOpAmpConvertParams DisOpAmpVParams;
MeasurementConvertParams MeasureVParams;
MeasurementConvertParams MeasureIParams;
//

// Functions prototypes
Int16U CU_XtoDAC(float Value, DisOpAmpConvertParams Coefficients);
float CU_ADCtoX(Int16U Data, MeasurementConvertParams* Coefficients);


// Functions
//
Int16U CU_XtoDAC(float Value, DisOpAmpConvertParams Coefficients)
{
	Value = (Value + Coefficients.B) * Coefficients.K;

	if(Value < 0)
		Value = 0;

	return (Int16U)Value;
}
//-----------------------------

Int16U CU_VtoDAC(float Voltage)
{
	Int16U Temp;

	Temp = CU_XtoDAC(Voltage, DisOpAmpVParams);

	if (Temp > DAC_MAX_VAL)
		return DAC_MAX_VAL;
	else
		return Temp;
}
//-----------------------------

float CU_ADCtoX(Int16U Data, MeasurementConvertParams* Coefficients)
{
	float Temp;

	Temp = Data * Coefficients->K + Coefficients->B;
	Temp = Temp * Temp * Coefficients->P2 + Temp * Coefficients->P1 + Coefficients->P0;

	return Temp;
}
//-----------------------------

float CU_ADCtoV(Int16U Data)
{
	return CU_ADCtoX(Data, &MeasureVParams);
}
//-----------------------------

float CU_ADCtoI(Int16U Data)
{
	return CU_ADCtoX(Data, &MeasureIParams);
}
//-----------------------------

void CU_LoadConvertParams(bool VoltageRange)
{
	// Параметры преобразования напряжения
	DisOpAmpVParams.K = DataTable[REG_DAC_V_K] / 1000;
	DisOpAmpVParams.B = DataTable[REG_DAC_V_B];

	if(VoltageRange == VOLTAGE_RANGE_0)
	{
		MeasureVParams.P2 = DataTable[REG_ADC_V_R0_P2] / 1e6;
		MeasureVParams.P1 = DataTable[REG_ADC_V_R0_P1] / 1000;
		MeasureVParams.P0 = DataTable[REG_ADC_V_R0_P0] / 10;
		MeasureVParams.K = DataTable[REG_ADC_V_R0_K] / 1000;
		MeasureVParams.B = DataTable[REG_ADC_V_R0_B] / 10;
	}
	else
	{
		MeasureVParams.P2 = DataTable[REG_ADC_V_R1_P2] / 1e6;
		MeasureVParams.P1 = DataTable[REG_ADC_V_R1_P1] / 1000;
		MeasureVParams.P0 = DataTable[REG_ADC_V_R1_P0] / 10;
		MeasureVParams.K = DataTable[REG_ADC_V_R1_K] / 1000;
		MeasureVParams.B = DataTable[REG_ADC_V_R1_B] / 10;
	}

	MeasureIParams.P2 = DataTable[REG_ADC_I_P2] / 1e6;
	MeasureIParams.P1 = DataTable[REG_ADC_I_P1] / 1000;
	MeasureIParams.P0 = DataTable[REG_ADC_I_P0] / 10;
	MeasureIParams.K = DataTable[REG_ADC_I_K] / 1000;
	MeasureIParams.B = DataTable[REG_ADC_I_B] / 10;
}
//-----------------------------

