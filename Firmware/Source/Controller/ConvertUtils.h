#ifndef CONVERTUTILS_H_
#define CONVERTUTILS_H_

// Includes
#include "ZwBase.h"

// Functions
Int16U CU_VtoDAC(float Voltage);
float CU_ADCtoV(Int16U Data);
float CU_ADCtoI(Int16U Data);
void CU_LoadConvertParams(bool VoltageRange);

#endif /* CONVERTUTILS_H_ */
