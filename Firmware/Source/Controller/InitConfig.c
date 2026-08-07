#include "InitConfig.h"
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"
#include "ZwSPI.h"
#include "Regulator.h"

// Forward functions
void INITCFG_GeneralADC(ADC_TypeDef* ADCx, Int16U Channel, Int32U Trigger);

// Functions
//
Boolean INITCFG_SysClk()
{
	return RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//------------------------------------------------

void INITCFG_IO()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	
	// Аналаговые порты
	GPIO_InitAnalog(GPIO_ANLG_U_CAP);
	GPIO_InitAnalog(GPIO_ANLG_UG);
	GPIO_InitAnalog(GPIO_ANLG_IG);
	
	// Входы
	GPIO_InitInput(GPIO_SAFETY, NoPull);

	// Выходы
	GPIO_InitPushPullOutput(GPIO_LED_BOARD);
	GPIO_InitPushPullOutput(GPIO_SYNC);
	GPIO_InitPushPullOutput(GPIO_SPI_NSS);
	GPIO_InitPushPullOutput(GPIO_SW_FAN);
	GPIO_InitPushPullOutput(GPIO_SW_IND);
	GPIO_InitPushPullOutput(GPIO_SW_SYNC);
	GPIO_InitPushPullOutput(GPIO_RCON);
	GPIO_InitPushPullOutput(GPIO_RSS);
	GPIO_InitPushPullOutput(GPIO_ROUT_LCAU);
	GPIO_InitPushPullOutput(GPIO_RDIS);
	GPIO_InitPushPullOutput(GPIO_ROUT_LCTU);
	GPIO_InitPushPullOutput(GPIO_RST1);
	GPIO_InitPushPullOutput(GPIO_RST2);
	GPIO_InitPushPullOutput(GPIO_RMES1);
	GPIO_InitPushPullOutput(GPIO_RMES2);
	GPIO_InitPushPullOutput(GPIO_RMES3);
	GPIO_InitPushPullOutput(GPIO_RMES4);
	GPIO_InitPushPullOutput(GPIO_RMES5);

	GPIO_SetState(GPIO_LED_BOARD, false);
	GPIO_SetState(GPIO_SYNC, false);
	GPIO_SetState(GPIO_SPI_NSS, true);
	GPIO_SetState(GPIO_SW_FAN, false);
	GPIO_SetState(GPIO_SW_IND, false);
	GPIO_SetState(GPIO_SW_SYNC, true);
	GPIO_SetState(GPIO_RCON, false);
	GPIO_SetState(GPIO_RSS, false);
	GPIO_SetState(GPIO_ROUT_LCAU, false);
	GPIO_SetState(GPIO_RDIS, false);
	GPIO_SetState(GPIO_ROUT_LCTU, false);
	GPIO_SetState(GPIO_RST1, false);
	GPIO_SetState(GPIO_RST2, false);
	GPIO_SetState(GPIO_RMES1, false);
	GPIO_SetState(GPIO_RMES2, false);
	GPIO_SetState(GPIO_RMES3, false);
	GPIO_SetState(GPIO_RMES4, false);
	GPIO_SetState(GPIO_RMES5, false);

	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_CAN_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_UART_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_SPI_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI_MOSI, AltFn_5);
}
//------------------------------------------------

void INITCFG_UART()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//------------------------------------------------

void INITCFG_GeneralADC(ADC_TypeDef* ADCx, Int16U Channel, Int32U Trigger)
{
	ADC_Calibration(ADCx);
	ADC_Enable(ADCx);
	ADC_TrigConfig(ADCx, Trigger, RISE);

	ADC_ChannelSeqReset(ADCx);
	for(int i = 1; i <= ADC_SEQ_LENGTH; i++)
		ADC_ChannelSet_Sequence(ADCx, Channel, i);
	ADC_ChannelSeqLen(ADCx, ADC_SEQ_LENGTH);

	ADC_ChannelSet_SampleTime(ADCx, Channel, ADC_SAMPLE_TIME);
	ADC_DMAConfigWithAutDLY(ADCx);
	ADC_SamplingStart(ADCx);
}
//------------------------------------------------

void INITCFG_ADC()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	RCC_ADC_Clk_EN(ADC_34_ClkEN);

	INITCFG_GeneralADC(ADC1, ADC1_CHANNEL_U_CAP, ADC12_TIM15_TRGO);
	INITCFG_GeneralADC(ADC2, ADC2_CHANNEL_IG, ADC12_TIM15_TRGO);
	INITCFG_GeneralADC(ADC3, ADC3_CHANNEL_UG, ADC34_TIM15_TRGO);
}
//------------------------------------------------

void INITCFG_Timer7()
{
	TIM_Clock_En(TIM_7);
	TIM_Config(TIM7, SYSCLK, TIMER7_uS);
	TIM_Interupt(TIM7, 0, true);
	TIM_Start(TIM7);
}
//------------------------------------------------

void INITCFG_Timer15()
{
	TIM_Clock_En(TIM_15);
	TIM_Config(TIM15, SYSCLK, TIMER15_uS);
	TIM_MasterMode(TIM15, MMS_UPDATE);
}
//------------------------------------------------

void INITCFG_WatchDog()
{
	IWDG_Config();
	IWDG_ConfigureSlowUpdate();
}
//------------------------------------------------

void INITCFG_ConfigCAN(Int16U NodeID)
{
	Int32U Mask = ((Int32U)NodeID) << CAN_SLAVE_NID_MPY;

	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, FALSE);
	NCAN_FIFOInterrupt(TRUE);
	NCAN_FilterInit(0, Mask, Mask);
}
//------------------------------------------------

void INITCFG_SPI()
{
	SPI_Init(SPI1, SPI_BAUDRATE_BITS, SPI_MSB_FIRST);
}
//------------------------------------------------

void INITCFG_GeneralDMA(DMA_Channel_TypeDef* DMA_ChannelX, uint32_t ArrayPointer, uint32_t ADCPointer)
{
	DMA_Reset(DMA_ChannelX);
	DMAChannelX_DataConfig(DMA_ChannelX, ArrayPointer, ADCPointer, ADC_SEQ_LENGTH);
	DMAChannelX_Config(DMA_ChannelX, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
			DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMA_Interrupt(DMA_ChannelX, DMA_TRANSFER_COMPLETE, 0, true);
	DMA_ChannelEnable(DMA_ChannelX, true);
}
//------------------------------------------------

void INITCFG_DMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);
	DMA_Clk_Enable(DMA2_ClkEN);

	INITCFG_GeneralDMA(DMA1_Channel1, (uint32_t)REGLTR_MemBuffIces, (uint32_t)(&ADC2->DR));
	INITCFG_GeneralDMA(DMA2_Channel5, (uint32_t)REGLTR_MemBuffUce, (uint32_t)(&ADC3->DR));
}
//------------------------------------------------
