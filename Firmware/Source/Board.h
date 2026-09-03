#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f30x.h"

#include "ZwRCC.h"
#include "ZwGPIO.h"
#include "ZwNCAN.h"
#include "ZwTIM.h"
#include "ZwDAC.h"
#include "ZwDMA.h"
#include "ZwADC.h"
#include "ZwEXTI.h"
#include "ZwSCI.h"
#include "ZwIWDG.h"
#include "ZwNFLASH.h"

// Определения для аналоговых портов
GPIO_PortPinSettingMacro GPIO_ANLG_U_CAP	= {GPIOA, Pin_3};
GPIO_PortPinSettingMacro GPIO_ANLG_UG		= {GPIOB, Pin_0};
GPIO_PortPinSettingMacro GPIO_ANLG_IG		= {GPIOA, Pin_4};

// Определения для входных портов
GPIO_PortPinSettingMacro GPIO_SAFETY		= {GPIOB, Pin_3};

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_LED_BOARD		= {GPIOB, Pin_4};   // service LED
GPIO_PortPinSettingMacro GPIO_SPI_SYNC		= {GPIOB, Pin_11};
GPIO_PortPinSettingMacro GPIO_SPI_LDAC		= {GPIOB, Pin_1};
GPIO_PortPinSettingMacro GPIO_SW_FAN		= {GPIOB, Pin_14};
GPIO_PortPinSettingMacro GPIO_SW_IND		= {GPIOB, Pin_13};
GPIO_PortPinSettingMacro GPIO_SW_SYNC		= {GPIOB, Pin_12};
GPIO_PortPinSettingMacro GPIO_LCAU_INPUT_CONTACTOR			= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_LCAU_SOFTSTART_DISABLE		= {GPIOA, Pin_15};
GPIO_PortPinSettingMacro GPIO_LCAU_HV_OUT					= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_LCAU_DISCHARGE_DISABLE		= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_SELFTEST1_7MEG				= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_SELFTEST2_700MEG				= {GPIOB, Pin_15};
GPIO_PortPinSettingMacro GPIO_HV_OUT		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_RMES1_NC		= {GPIOB, Pin_5};
GPIO_PortPinSettingMacro GPIO_RMES2			= {GPIOA, Pin_0};
GPIO_PortPinSettingMacro GPIO_RMES3			= {GPIOA, Pin_1};
GPIO_PortPinSettingMacro GPIO_RMES4			= {GPIOA, Pin_2};
GPIO_PortPinSettingMacro GPIO_RMES5			= {GPIOA, Pin_6};

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_CAN_RX	= {GPIOB, Pin_8};
GPIO_PortPinSettingMacro GPIO_ALT_CAN_TX	= {GPIOB, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_UART_RX	= {GPIOB, Pin_7};
GPIO_PortPinSettingMacro GPIO_ALT_UART_TX	= {GPIOB, Pin_6};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_CLK	= {GPIOA, Pin_5};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_MOSI	= {GPIOA, Pin_7};

#endif // __BOARD_H
