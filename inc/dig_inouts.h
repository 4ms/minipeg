/*
 * dig_inouts.h
 */

#pragma once

#include <stm32f0xx.h>


#define PIN_IS_HIGH(x,y) (x->IDR & y)
#define PIN_IS_LOW(x,y) (!(x->IDR & y))

#define PIN_HIGH(x,y) x->BSRR = y
#define PIN_LOW(x,y) x->BRR = y

#define ALL_GPIO_RCC (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOF)

#define TRIGJACK_pin GPIO_Pin_15
#define TRIGJACK_GPIO GPIOA
#define TRIGJACK_READ PIN_IS_HIGH(TRIGJACK_GPIO, TRIGJACK_pin)

#define CYCLEJACK_pin GPIO_Pin_1
#define CYCLEJACK_GPIO GPIOF
#define CYCLEJACK_READ PIN_IS_HIGH(CYCLEJACK_GPIO, CYCLEJACK_pin)


#define PINGJACK_pin GPIO_Pin_5
#define PINGJACK_GPIO GPIOB
#define PINGJACK PIN_IS_HIGH(PINGJACK_GPIO, PINGJACK_pin)

#define PINGBUT_pin GPIO_Pin_6
#define PINGBUT_GPIO GPIOB
#define PINGBUT PIN_IS_LOW(PINGBUT_GPIO, PINGBUT_pin)


#define CYCLEBUT_pin GPIO_Pin_3
#define CYCLEBUT_GPIO GPIOB
#define CYCLEBUT PIN_IS_LOW(CYCLEBUT_GPIO, CYCLEBUT_pin)

#define TRIGOUT_pin GPIO_Pin_12
#define TRIGOUT_GPIO GPIOA

#ifdef TRIGDEBUGMODE
	#define TRIGOUT_ON 
	#define TRIGOUT_OFF 
	#define DEBUGON PIN_HIGH(TRIGOUT_GPIO, TRIGOUT_pin)
	#define DEBUGOFF PIN_LOW(TRIGOUT_GPIO, TRIGOUT_pin)
#else
	#define TRIGOUT_ON PIN_HIGH(TRIGOUT_GPIO, TRIGOUT_pin)
	#define TRIGOUT_OFF PIN_LOW(TRIGOUT_GPIO, TRIGOUT_pin)
	#define DEBUGON
	#define DEBUGOFF
#endif


void init_dig_inouts(void);
