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
GPIO_PortPinSettingMacro GPIO_LED			= {GPIOA, Pin_8};   // Rst1 / service LED
GPIO_PortPinSettingMacro GPIO_LED_EXT		= {GPIOB, Pin_4};   // reserved
GPIO_PortPinSettingMacro GPIO_SYNC			= {GPIOB, Pin_11};
GPIO_PortPinSettingMacro GPIO_SPI_SS		= {GPIOB, Pin_1};
GPIO_PortPinSettingMacro GPIO_SW_FAN		= {GPIOB, Pin_14};
GPIO_PortPinSettingMacro GPIO_SW_IND		= {GPIOB, Pin_13};
GPIO_PortPinSettingMacro GPIO_SW_SYNC		= {GPIOB, Pin_12};
GPIO_PortPinSettingMacro GPIO_RCON			= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_RSS			= {GPIOA, Pin_15};
GPIO_PortPinSettingMacro GPIO_ROUT_LCAU		= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_RDIS			= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ROUT_LCTU		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_RST1			= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_RST2			= {GPIOB, Pin_15};
GPIO_PortPinSettingMacro GPIO_RMES1			= {GPIOB, Pin_5};
GPIO_PortPinSettingMacro GPIO_RMES2			= {GPIOA, Pin_0};
GPIO_PortPinSettingMacro GPIO_RMES3			= {GPIOA, Pin_1};
GPIO_PortPinSettingMacro GPIO_RMES4			= {GPIOA, Pin_2};
GPIO_PortPinSettingMacro GPIO_RMES5			= {GPIOA, Pin_5};

// Переходные алиасы до полной миграции логики
#define GPIO_VCC_24 GPIO_SW_IND
#define GPIO_VCC_48 GPIO_SW_FAN

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_CAN_RX	= {GPIOB, Pin_8};
GPIO_PortPinSettingMacro GPIO_ALT_CAN_TX	= {GPIOB, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_UART_RX	= {GPIOB, Pin_7};
GPIO_PortPinSettingMacro GPIO_ALT_UART_TX	= {GPIOB, Pin_6};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_CLK	= {GPIOB, Pin_2};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_MOSI	= {GPIOB, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_NSS	= {GPIOB, Pin_1};

#endif // __BOARD_H
