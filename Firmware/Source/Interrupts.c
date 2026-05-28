// Include
#include "Interrupts.h"
//
#include "Controller.h"
#include "LowLevel.h"
#include "Board.h"
#include "SysConfig.h"
#include "Global.h"
#include "Regulator.h"
#include "Measurement.h"

// Variables
volatile bool UgReady = false, IgReady = false;
volatile float UcapValue = 0.0f;

// Forward functions
void INT_GeneralDMAHandler(DMA_TypeDef* DMAx, uint32_t Channelx,volatile bool *Flag);

// Functions
void USART1_IRQHandler()
{
	if(ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//-----------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if(NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------

void TIM7_IRQHandler()
{
	static uint16_t LED_BlinkTimeCounter = 0;

	if(TIM_StatusCheck(TIM7))
	{
		UcapValue = MEASURE_Ucap((float)ADC1->DR);
		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}
		TIM_StatusClear(TIM7);
	}
}
//-----------------------------------------

void INT_ResetDMAFlags()
{
	UgReady = IgReady = false;
}
//-----------------------------------------

void INT_GeneralDMAHandler(DMA_TypeDef* DMAx, uint32_t Channelx,volatile bool *Flag)
{
	if(DMA_IsTransferComplete(DMAx, Channelx))
	{
		DMA_TransferCompleteReset(DMAx, Channelx);

		*Flag = true;

		if(UgReady && IgReady)
		{
			REGLTR_Process();
			CONTROL_WatchDogUpdate();
			INT_ResetDMAFlags();
		}
	}
}
//-----------------------------------------

void DMA1_Channel1_IRQHandler()
{
	INT_GeneralDMAHandler(DMA1, DMA_ISR_TCIF1, &IgReady);
}
//-----------------------------------------

void DMA2_Channel5_IRQHandler()
{
	INT_GeneralDMAHandler(DMA2, DMA_ISR_TCIF5, &UgReady);
}
//-----------------------------------------
