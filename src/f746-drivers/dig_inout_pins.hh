#pragma once

#include "drivers/pin.hh"

// TODO: use mdrivlib, not macros:
struct DigIO {
	using GPIO = mdrivlib::GPIO;
	using PinNum = mdrivlib::PinNum;
	using PinMode = mdrivlib::PinMode;
	using PinPolarity = mdrivlib::PinPolarity;

	using PingBut = mdrivlib::FPin<GPIO::G, PinNum::_14, PinMode::Input, PinPolarity::Inverted>;
	using CycleBut = mdrivlib::FPin<GPIO::C, PinNum::_13, PinMode::Input, PinPolarity::Inverted>;

	using PingJack = mdrivlib::FPin<GPIO::G, PinNum::_13, PinMode::Input, PinPolarity::Normal>;
	using CycleJack = mdrivlib::FPin<GPIO::E, PinNum::_4, PinMode::Input, PinPolarity::Normal>;
	using TrigJack = mdrivlib::FPin<GPIO::D, PinNum::_10, PinMode::Input, PinPolarity::Normal>;

	using EOJack = mdrivlib::FPin<GPIO::D, PinNum::_8, PinMode::Output, PinPolarity::Normal>;
	using ClockBusOut = mdrivlib::FPin<GPIO::D, PinNum::_7, PinMode::Output, PinPolarity::Normal>;
};
////////////

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

//CYCLE JACK:
#define AUXTRIG_JACK_Pin GPIO_PIN_4
#define AUXTRIG_JACK_GPIO_Port GPIOE

#define TRIG_JACK_Pin GPIO_PIN_10
#define TRIG_JACK_GPIO_Port GPIOD

//EOF:
#define TRIGOUT_Pin GPIO_PIN_8
#define TRIGOUT_GPIO_Port GPIOD

#define DEBUG_Pin GPIO_PIN_7
#define DEBUG_GPIO_Port GPIOD
