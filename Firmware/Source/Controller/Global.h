#ifndef __GLOBAL_H
#define __GLOBAL_H

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define EP_WRITE_COUNT							0		// Количество массивов для записи
#define EP_COUNT								3		// Количество массивов для чтения
#define VALUES_x_SIZE							300		// Размер массивов
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем

// Параметры DiscreteOpAmp
#define DISOPAMP_CELL_VOLATGE_MAX				490		// V
#define DISOPAMP_TOTAL_CELL						3
#define DSIOPAMP_STACK_VOLTAGE_MAX				(DISOPAMP_CELL_VOLATGE_MAX * DISOPAMP_TOTAL_CELL)
//
#define COMMUTATION_DELAY						100		// мс
//
#define VOLTAGE_RANGE_0							0
#define VOLTAGE_RANGE_1							1

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)

#endif //  __GLOBAL_H
