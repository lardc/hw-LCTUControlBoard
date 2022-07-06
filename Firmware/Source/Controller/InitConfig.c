#include "InitConfig.h"
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"
#include "Measurement.h"

// Functions
//
Boolean INITCFG_ConfigSystemClock()
{
	return RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//------------------------------------------------

void INITCFG_ConfigIO()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	RCC_GPIO_Clk_EN(PORTC);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_OPAMP_SYNC);
	GPIO_InitPushPullOutput(GPIO_OPAMP_LDAC);
	GPIO_InitPushPullOutput(GPIO_OSC_SYNC);
	GPIO_InitPushPullOutput(GPIO_PS_CTRL);
	GPIO_InitPushPullOutput(GPIO_U_RANGE);
	GPIO_InitPushPullOutput(GPIO_PAU_SHUNT);
	GPIO_InitPushPullOutput(GPIO_STST);
	GPIO_InitPushPullOutput(GPIO_COMM);
	GPIO_InitPushPullOutput(GPIO_FAN_CTRL);
	GPIO_InitPushPullOutput(GPIO_IND_CTRL);
	GPIO_InitPushPullOutput(GPIO_PAU_SYNC);

	// Входы
	GPIO_InitInput(GPIO_OSC_SYNC_CHK, NoPull);
	GPIO_InitInput(GPIO_PAU_SYNC_CHK, NoPull);

	// Начальная установка состояний выводов
	GPIO_SetState(GPIO_OPAMP_SYNC, true);
	GPIO_SetState(GPIO_OPAMP_LDAC, true);


	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_UART1_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART1_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_CAN1_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN1_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI1_DAT, AltFn_5);
}

//------------------------------------------------

void INITCFG_ConfigUART()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//------------------------------------------------

void INITCFG_ConfigTimer7()
{
	TIM_Clock_En(TIM_7);
	TIM_Config(TIM7, SYSCLK, TIMER7_uS);
	TIM_Interupt(TIM7, 2, true);
	TIM_Start(TIM7);
}
//------------------------------------------------

void INITCFG_ConfigTimer6()
{
	TIM_Clock_En(TIM_6);
	TIM_Config(TIM6, SYSCLK, TIMER6_uS);
	TIM_Interupt(TIM6, 1, true);
}
//------------------------------------------------

void INITCFG_ConfigWatchDog()
{
	IWDG_Config();
	IWDG_ConfigureSlowUpdate();
}
//------------------------------------------------

void INITCFG_ConfigADC()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	RCC_ADC_Clk_EN(ADC_34_ClkEN);

	// ADC1
	ADC_Calibration(ADC1);
	ADC_SoftTrigConfig(ADC1);
	ADC_ChannelSeqReset(ADC1);

	for (uint8_t i = 1; i <= ADC_DMA_BUFF_SIZE; ++i)
		ADC_ChannelSet_Sequence(ADC1, ADC1_CURRENT_CHANNEL, i);

	ADC_ChannelSeqLen(ADC1, ADC_DMA_BUFF_SIZE);
	ADC_DMAConfig(ADC1);
	ADC_Enable(ADC1);

	// ADC3
	ADC_Calibration(ADC3);
	ADC_SoftTrigConfig(ADC3);
	ADC_ChannelSeqReset(ADC3);

	for (uint8_t i = 1; i <= ADC_DMA_BUFF_SIZE; ++i)
		ADC_ChannelSet_Sequence(ADC3, ADC3_VOLTAGE_CHANNEL, i);

	ADC_ChannelSeqLen(ADC3, ADC_DMA_BUFF_SIZE);
	ADC_DMAConfig(ADC3);
	ADC_Enable(ADC3);
}
//------------------------------------------------

void INITCFG_ConfigDMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);
	DMA_Clk_Enable(DMA2_ClkEN);

	// DMA для АЦП тока DUT
	DMA_Reset(DMA_ADC_DUT_I_CHANNEL);
	DMAChannelX_Config(DMA_ADC_DUT_I_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
							DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMAChannelX_DataConfig(DMA_ADC_DUT_I_CHANNEL, (uint32_t)(&MEASURE_ADC_CurrentRaw[0]), (uint32_t)(&ADC1->DR), ADC_DMA_BUFF_SIZE);
	DMA_ChannelEnable(DMA_ADC_DUT_I_CHANNEL, true);

	// DMA для АЦП напряжения DUT
	DMA_Reset(DMA_ADC_DUT_U_CHANNEL);
	DMAChannelX_Config(DMA_ADC_DUT_U_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
							DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMAChannelX_DataConfig(DMA_ADC_DUT_U_CHANNEL, (uint32_t)(&MEASURE_ADC_VoltageRaw[0]), (uint32_t)(&ADC3->DR), ADC_DMA_BUFF_SIZE);
	DMA_ChannelEnable(DMA_ADC_DUT_U_CHANNEL, true);
}
//------------------------------------------------

void INITCFG_ConfigSPI()
{
	SPI_Init(SPI1, SPI1_BAUDRATE_BITS, SPI1_LSB_FIRST);
	SPI_InvertClockPolarity(SPI1, true);
}
//------------------------------------------------
