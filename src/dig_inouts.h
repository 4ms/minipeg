/*
 * dig_inouts.h
 */

#pragma once

#include <stddef.h>

#define PIN_IS_HIGH(x, y) ((x)->IDR & (y))
#define PIN_IS_LOW(x, y) (!((x)->IDR & (y)))

#define PIN_HIGH(x, y) (x)->BSRR = (y)
#define PIN_LOW(x, y) (x)->BRR = (y)

#define ALL_GPIO_RCC_ENABLE                                                                                            \
	__HAL_RCC_GPIOA_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOB_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOC_CLK_ENABLE

#define PING_BUT_Pin GPIO_PIN_14
#define PING_BUT_GPIO_Port GPIOC

#define CYCLE_BUT_Pin GPIO_PIN_13
#define CYCLE_BUT_GPIO_Port GPIOC

#define PING_JACK_Pin GPIO_PIN_15
#define PING_JACK_GPIO_Port GPIOC
#define PING_JACK_EXTI_IRQn EXTI15_10_IRQn

#define AUXTRIG_JACK_Pin GPIO_PIN_0
#define AUXTRIG_JACK_GPIO_Port GPIOA
#define AUXTRIG_JACK_EXTI_IRQn EXTI0_IRQn

#define TRIG_JACK_Pin GPIO_PIN_13
#define TRIG_JACK_GPIO_Port GPIOB
#define TRIG_JACK_EXTI_IRQn EXTI15_10_IRQn

#define TRIGOUT_Pin GPIO_PIN_12
#define TRIGOUT_GPIO_Port GPIOA

#define DEBUG_Pin GPIO_PIN_1
#define DEBUG_GPIO_Port GPIOB

#define PINGBUT PIN_IS_LOW(PING_BUT_GPIO_Port, PING_BUT_Pin)
#define CYCLEBUT PIN_IS_LOW(CYCLE_BUT_GPIO_Port, CYCLE_BUT_Pin)
#define PING_JACK_READ PIN_IS_HIGH(PING_JACK_GPIO_Port, PING_JACK_Pin)
#define AUXTRIG_JACK_READ PIN_IS_HIGH(AUXTRIG_JACK_GPIO_Port, AUXTRIG_JACK_Pin)
#define TRIG_JACK_READ PIN_IS_HIGH(TRIG_JACK_GPIO_Port, TRIG_JACK_Pin)

#ifdef TRIGDEBUGMODE
#define TRIGOUT_ON
#define TRIGOUT_OFF
#define DEBUGON LL_GPIO_SetOutputPin(TRIGOUT_GPIO_Port, TRIGOUT_Pin)
#define DEBUGOFF LL_GPIO_ResetOutputPin(TRIGOUT_GPIO_Port, TRIGOUT_Pin)
#else
#define TRIGOUT_ON LL_GPIO_SetOutputPin(TRIGOUT_GPIO_Port, TRIGOUT_Pin)
#define TRIGOUT_OFF LL_GPIO_ResetOutputPin(TRIGOUT_GPIO_Port, TRIGOUT_Pin)
#define DEBUGON LL_GPIO_SetOutputPin(DEBUG_GPIO_Port, DEBUG_Pin)
#define DEBUGOFF LL_GPIO_ResetOutputPin(DEBUG_GPIO_Port, DEBUG_Pin)
#endif

void init_dig_inouts(void);
