#pragma once
#include "stm32xx.h"

void enable_gpio_rcc(GPIO_TypeDef *port);
void Error_Handler(void);
void system_init(void);
