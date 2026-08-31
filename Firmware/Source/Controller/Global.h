#ifndef __GLOBAL_H
#define __GLOBAL_H

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS			1000	// Таймаут интерфейса SCCI (в мс)
#define EP_WRITE_COUNT				0		// Количество массивов для записи
#define EP_COUNT					0		// Количество массивов для чтения
#define FEP_COUNT					7		// Количество массивов для чтения типа float
#define ENABLE_LOCKING				FALSE	// Защита NV регистров паролем

// Временные параметры
#define TIME_LED_BLINK				500		// Мигание светодиодом (в мс)
#define TIME_EXT_LED_BLINK			500		// Время загорания внешнего светодиода (в мс)
#define TIME_FAULT_LED_BLINK		250		// Мигание светодиодом в состоянии Fault
#define TIME_SPI_DELAY				5		// Задержка интерфейса SPI
#define TIME_INIT_48V_TIMER			10		// Время для выставления 48v на плате, в мс
#define TIME_START_FLAT				10		// Время начальной полки перед формированием, в мс
#define TIME_ACTIVATION_TIMEOUT		20000	// Таймаут заряда накопителя, в мс

#define VALUES_EXT_INFO_SIZE		300
#define VALUES_DEBUG_RGLTR_SIZE		600		// Размер отладочного буфера регулятора

#endif //  __GLOBAL_H
