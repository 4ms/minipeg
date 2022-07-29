#pragma once
#include "drivers/pin.hh"
#include "pwm.h"

constexpr inline uint32_t kTimPeriod = 4096;

PWMOutput pwm[NUM_PWMS] = {
	{GPIOD, GPIO_PIN_15, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_4, kTimPeriod}, // PWM_ENVA_R,
	{GPIOA, GPIO_PIN_15, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_1, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, GPIO_PIN_8, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_3, kTimPeriod},	// PWM_ENVA_B,
	{GPIOB, GPIO_PIN_11, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_4, kTimPeriod}, // PWM_ENVB_R,
	{GPIOE, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_2, kTimPeriod}, // PWM_ENVB_G,
	{GPIOB, GPIO_PIN_4, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_1, kTimPeriod},	// PWM_ENVB_B,
	{GPIOC, GPIO_PIN_9, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_4, kTimPeriod},	// PWM_CYCLEBUT_R,
	{GPIOA, GPIO_PIN_10, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_3, kTimPeriod}, // PWM_CYCLEBUT_G,
	{GPIOA, GPIO_PIN_11, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_4, kTimPeriod}, // PWM_CYCLEBUT_B,
	{GPIOB, GPIO_PIN_5, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_2, kTimPeriod},	// PWM_PINGBUT_R,
	{GPIOC, GPIO_PIN_8, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_3, kTimPeriod},	// PWM_PINGBUT_G,
	{GPIOD, GPIO_PIN_13, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_2, kTimPeriod}, // PWM_PINGBUT_B,
	{GPIOE, GPIO_PIN_9, GPIO_AF1_TIM1, {.Instance = TIM1}, TIM_CHANNEL_1, kTimPeriod},	// PWM_EOF_LED,
};

namespace LEDPins
{
using PinNoInit = mdrivlib::PinNoInit;
using GPIO = mdrivlib::GPIO;
using PinNum = mdrivlib::PinNum;
using PinAF = mdrivlib::PinAF;

constexpr inline PinNoInit EnvA_r{GPIO::D, PinNum::_15, PinAF::AltFunc2};
constexpr inline PinNoInit EnvA_g{GPIO::A, PinNum::_15, PinAF::AltFunc1};
constexpr inline PinNoInit EnvA_b{GPIO::B, PinNum::_8, PinAF::AltFunc2};
constexpr inline PinNoInit EnvB_r{GPIO::B, PinNum::_11, PinAF::AltFunc1};
constexpr inline PinNoInit EnvB_g{GPIO::E, PinNum::_11, PinAF::AltFunc1};
constexpr inline PinNoInit EnvB_b{GPIO::B, PinNum::_4, PinAF::AltFunc2};
constexpr inline PinNoInit Cyclebut_r{GPIO::C, PinNum::_9, PinAF::AltFunc2};
constexpr inline PinNoInit Cyclebut_g{GPIO::A, PinNum::_10, PinAF::AltFunc1};
constexpr inline PinNoInit Cyclebut_b{GPIO::A, PinNum::_11, PinAF::AltFunc1};
constexpr inline PinNoInit Pingbut_r{GPIO::B, PinNum::_5, PinAF::AltFunc2};
constexpr inline PinNoInit Pingbut_g{GPIO::C, PinNum::_8, PinAF::AltFunc2};
constexpr inline PinNoInit Pingbut_b{GPIO::D, PinNum::_13, PinAF::AltFunc2};
constexpr inline PinNoInit Eof_led{GPIO::E, PinNum::_9, PinAF::AltFunc1};
} // namespace LEDPins
