#pragma once

#define ALL_GPIO_RCC_ENABLE                                                                                            \
	__HAL_RCC_GPIOG_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOC_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOE_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOD_CLK_ENABLE

#define PING_BUT_Pin GPIO_PIN_14
#define PING_BUT_GPIO_Port GPIOG

#define CYCLE_BUT_Pin GPIO_PIN_13
#define CYCLE_BUT_GPIO_Port GPIOC

#define PING_JACK_Pin GPIO_PIN_13
#define PING_JACK_GPIO_Port GPIOG
// #define PING_JACK_EXTI_IRQn EXTI15_10_IRQn

//CYCLE JACK:
#define AUXTRIG_JACK_Pin GPIO_PIN_4
#define AUXTRIG_JACK_GPIO_Port GPIOE
// #define AUXTRIG_JACK_EXTI_IRQn EXTI0_IRQn

#define TRIG_JACK_Pin GPIO_PIN_10
#define TRIG_JACK_GPIO_Port GPIOD
// #define TRIG_JACK_EXTI_IRQn EXTI15_10_IRQn

//EOF:
#define TRIGOUT_Pin GPIO_PIN_8
#define TRIGOUT_GPIO_Port GPIOD

#define DEBUG_Pin GPIO_PIN_0
#define DEBUG_GPIO_Port GPIOC
