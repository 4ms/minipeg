#pragma once
#include <stm32f0xx.h>

#define EXTI_CLOCK_PING_GPIO EXTI_PortSourceGPIOB
#define EXTI_CLOCK_PING_pin EXTI_PinSource5
#define EXTI_CLOCK_PING_line EXTI_Line5
#define EXTI_CLOCK_PING_IRQ EXTI4_15_IRQn

#define EXTI_CLOCK_TRIG_GPIO EXTI_PortSourceGPIOA
#define EXTI_CLOCK_TRIG_pin EXTI_PinSource15
#define EXTI_CLOCK_TRIG_line EXTI_Line15
#define EXTI_CLOCK_TRIG_IRQ EXTI4_15_IRQn

void init_EXTI_inputs(void);

