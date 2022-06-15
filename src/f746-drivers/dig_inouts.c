/*
 * dig_inouts.c
 */
#include "dig_inouts.h"
#include <stm32g4xx.h>

void init_dig_inouts(void) {
	GPIO_InitTypeDef gpio = {0};

	ALL_GPIO_RCC_ENABLE();

	//Configure outputs
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Pull = GPIO_NOPULL;
	gpio.Pin = TRIGOUT_Pin;
	HAL_GPIO_Init(TRIGOUT_GPIO_Port, &gpio);
	gpio.Pin = DEBUG_Pin;
	HAL_GPIO_Init(DEBUG_GPIO_Port, &gpio);

	//Configure inputs with pullup
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pull = GPIO_PULLUP;

	gpio.Pin = PING_BUT_Pin;
	HAL_GPIO_Init(PING_BUT_GPIO_Port, &gpio);
	gpio.Pin = CYCLE_BUT_Pin;
	HAL_GPIO_Init(CYCLE_BUT_GPIO_Port, &gpio);

	//Configure inputs with pulldown
	gpio.Pull = GPIO_PULLDOWN;

	gpio.Pin = TRIG_JACK_Pin;
	HAL_GPIO_Init(TRIG_JACK_GPIO_Port, &gpio);
	gpio.Pin = PING_JACK_Pin;
	HAL_GPIO_Init(PING_JACK_GPIO_Port, &gpio);
	gpio.Pin = AUXTRIG_JACK_Pin;
	HAL_GPIO_Init(AUXTRIG_JACK_GPIO_Port, &gpio);
}
