#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Включение блока
#define ACT_DISABLE_POWER				2	// Выключение блока
#define ACT_CLR_FAULT					3	// Очистка всех fault
#define ACT_CLR_WARNING					4	// Очистка всех warning

#define ACT_DBG_OSC_PULSE_SYNC			50	// Одиночный импульс в линию OSC_SYNC
#define ACT_DBG_PAU_PULSE_SYNC			51	// Одиночный импульс в линию PAU_SYNC
// Отладочные команды с использованием регистра REG_DBG
#define ACT_DBG_OPAMP_SET_VOLTAGE		52	// Установка напряжения
#define ACT_DBG_POWER_SUPPLY			53	// Управление источниками напряжения
#define ACT_DBG_VOLTAGE_RANGE_SET		54	// Установка диапазона напряжения
#define ACT_DBG_PAU_SHUNT				55	// Управление шунтированием PAU
#define ACT_DBG_STST_COMM				56	// Управление коммутацией для режима самотестирования
#define ACT_DBG_OUTPUT_COMM				57	// Управлением коммутацией выхода
#define ACT_DBG_FAN_CTRL				58	// Управление вентилятором
#define ACT_DBG_IND_CTRL				59	// Управление внешней индикацией


#define ACT_START_TEST					100	// Старт измерения
#define ACT_STOP_TEST					101	// Стоп измерения
//
#define ACT_START_SELF_TEST				105	// Запуск самотестирования

#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_DAC_V_K						0	// Коэффициент грубой подстройки К
#define REG_DAC_V_B						1	// Смещение грубой подстройки B
//
#define REG_ADC_V_R0_P2					2	// Коэффициент тонкой подстройки Р2
#define REG_ADC_V_R0_P1					3	// Коэффициент тонкой подстройки Р1
#define REG_ADC_V_R0_P0					4	// Смещение тонкой подстройки Р0
#define REG_ADC_V_R0_K					5	// Коэффициент грубой подстройки K
#define REG_ADC_V_R0_B					6	// Смещение грубой подстройки B
//
#define REG_ADC_V_R1_P2					7	// Коэффициент тонкой подстройки Р2
#define REG_ADC_V_R1_P1					8	// Коэффициент тонкой подстройки Р1
#define REG_ADC_V_R1_P0					9	// Смещение тонкой подстройки Р0
#define REG_ADC_V_R1_K					10	// Коэффициент грубой подстройки K
#define REG_ADC_V_R1_B					11	// Смещение грубой подстройки B
//
#define REG_ADC_I_P2					12	// Коэффициент тонкой подстройки Р2
#define REG_ADC_I_P1					13	// Коэффициент тонкой подстройки Р1
#define REG_ADC_I_P0					14	// Смещение тонкой подстройки Р0
#define REG_ADC_I_K						15	// Коэффициент грубой подстройки K
#define REG_ADC_I_B						16	// Смещение грубой подстройки B
//
#define REG_REGULATOR_Kp				17	// Пропорциональный коэффициент регулятора
#define REG_REGULATOR_Ki				18	// Интегральный коэффициент регулятора
//
#define REG_PAU_CAN_ID					20	// CAN ID PAU
//
#define REG_PULSE_FRONT_WIDTH			30	// Длительность фронта импульса в (мс)
#define REG_REGULATOR_ALOWED_ERR		31	// Допустимая ошибка регулирования (%)
#define REG_SCOPE_STEP					32	// Сохранение измеренных данных с заданным шагом
#define REG_AFTER_PULSE_PAUSE			33	// Время выдержки перед началом следующего измерения (мс)
#define REG_REGULATOR_QI_MAX			34	// Максимально допустимое значение Qi регулятора
#define REG_FOLLOWING_ERR_CNT			35	// Счетчик выставления ошибки FollowingError
#define REG_FOLLOWING_ERR_MUTE			36	// Выключение слежения за ошибкой FollowingError
#define REG_PS_FIRST_START_TIME			37	// Время готовности системы питания при первом старте (мс)
#define REG_PS_PREPARE_TIME				38	// Время готовности системы питания после формирования импульса (мс)
#define REG_VOLTAGE_RANGE_THRESHOLD		39	// Порог переключения диапазона напряжения (В)
#define REG_FAN_CTRL					40	// Управление вентилятором
#define REG_FAN_OPERATE_PERIOD			41	// Период работы вентилятора (с)
#define REG_FAN_OPERATE_TIME			42	// Время работы вентилятора (с)
#define REG_PAU_SNC_DELAY				43	// Задержка синхронизации PAU с момента выхода на полку импульса (в тиках таймера регулятора)
#define REG_PAU_EMULATED				44	// Эмуляция работы PAU
#define REG_LAMP_CTRL					45	// Управление внешним индикатором
#define REG_SELF_TEST_ACTIVE			46	// Активация самотестирования при запуске блока
#define REG_SAFETY_ACTIVE				47	// Активация системы безопасности
#define REG_PAU_SYNC_STEP				48	// Период импульсов синхронизации (кратно периоду регулятора)
#define REG_PAU_SYNC_CNT				49	// Количество импульсов синхронизации
#define REG_PAU_SYNC_WIDTH				50	// Длительность импульса синхронизации (мкс)
#define REG_PAU_SYNC_TRACKING			51	// Отслеживание поступления испульсов синхронизации от PAU
#define REG_OSC_SYNC_WIDTH				52	// Длительность импульса синхронизации осциллографа (в тиках регулятора)
//

#define REG_TEST_VOLTAGE				128	// Уставка по напряжению, В
#define REG_LIMIT_CURRENT				129	// Отсечтка по току утечки (мА)
#define REG_PULSE_WIDTH					130	// Длительность импульса напряжения, мс

// Несохраняемы регистры чтения-записи
#define REG_DBG							150	// Отладочный регистр

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_SELF_TEST_OP_RESULT			198	// Регистр результата самотестирования
#define REG_SUB_STATE					199	// Регистр вспомогательного состояния

#define REG_RESULT_VOLTAGE				200	// Достигнутое напряжение во время теста (В)
#define REG_RESULT_CURRENT				201	// Измеренный ток утечки (мА)

#define REG_PAU_ERROR_CODE				210	// Ошибка интерфейса PAU: код ошибки
#define REG_PAU_FUNCTION				211	// Ошибка интерфейса PAU: код функции
#define REG_PAU_EXT_DATA				212	// Ошибка интерфейса PAU: расширенная информация
// -----------------------------
#define REG_FWINFO_SLAVE_NID			256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID			257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN				260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN			261	// Begining of the information string record


// PAU commands
#define ACT_PAU_FAULT_CLEAR				3	// Очистка fault
#define ACT_PAU_WARNING_CLEAR			4	// Очистка warning
//
#define ACT_PAU_PULSE_CONFIG			100	// Конфигурация PAU
// -----------------------------

// Регистры PAU
#define REG_PAU_CHANNEL					128	// Выбор канала измерения PAU
#define REG_PAU_RANGE					129	// Диапазон измерения тока (мА)
#define REG_PAU_MEASUREMENT_TIME		130	// Время измерения (мс)
//
#define REG_PAU_RESULT_CURRENT			200	// Измеренное значение тока
//
#define REG_PAU_DEV_STATE				192	// Состояние блока
#define REG_PAU_FAULT_REASON			193
#define REG_PAU_DISABLE_REASON			194
#define REG_PAU_WARNING					195
#define REG_PAU_PROBLEM					196
// -----------------------------


// Operation results
#define OPRESULT_NONE					0	// No information or not finished
#define OPRESULT_OK						1	// Operation was successful
#define OPRESULT_FAIL					2	// Operation failed

//  Fault and disable codes
#define DF_NONE							0
#define DF_FOLLOWING_ERROR				1
#define DF_CURRENT_MEASURING			2
#define DF_INTERFACE					3
#define DF_PAU							4
#define DF_PAU_CONFIG_TIMEOUT			5
#define DF_PAU_ABNORMAL_STATE			6

// Problem
#define PROBLEM_NONE					0
#define PROBLEM_FORCED_STOP				1
#define PROBLEM_SAFETY					2

//  Warning
#define WARNING_NONE					0
#define WARNING_OUTPUT_SHORT			1
#define WARNING_PAU						2

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	//  Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	//  Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	//  Неправильный ключ

// Endpoints
#define EP_VOLTAGE						1
#define EP_CURRENT						2
#define EP_REGULATOR_ERR				3

#endif //  __DEV_OBJ_DIC_H
