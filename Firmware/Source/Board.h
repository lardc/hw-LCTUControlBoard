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
#include "ZwSPI.h"

// Определения для выходных портов
GPIO_PortPinSettingMacro GPIO_LED				= {GPIOA, Pin_6};
GPIO_PortPinSettingMacro GPIO_OPAMP_SYNC		= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_OPAMP_LDAC		= {GPIOB, Pin_14};
GPIO_PortPinSettingMacro GPIO_SYNC				= {GPIOC, Pin_13};
GPIO_PortPinSettingMacro GPIO_PS_EN				= {GPIOB, Pin_1};

// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_UART1_TX		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_UART1_RX		= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_SPI2_CLK		= {GPIOB, Pin_13};
GPIO_PortPinSettingMacro GPIO_ALT_SPI2_DAT		= {GPIOB, Pin_15};

#endif // __BOARD_H
