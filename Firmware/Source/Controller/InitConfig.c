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
	GPIO_InitPushPullOutput(GPIO_SYNC);
	GPIO_InitPushPullOutput(GPIO_PS_EN);

	// Начальная установка состояний выводов
	GPIO_SetState(GPIO_OPAMP_SYNC, true);
	GPIO_SetState(GPIO_OPAMP_LDAC, true);
	GPIO_SetState(GPIO_SYNC, false);
	GPIO_SetState(GPIO_PS_EN, true);


	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_UART1_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART1_TX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_SPI2_CLK, AltFn_5);
	GPIO_InitAltFunction(GPIO_ALT_SPI2_DAT, AltFn_5);
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

	// ADC1
	ADC_Calibration(ADC1);
	ADC_SoftTrigConfig(ADC1);
	ADC_ChannelSeqReset(ADC1);

	for (uint8_t i = 1; i <= ADC_DMA_BUFF_SIZE; ++i)
		ADC_ChannelSet_Sequence(ADC1, ADC1_VOLTAGE_CHANNEL, i);

	ADC_ChannelSeqLen(ADC1, ADC_DMA_BUFF_SIZE);
	ADC_DMAConfig(ADC1);
	ADC_Enable(ADC1);
}
//------------------------------------------------

void INITCFG_ConfigDMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);

	// DMA для АЦП напряжения на DUT
	DMA_Reset(DMA_ADC_DUT_U_CHANNEL);
	DMAChannelX_Config(DMA_ADC_DUT_U_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
							DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMAChannelX_DataConfig(DMA_ADC_DUT_U_CHANNEL, (uint32_t)(&MEASURE_ADC_VoltageRaw[0]), (uint32_t)(&ADC1->DR), ADC_DMA_BUFF_SIZE);
	DMA_ChannelEnable(DMA_ADC_DUT_U_CHANNEL, true);
}
//------------------------------------------------

void INITCFG_ConfigSPI()
{
	SPI_Init(SPI2, SPI2_BAUDRATE_BITS, SPI2_LSB_FIRST);
	SPI_InvertClockPolarity(SPI2, true);
}
//------------------------------------------------
