// Include
#include "Interrupts.h"
//
#include "Controller.h"
#include "LowLevel.h"
#include "Board.h"
#include "SysConfig.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Logic.h"

// Functions
//
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

void TIM6_DAC_IRQHandler()
{
	if(TIM_StatusCheck(TIM6))
	{
		CONTROL_HighPriorityProcess();

		TIM_StatusClear(TIM6);
	}
}
//-----------------------------------------

void TIM7_IRQHandler()
{
	static uint16_t LED_BlinkTimeCounter = 0;

	if(TIM_StatusCheck(TIM7))
	{
		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}

		LOGIC_HandleFan(IsImpulse);
		CONTROL_HandleExternalLamp(IsImpulse);

		TIM_StatusClear(TIM7);
	}
}
//-----------------------------------------

void EXTI9_5_IRQHandler()
{
	static bool Flag = false;

	if (EXTI_FlagCheck(EXTI_6))
	{
		PAUSyncReceiveCounter++;

		(Flag) ? LL_ExtIndicationControl(false) : LL_ExtIndicationControl(true);
	}

	EXTI_FlagReset(EXTI_6);
}
//-----------------------------------------
