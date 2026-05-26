// Header
#include "RingBuffer.h"

// Variables
//
static float IgesBuffer[IGES_AVG_BUF_SIZE];
static Int16U IgesIndex = 0;
static Int16U IgesCount = 0;
static float IgesSum = 0.0f;

// Functions
//
void RINGBUF_ResetIgesAvg()
{
	IgesIndex = IgesCount = 0;
	IgesSum = 0.0f;
	for(Int16U i = 0; i < IGES_AVG_BUF_SIZE; ++i)
		IgesBuffer[i] = 0.0f;
}
//-----------------------------------------

void RINGBUF_AddNewSampleIges(float Ig)
{
	if(IgesCount >= IGES_AVG_BUF_SIZE)
		IgesSum -= IgesBuffer[IgesIndex];
	else
		IgesCount++;

	IgesBuffer[IgesIndex] = Ig;
	IgesSum += Ig;
	IgesIndex++;
	if(IgesIndex >= IGES_AVG_BUF_SIZE)
		IgesIndex = 0;
}
//-----------------------------------------

float RINGBUF_GetIgesAvg()
{
	if(IgesCount == 0)
		return 0;
	return IgesSum / (float)IgesCount;
}
//-----------------------------------------

Int16U RINGBUF_GetIgesAvgCount()
{
	return IgesCount;
}
//-----------------------------------------
