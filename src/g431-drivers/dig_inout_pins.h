#pragma once

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
