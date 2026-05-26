#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

// Команды
#define ACT_ENABLE_POWER				1	// Переход в состояние ожидания
#define ACT_DISABLE_POWER				2	// Отключение блока
#define ACT_FAULT_CLEAR					3	// Очистка fault
#define ACT_WARNING_CLEAR				4	// Очистка warning

#define ACT_DBG_EXT_INDICATION			10	// Управление внешней индикацией
#define ACT_DBG_SPI_WRITE_TWO_BYTES		11	// Запись двух байтов для отладки SPI
#define ACT_DBG_PULSE					12	// Запуск импульса в виде трапеции
#define ACT_DBG_SWITCH_POWER			13	// Диагностическое переключение питаний
#define ACT_DBG_SWITCH_RELAY			14 	// Диагностическое переключение реле токов
#define ACT_DBG_DAC_WRITE				15	// Прямая запись значения в ЦАП
#define ACT_DBG_48V_ON					16	// Включить питание 48 В
#define ACT_DBG_48V_OFF					17	// Выключить питание 48 В
#define ACT_DBG_24V_ON					18	// Включить питание 24 В
#define ACT_DBG_24V_OFF					19	// Выключить питание 24 В
#define ACT_DBG_SYNC					20	// Запуск синхронизации

#define ACT_START_MEASURE_RTH			100 // Запуск процесса измерения Rth
#define ACT_START_MEASURE_IGES			101	// Запуск процесса измерения Iges
#define ACT_START_MEASURE_UGETH			102	// Запуск процесса измерения Uge_th
#define ACT_START_SELFTEST_UPOT			103 // Запуск процесса самодиагностики потенциальных линий
#define ACT_START_SELFTEST_TESTLOAD		104 // Запуск процесса самодиагностики с тестовой нагрузкой
#define ACT_SAVE_TO_ROM					200	// Сохранение пользовательских данных во FLASH процессора
#define ACT_RESTORE_FROM_ROM			201	// Восстановление данных из FLASH
#define ACT_RESET_TO_DEFAULT			202	// Сброс DataTable в состояние по умолчанию

#define ACT_BOOT_LOADER_REQUEST			320	// Перезапуск процессора с целью перепрограммирования

#define ACT_FLASH_CNT_INIT_READ			334	// Перемещение указателя в область счетчиков
#define ACT_SET_COUNTER					336	// Установка значения счетчика
#define ACT_SAVE_COUNTERS				337	// Сохранить счетчики в памяти
#define ACT_ERASE_COUNTERS				338	// Удалить счетчики из памяти
#define ACT_FLASH_COUNTER_TO_EP			339 // Выполнить чтение массива из памяти счетчиков отработки в EP

#define ACT_JSON_INIT_READ				341	// Инициализация начала считывания JSON
#define ACT_JSON_TO_EP					342	// Выполнить чтение шаблона JSON в EP
// -----------------------------

// Регистры
// Сохраняемые регистры
#define REG_U_ADC_REF					0	// Опорное напряжение ADC/DAC, в В
//
#define REG_U_G_P2						1	// Коэффициент тонкой подстройки Р2
#define REG_U_G_P1						2	// Коэффициент тонкой подстройки Р1
#define REG_U_G_P0						3	// Смещение тонкой подстройки Р0
#define REG_U_G_K						4	// Коэффициент преобразования K
#define REG_U_G_B						5	// Коэффициент преобразования B
//
#define REG_U_POT_P2					6	// Коэффициент тонкой подстройки Р2
#define REG_U_POT_P1					7	// Коэффициент тонкой подстройки Р1
#define REG_U_POT_P0					8	// Смещение тонкой подстройки Р0
#define REG_U_POT_K						9	// Коэффициент преобразования K
#define REG_U_POT_B						10	// Коэффициент преобразования B
//
#define REG_I_0_P2						11	// Коэффициент тонкой подстройки Р2
#define REG_I_0_P1						12	// Коэффициент тонкой подстройки Р1
#define REG_I_0_P0						13	// Смещение тонкой подстройки Р0
#define REG_I_0_K						14	// Коэффициент преобразования K
#define REG_I_0_B						15	// Коэффициент преобразования B
#define REG_I_0_RSH						16	// Сопротивление шунта
//
#define REG_I_1_P2						17	// Коэффициент тонкой подстройки Р2
#define REG_I_1_P1						18	// Коэффициент тонкой подстройки Р1
#define REG_I_1_P0						19	// Смещение тонкой подстройки Р0
#define REG_I_1_K						20	// Коэффициент преобразования K
#define REG_I_1_B						21	// Коэффициент преобразования B
#define REG_I_1_RSH						22	// Сопротивление шунта
//
#define REG_I_2_P2						23	// Коэффициент тонкой подстройки Р2
#define REG_I_2_P1						24	// Коэффициент тонкой подстройки Р1
#define REG_I_2_P0						25	// Смещение тонкой подстройки Р0
#define REG_I_2_K						26	// Коэффициент преобразования K
#define REG_I_2_B						27	// Коэффициент преобразования B
#define REG_I_2_RSH						28	// Сопротивление шунта
//
#define REG_I_3_P2						29	// Коэффициент тонкой подстройки Р2
#define REG_I_3_P1						30	// Коэффициент тонкой подстройки Р1
#define REG_I_3_P0						31	// Смещение тонкой подстройки Р0
#define REG_I_3_K						32	// Коэффициент преобразования K
#define REG_I_3_B						33	// Коэффициент преобразования B
#define REG_I_3_RSH						34	// Сопротивление шунта
//
#define REG_I_4_P2						35	// Коэффициент тонкой подстройки Р2
#define REG_I_4_P1						36	// Коэффициент тонкой подстройки Р1
#define REG_I_4_P0						37	// Смещение тонкой подстройки Р0
#define REG_I_4_K						38	// Коэффициент преобразования K
#define REG_I_4_B						39	// Коэффициент преобразования B
#define REG_I_4_RSH						40	// Сопротивление шунта
//
#define REG_I_5_P2						41	// Коэффициент тонкой подстройки Р2
#define REG_I_5_P1						42	// Коэффициент тонкой подстройки Р1
#define REG_I_5_P0						43	// Смещение тонкой подстройки Р0
#define REG_I_5_K						44	// Коэффициент преобразования K
#define REG_I_5_B						45	// Коэффициент преобразования B
#define REG_I_5_RSH						46	// Сопротивление шунта
//
#define REG_I_6_P2						47	// Коэффициент тонкой подстройки Р2
#define REG_I_6_P1						48	// Коэффициент тонкой подстройки Р1
#define REG_I_6_P0						49	// Смещение тонкой подстройки Р0
#define REG_I_6_K						50	// Коэффициент преобразования K
#define REG_I_6_B						51	// Коэффициент преобразования B
#define REG_I_6_RSH						52	// Сопротивление шунта
//
#define REG_I_7_P2						53	// Коэффициент тонкой подстройки Р2
#define REG_I_7_P1						54	// Коэффициент тонкой подстройки Р1
#define REG_I_7_P0						55	// Смещение тонкой подстройки Р0
#define REG_I_7_K						56	// Коэффициент преобразования K
#define REG_I_7_B						57	// Коэффициент преобразования B
#define REG_I_7_RSH						58	// Сопротивление шунта
//
// 59
//
#define REG_CFG_NODE_ID					60	// Настройка CAN NodeID
//
#define REG_U_SET_P2					61	// Коэффициент тонкой подстройки Р2
#define REG_U_SET_P1					62	// Коэффициент тонкой подстройки Р1
#define REG_U_SET_P0					63	// Смещение тонкой подстройки Р0
#define REG_U_SET_K						64	// Коэффициент преобразования K
#define REG_U_SET_B						65	// Коэффициент преобразования B
//
#define REG_WORK_VOLTAGE_ST_UPOT		66	// Номинальное рабочее напряжение для самодиагностики потенциальных линий, мВ
#define REG_WORK_VOLTAGE_ST_TESTLOAD	67	// Номинальное рабочее напряжение для самодиагностики с тестовой нагрузкой, мВ
#define REG_WORK_VOLTAGE_RTH			68	// Номинальное рабочее напряжение для измерения Rth, мВ
//
#define REG_RANGE_I_0					69	// Диапазон тока для канала 0 - от 50 до 500 мА, в А
#define REG_RANGE_I_1					70	// Диапазон тока для канала 1 - от 5 до 50 мА, в А
#define REG_RANGE_I_2					71	// Диапазон тока для канала 2 - от 0.5 до 5 мА, в А
#define REG_RANGE_I_3					72	// Диапазон тока для канала 3 - от 50 до 500 мкА, в А
#define REG_RANGE_I_4					73	// Диапазон тока для канала 4 - от 2 до 50 мкА, в А
#define REG_RANGE_I_5					74	// Диапазон тока для канала 5 - от 200 до 2000 нА, в А
#define REG_RANGE_I_6					75	// Диапазон тока для канала 6 - от 20 до 200 нА, в А
#define REG_RANGE_I_7					76	// Диапазон тока для канала 7 - от 5 до 20 нА, в А
//
#define REG_MAX_VOLTAGE_UGETH			77	// Максимальное напряжение при измерении Ugeth, B

#define REG_CURRENT_RGLTR_Kp			78	// Пропорциональный коэффициент регулятора для тока Ugeth
#define REG_CURRENT_RGLTR_Ki			79	// Интегральный коэффициент регулятора для тока при измерении Ugeth
//
#define REG_RGLTR_Kp					80	// Пропорциональный коэффициент регулятора
#define REG_RGLTR_Ki					81	// Интегральный коэффициент регулятора
//
#define REG_RGLTR_FOLLOWING_ERR_LIMIT	82	// Лимит ошибки Following Error
#define REG_RGLTR_FOLLOWING_ERR_THRESH	83	// Порог ошибки Following Error, в частях от 0 до 1
#define REG_VOLTAGE_ERR_THRESH			84	// Допустимая ошибка напряжения для начала измерения, в частях от 0 до 1
#define REG_VOLTAGE_ERR_COUNT_LIMIT		85  // Лимит ошибки счетчика перед выставлением PROBLEM_VOLTAGE_OUT_OF_RANGE
//
#define REG_CURRENT_FLATTOP_DURATION	86 	// Длительность полки поддержания тока, мс
#define REG_CURRENT_ERR_THRESH			87	// Допустимая ошибка тока для начала измерения, в частях от 0 до 1
#define REG_CURRENT_ERR_COUNT_LIMIT		88	// Лимит ошибки счетчика перед выставлением PROBLEM_CURRENT_OUT_OF_RANGE
//
#define REG_REGLTR_TIMER				89	// Время для выхода регулятора на рабочее напряжение, мс
#define REG_RELAY_SW_TIMER_RTH			90	// Время для переключения реле тока при измерении Rth, мс
#define REG_RELAY_SW_TIMER_IGES			91	// Время для переключения реле тока при измерении Iges, мс
#define REG_RELAY_SW_TIMER_UGETH		92	// Время для переключения реле тока при измерении Uge_th, мс
//
#define REG_SLEW_RATE_RTH				93	// Скорость нарастания для измерения Rth, В\мс
#define REG_SLEW_RATE_IGES				94	// Скорость нарастания для измерения Iges, В\мс
#define REG_SLEW_RATE_UGETH				95	// Скорость нарастания для измерения Uge_th, В\мс
#define REG_SLEW_RATE_ST_UPOT			96	// Скорость нарастания для самодиагностики Upot, В\мс
#define REG_SLEW_RATE_ST_TESTLOAD		97	// Скорость нарастания для самодиагностики с тестовой нагрузкой, В\мс
//
#define REG_ST_UPOT_FLATTOP_DURATION	98	// Длительность полки поддержания напряжения при диагностики пот. линий, мс
#define REG_ST_TL_FLATTOP_DURATION		99	// Длительность полки поддержания напряжения при диагностики с нагрузкой, мс
#define REG_RGLTR_ST_ERR_THRESH			100	// Порог ошибки напряжения для диаг. потенциальных линий, в частях от 0 до 1
#define REG_ST_TESTLOAD_RESIS			101	// Сопротивление тестовой нагрузки, Ом
// 102 - 105
//
#define REG_CNT_ACTIVE					106	// Включение сохранения счетчиков
#define REG_SCALING_MUTE				107	// Отключение масштабирования значений в EP
//
// 108 - 127

// Несохраняемы регистры чтения-записи
#define REG_WORK_VOLTAGE_IGES			128	// Номинальное рабочее напряжение для измерения Iges, мВ
#define REG_WORK_CURRENT_UGETH			129	// Номинальный рабочий ток для измерения Ugeth, мА
//
#define REG_SAFETY_MUTE					130	// Отключение контура безопасности
//
#define REG_CNT_NUMBER					131	// Номер счетчика, в который будет записано значение
#define REG_CNT_VALUE					132	// Значение, которое будет записано в счетчик
//
#define REG_DBG							150	// Отладочный регистр
#define REG_DIAG_FORCE_CHANNEL			151	// Принудительное включение определенного диапазона тока
//
// 152 - 191
//

// Регистры только чтение
#define REG_DEV_STATE					192	// Регистр состояния
#define REG_FAULT_REASON				193	// Регистр Fault
#define REG_DISABLE_REASON				194	// Регистр Disable
#define REG_WARNING						195	// Регистр Warning
#define REG_PROBLEM						196	// Регистр Problem
#define REG_OP_RESULT					197	// Регистр результата операции
#define REG_DEV_SUBSTATE				198

#define REG_THERM_RESIS					200	// Полученное сопротивление термистора
#define REG_IGES_RESULT					201	// Полученное значение тока Iges
#define REG_UGE_TH						202 // Полученное пороговое напряжение затвор-эмиттер
//
#define REG_DIAG_CURRENT				230	// Полученный ток
#define REG_DIAG_VOLTAGE				231	// Полученное напряжение
#define REG_DIAG_POT_VOLTAGE			232	// Полученное потенциальное напряжение
#define REG_DEBUG_SCALING_COEF			233	// Рассчитанный коэф масштабирования
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

// Problem
#define PROBLEM_NONE						0
#define PROBLEM_FOLLOWING_ERROR				1
#define PROBLEM_VOLTAGE_OUT_OF_RANGE		2 // Измеренное напряжение вне рабочего диапозона
#define PROBLEM_CURRENT_OUT_OF_RANGE		3 // Измеренный ток вне рабочего диапозона
#define PROBLEM_SAFETY						4 // Сработала система безопасности
#define PROBLEM_NEED_MORE_SAMPLES			5 // Недостаточная длина измерения Iges для получения точного значения
#define PROBLEM_VOLTAGE_LIMIT_NO_CURRENT	6 // Достигнут устновленный лимит напряжения без достижения уровня тока
#define PROBLEM_WRONG_SELECTED_RELAY		7 // Выбрано неверное реле для диагностики

//  Warning
#define WARNING_NONE					0

//  User Errors
#define ERR_NONE						0
#define ERR_CONFIGURATION_LOCKED		1	//  Устройство защищено от записи
#define ERR_OPERATION_BLOCKED			2	//  Операция не может быть выполнена в текущем состоянии устройства
#define ERR_DEVICE_NOT_READY			3	//  Устройство не готово для смены состояния
#define ERR_WRONG_PWD					4	//  Неправильный ключ

// EP
#define EP16_RegulatorIg				1	// Regulator Ig data
#define EP16_RegulatorUg				2	// Regulator Ug data
#define EP16_RegulatorUpot				3	// Regulator Upot data
#define EP16_RegulatorSetpoint			4	// Regulator Setpoint data
#define EP16_RegulatorCorrection		5	// Regulator Correction data
#define EP16_RegulatorError				6	// Regulator Error data
#define EP16_DACRaw						7	// Raw data sent to DAC
#define EP16_ExtInfoData				20	// Diag data drom flash

#endif //  __DEV_OBJ_DIC_H
