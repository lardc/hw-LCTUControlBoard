#include "Controller.h"
#include "InitConfig.h"
#include "SysConfig.h"
#include "Diagnostic.h"

int main()
{
	__disable_irq();
	SCB->VTOR = (uint32_t)BOOT_LOADER_MAIN_PR_ADDR;
	__enable_irq();
	
	// Настройка системной частоты тактирования
	INITCFG_SysClk();
	
	// Настройка портов
	INITCFG_IO();
	
	// Настройка UART
	INITCFG_UART();
	
	// Настройка системного счетчика
	INITCFG_Timer7();

	// Настройка таймера тактирования регулятора
	INITCFG_Timer15();

	// Настройка АЦП
	INITCFG_ADC();

	// Настройка ЦАП
	INITCFG_DAC1();
	
	// Настройка сторожевого таймера
	INITCFG_WatchDog();

	// Настройка SPI
	INITCFG_SPI();

	// Настройка DMA
	INITCFG_DMA();

	// Инициализация логики контроллера
	CONTROL_Init();
	
	// Фоновый цикл
	while(TRUE)
		CONTROL_Idle();
	
	return 0;
}
