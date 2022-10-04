#ifndef __GLOBAL_H
#define __GLOBAL_H

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define EP_WRITE_COUNT							0		// Количество массивов для записи
#define EP_COUNT								3		// Количество массивов для чтения
#define FEP_COUNT								3		// Количество массивов для чтения типа float
#define VALUES_x_SIZE							300		// Размер массивов
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем

// Параметры DiscreteOpAmp
#define DISOPAMP_CELL_VOLATGE_MAX				520		// V
#define DISOPAMP_TOTAL_CELL						7
#define DSIOPAMP_STACK_VOLTAGE_MAX				(DISOPAMP_CELL_VOLATGE_MAX * DISOPAMP_TOTAL_CELL)
#define DISOPAMP_CURRENT_MAX					32		// мА
//
#define COMMUTATION_DELAY						7		// мс
#define POST_PULSE_DELAY						500		// мкс
#define EXT_LAMP_ON_STATE_TIME					500		// мс
#define LCTU_VOLTAGE_MAX						3300	// В
#define PAU_SYNC_WIDTH_CORR						2		// мкс
#define LCTU_CURRENT_MIN						1		// мА
//
#define VOLTAGE_RANGE_0							0
#define VOLTAGE_RANGE_1							1

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)
#define TIME_FAULT_LED_BLINK					250		// Мигание светодиодом в состоянии Fault

#endif //  __GLOBAL_H
