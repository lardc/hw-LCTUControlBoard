// Header
#include "RingBuffer.h"

// Variables
//
static float IcesBuffer[ICES_AVG_BUF_SIZE];
static Int16U IcesIndex = 0;
static Int16U IcesCount = 0;
static float IcesSum = 0.0f;

// Functions
//
void RINGBUF_ResetIcesAvg()
{
	IcesIndex = IcesCount = 0;
	IcesSum = 0.0f;
	for(Int16U i = 0; i < ICES_AVG_BUF_SIZE; ++i)
		IcesBuffer[i] = 0.0f;
}
//-----------------------------------------

void RINGBUF_AddNewSampleIces(float Ig)
{
	if(IcesCount >= ICES_AVG_BUF_SIZE)
		IcesSum -= IcesBuffer[IcesIndex];
	else
		IcesCount++;

	IcesBuffer[IcesIndex] = Ig;
	IcesSum += Ig;
	IcesIndex++;
	if(IcesIndex >= ICES_AVG_BUF_SIZE)
		IcesIndex = 0;
}
//-----------------------------------------

float RINGBUF_GetIcesAvg()
{
	if(IcesCount == 0)
		return 0;
	return IcesSum / (float)IcesCount;
}
//-----------------------------------------

Int16U RINGBUF_GetIcesAvgCount()
{
	return IcesCount;
}
//-----------------------------------------
