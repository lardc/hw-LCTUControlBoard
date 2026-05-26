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
GPIO_PortPinSettingMacro GPIO_ANLG_UG		= {GPIOA, Pin_0};
GPIO_PortPinSettingMacro GPIO_ANLG_UPOT		= {GPIOA, Pin_4};
GPIO_PortPinSettingMacro GPIO_ANLG_IG		= {GPIOB, Pin_1};

// Определения для входных портов
GPIO_PortPinSettingMacro GPIO_SAFETY		= {GPIOB, Pin_8};

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_LED			= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_LED_EXT		= {GPIOB, Pin_4};
GPIO_PortPinSettingMacro GPIO_SYNC			= {GPIOB, Pin_6};
GPIO_PortPinSettingMacro GPIO_SPI_OE		= {GPIOB, Pin_7};
GPIO_PortPinSettingMacro GPIO_VCC_24		= {GPIOB, Pin_14};
GPIO_PortPinSettingMacro GPIO_VCC_48		= {GPIOB, Pin_15};
GPIO_PortPinSettingMacro GPIO_SPI_SS		= {GPIOA, Pin_15};

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_CAN_RX	= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_ALT_CAN_TX	= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ALT_UART_RX	= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_UART_TX	= {GPIOA, Pin_9};
//
GPIO_PortPinSettingMacro GPIO_ALT_SPI_CLK	= {GPIOB, Pin_3};
GPIO_PortPinSettingMacro GPIO_ALT_SPI_MOSI	= {GPIOB, Pin_5};

#endif // __BOARD_H
