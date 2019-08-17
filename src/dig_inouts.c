/*
 * dig_inouts.c
 */

#include "globals.h"


void init_dig_inouts(void){
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	RCC_AHBPeriphClockCmd(ALL_GPIO_RCC, ENABLE);

	//Configure outputs
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	gpio.GPIO_Pin = TRIGOUT_pin;	GPIO_Init(TRIGOUT_GPIO, &gpio);
	gpio.GPIO_Pin = LEDTRIGOUT_pin;	GPIO_Init(LEDTRIGOUT_GPIO, &gpio);


	//Configure inputs
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;

	gpio.GPIO_PuPd = GPIO_PuPd_UP;
	gpio.GPIO_Pin = PINGBUT_pin;	GPIO_Init(PINGBUT_GPIO, &gpio);
	gpio.GPIO_Pin = CYCLEBUT_pin;	GPIO_Init(CYCLEBUT_GPIO, &gpio);

	gpio.GPIO_PuPd = GPIO_PuPd_DOWN;
	gpio.GPIO_Pin = TRIGJACK_pin;	GPIO_Init(TRIGJACK_GPIO, &gpio);
	gpio.GPIO_Pin = PINGJACK_pin;	GPIO_Init(PINGJACK_GPIO, &gpio);
	gpio.GPIO_Pin = CYCLEJACK_pin;	GPIO_Init(CYCLEJACK_GPIO, &gpio);

}
