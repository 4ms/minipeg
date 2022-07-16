#pragma once
#define ALL_GPIO_RCC_ENABLE                                                                                            \
	__HAL_RCC_GPIOC_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOE_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOA_CLK_ENABLE();                                                                                      \
	__HAL_RCC_GPIOD_CLK_ENABLE

#define PING_BUT_Pin GPIO_PIN_1
#define PING_BUT_GPIO_Port GPIOE

#define CYCLE_BUT_Pin GPIO_PIN_14
#define CYCLE_BUT_GPIO_Port GPIOC

#define PING_JACK_Pin GPIO_PIN_3
#define PING_JACK_GPIO_Port GPIOE
// #define PING_JACK_EXTI_IRQn EXTI15_10_IRQn

//CYCLE JACK:
#define AUXTRIG_JACK_Pin GPIO_PIN_4
#define AUXTRIG_JACK_GPIO_Port GPIOE
// #define AUXTRIG_JACK_EXTI_IRQn EXTI0_IRQn

#define TRIG_JACK_Pin GPIO_PIN_5
#define TRIG_JACK_GPIO_Port GPIOD
// #define TRIG_JACK_EXTI_IRQn EXTI15_10_IRQn

//EOF:
#define TRIGOUT_Pin GPIO_PIN_7
#define TRIGOUT_GPIO_Port GPIOD

// Actually is clock out
#define DEBUG_Pin GPIO_PIN_12
#define DEBUG_GPIO_Port GPIOA
