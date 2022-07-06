#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H


// Flash loader options
#define BOOT_LOADER_VARIABLE			(*((volatile uint32_t *)0x20000000))
#define BOOT_LOADER_REQUEST				0x12345678
#define BOOT_LOADER_MAIN_PR_ADDR		0x08008000
//-----------------------------------------------

// System clock
#define SYSCLK							70000000	// Тактовая частота системной шины процессора
#define QUARTZ_FREQUENCY				20000000	// Частота кварца
// ----------------------------------------------

// USART
#define USART_BAUDRATE					115200		// Скорость USART
#define USART_FIFOlen					32			// Длина FIFO USART
// ----------------------------------------------

// Timers
#define TIMER7_uS						1000		// в мкс
#define TIMER6_uS						150			// в мкс
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// SPI
#define SPI2_BAUDRATE_BITS				0x1			// Биты задания битрейта SPI
#define SPI2_LSB_FIRST					false		// Передача младшим битов вперед
// ----------------------------------------------

// ADC
#define DMA_ADC_DUT_I_CHANNEL			DMA1_Channel1	// Канал DMA для АЦП тока
#define DMA_ADC_DUT_U_CHANNEL			DMA2_Channel5	// Канал DMA для АЦП напряжения
#define ADC1_CURRENT_CHANNEL			1				// АЦП1 номер канала тока
#define ADC3_VOLTAGE_CHANNEL			1				// АЦП3 номер канала напряжения
#define ADC_DMA_BUFF_SIZE				5				// Количество данных для DMA
// ----------------------------------------------

#endif // __SYSCONFIG_H
