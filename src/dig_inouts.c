/*
 * dig_inouts.c
 */

#include "globals.h"


void init_dig_inouts(void){
	GPIO_InitTypeDef gpio = {0};

	ALL_GPIO_RCC_ENABLE();

	//Configure outputs
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	gpio.Pull = GPIO_NOPULL;

	gpio.Pin = TRIGOUT_pin;		
	HAL_GPIO_Init(TRIGOUT_GPIO, &gpio);

	gpio.Pin = LED_PING_BUT_G_pin;
	HAL_GPIO_Init(LED_PING_BUT_G_GPIO, &gpio);
	gpio.Pin = LED_CYCLE_BUT_B_pin;		
	HAL_GPIO_Init(LED_CYCLE_BUT_B_GPIO, &gpio);

	//Configure inputs with pullup
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pull = GPIO_PULLUP;

	gpio.Pin = PINGBUT_pin;		
	HAL_GPIO_Init(PINGBUT_GPIO, &gpio);
	gpio.Pin = CYCLEBUT_pin;	
	HAL_GPIO_Init(CYCLEBUT_GPIO, &gpio);

	//Configure inputs with pulldown
	gpio.Pull = GPIO_PULLDOWN;

	gpio.Pin = TRIGJACK_pin;	
	HAL_GPIO_Init(TRIGJACK_GPIO, &gpio);
	gpio.Pin = PINGJACK_pin;	
	HAL_GPIO_Init(PINGJACK_GPIO, &gpio);
	gpio.Pin = CYCLEJACK_pin;	
	HAL_GPIO_Init(CYCLEJACK_GPIO, &gpio);
}
