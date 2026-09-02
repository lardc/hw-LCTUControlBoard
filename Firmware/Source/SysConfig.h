#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "ZwADC.h"

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
#define TIMER15_uS						70			// в мкс
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// SPI
#define SPI_DAC_FREQ					1000000		// Частота SPI для внешнего DAC, Гц
#define SPI_BAUDRATE_BITS				0x5			// Делитель для целевой частоты SPI_DAC_FREQ
#define SPI_LSB_FIRST					false
#define SPI_MSB_FIRST					true
// ----------------------------------------------

// ADC
#define ADC_SEQ_LENGTH					10			// Размер выборки за один цикл
#define ADC_SAMPLE_TIME					ADC_SMPL_TIME_1_5	// Время сэмплинга
#define ADC1_CHANNEL_U_CAP				4
#define ADC2_CHANNEL_IG					1
#define ADC3_CHANNEL_UG					12
// ----------------------------------------------

#endif // __SYSCONFIG_H
