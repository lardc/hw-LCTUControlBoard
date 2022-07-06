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
GPIO_PortPinSettingMacro GPIO_LED				= {GPIOB, Pin_12};
GPIO_PortPinSettingMacro GPIO_OPAMP_SYNC		= {GPIOB, Pin_6};
GPIO_PortPinSettingMacro GPIO_OPAMP_LDAC		= {GPIOB, Pin_4};
GPIO_PortPinSettingMacro GPIO_OSC_SYNC			= {GPIOB, Pin_15};
GPIO_PortPinSettingMacro GPIO_PS_CTRL			= {GPIOB, Pin_0};
GPIO_PortPinSettingMacro GPIO_U_RANGE			= {GPIOA, Pin_2};
GPIO_PortPinSettingMacro GPIO_PAU_SHUNT			= {GPIOA, Pin_3};
GPIO_PortPinSettingMacro GPIO_STST				= {GPIOA, Pin_4};
GPIO_PortPinSettingMacro GPIO_COMM				= {GPIOA, Pin_5};
GPIO_PortPinSettingMacro GPIO_FAN_CTRL			= {GPIOA, Pin_6};
GPIO_PortPinSettingMacro GPIO_IND_CTRL			= {GPIOA, Pin_7};
GPIO_PortPinSettingMacro GPIO_PAU_SYNC			= {GPIOB, Pin_13};

// Определения для входных портов
GPIO_PortPinSettingMacro GPIO_OSC_SYNC_CHK		= {GPIOA, Pin_8};
GPIO_PortPinSettingMacro GPIO_PAU_SYNC_CHK		= {GPIOB, Pin_14};


// Определения для портов альтернативных функций
GPIO_PortPinSettingMacro GPIO_ALT_UART1_TX		= {GPIOA, Pin_9};
GPIO_PortPinSettingMacro GPIO_ALT_UART1_RX		= {GPIOA, Pin_10};
GPIO_PortPinSettingMacro GPIO_ALT_CAN1_TX		= {GPIOA, Pin_12};
GPIO_PortPinSettingMacro GPIO_ALT_CAN1_RX		= {GPIOA, Pin_11};
GPIO_PortPinSettingMacro GPIO_ALT_SPI3_CLK		= {GPIOB, Pin_3};
GPIO_PortPinSettingMacro GPIO_ALT_SPI3_DAT		= {GPIOB, Pin_5};

#endif // __BOARD_H
