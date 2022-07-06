#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Включение блока
#define ACT_DISABLE_POWER				2	// Выключение блока
#define ACT_CLR_FAULT					3	// Очистка всех fault
#define ACT_CLR_WARNING					4	// Очистка всех warning

#define ACT_DBG_PULSE_SYNC				50	// Одиночный импульс в линию SYNC
#define ACT_DBG_SET_VOLTAGE				51	// Установка напряжения
#define ACT_DBG_POWER_SUPPLY			52	// Управление источниками напряжения

#define ACT_START_PROCESS				100	// Старт измерения
#define ACT_STOP_PROCESS				101	// Стоп измерения

#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_PULSE_FRONT_WIDTH			0	// Длительность фронта импульса в (мс)
#define REG_REGULATOR_ALOWED_ERR		1	// Допустимая ошибка регулирования (% * 10)
#define REG_SCOPE_STEP					2	// Сохранение измеренных данных с заданным шагом
#define REG_AFTER_PULSE_PAUSE			3	// Время выдержки перед началом следующего измерения (мс)
#define REG_REGULATOR_QI_MAX			4	// Максимально допустимое значение Qi регулятора
#define REG_FOLLOWING_ERR_CNT			5	// Счетчик выставления ошибки FollowingError
#define REG_FOLLOWING_ERR_MUTE			6	// Выключение слежения за ошибкой FollowingError
//
#define REG_DAC_V_K						10	// Коэффициент грубой подстройки К
#define REG_DAC_V_B						11	// Смещение грубой подстройки B
//
#define REG_ADC_V_P2					12	// Коэффициент тонкой подстройки Р2 х1е6
#define REG_ADC_V_P1					13	// Коэффициент тонкой подстройки Р1 х1000
#define REG_ADC_V_P0					14	// Смещение тонкой подстройки Р0
#define REG_ADC_V_K						15	// Коэффициент грубой подстройки K
#define REG_ADC_V_B						16	// Смещение грубой подстройки B
//
#define REG_REGULATOR_Kp				17	// Пропорциональный коэффициент регулятора
#define REG_REGULATOR_Ki				18	// Интегральный коэффициент регулятора

#define REG_VOLTAGE_SETPOINT			128	// Уставка по напряжению, В
#define REG_PULSE_WIDTH					129	// Длительность импульса напряжения, мс

// Несохраняемы регистры чтения-записи
#define REG_DBG							150	// Отладочный регистр

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_SUB_STATE					198	// Регистр вспомогательного состояния

#define REG_RESULT_VOLTAGE				200	// Достигнутое напряжение во время теста (В * 10)
// -----------------------------
#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record


// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

//  Fault and disable codes
#define DF_NONE							0
#define DF_FOLLOWING_ERROR				1

// Problem
#define PROBLEM_NONE					0

//  Warning
#define WARNING_NONE					0

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	//  Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	//  Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	//  Неправильный ключ

// Endpoints
#define EP_VOLTAGE						1
#define EP_REGULATOR_ERR				2

#endif //  __DEV_OBJ_DIC_H
