// Include
//
#include "Global.h"
#include "Controller.h"
#include "Interrupts.h"
#include "SysConfig.h"
#include "BoardConfig.h"


// Forward functions
//
void SysClk_Config();
void IO_Config();
void UART_Config();
void Timer2_Config();
void WatchDog_Config();

// Functions
//
int main()
{
	// Set request flag if firmware update is required
	if(*ProgramAddressStart == 0xFFFFFFFF || BOOT_LOADER_VARIABLE == BOOT_LOADER_REQUEST)
		WaitForFWUpload = TRUE;
	
	// Init peripherals
	SysClk_Config();
	IO_Config();
	UART_Config();
	Timer2_Config();
	WatchDog_Config();
	
	// Init controller
	CONTROL_Init();
	
	// Infinite cycle
	while(true)
		CONTROL_Idle();
	
	return 0;
}
//--------------------------------------------

void SysClk_Config()
{
	RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//--------------------------------------------

void IO_Config()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	
	//Выходы
	GPIO_Config(LED_BLINK_PORT, LED_BLINK_PIN, Output, PushPull, HighSpeed, NoPull);
	// выключение RCON
	GPIO_Config(GPIOA, Pin_11, Output, PushPull, HighSpeed, NoPull);
	GPIO_Bit_Rst(GPIOA, Pin_11);
	
	//Альтернативные функции портов
	GPIO_Config(GPIOB, Pin_6, AltFn, PushPull, HighSpeed, NoPull); //PB6(USART1 TX)
	GPIO_AltFn(GPIOB, Pin_6, AltFn_7);
	
	GPIO_Config(GPIOB, Pin_7, AltFn, PushPull, HighSpeed, NoPull); //PB7(USART1 RX)
	GPIO_AltFn(GPIOB, Pin_7, AltFn_7);
	
	GPIO_Config(GPIOB, Pin_8, AltFn, PushPull, HighSpeed, NoPull); //PB8(CAN RX)
	GPIO_AltFn(GPIOB, Pin_8, AltFn_9);
	
	GPIO_Config(GPIOB, Pin_9, AltFn, PushPull, HighSpeed, NoPull); //PB9(CAN TX)
	GPIO_AltFn(GPIOB, Pin_9, AltFn_9);
	
}
//--------------------------------------------

void UART_Config()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//--------------------------------------------

void Timer2_Config()
{
	TIM_Clock_En(TIM_2);
	TIM_Config(TIM2, SYSCLK, TIMER2_uS);
	TIM_Interupt(TIM2, 0, true);
	TIM_Start(TIM2);
}
//--------------------------------------------

void WatchDog_Config()
{
	IWDG_Config();
	IWDG_ConfigureFastUpdate();
}
//--------------------------------------------
