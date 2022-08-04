#include "drivers/pin.hh"
#include "pwm.h"
#include "stm32xx.h"

constexpr inline uint32_t kTimPeriod = 4096;

// FIXME: change these values to match f746. already did gpio/pin
PWMOutput pwm[NUM_PWMS] = {
	{GPIOB, GPIO_PIN_11, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_4, kTimPeriod},	  // PWM_ENVA_R,
	{GPIOB, GPIO_PIN_15, GPIO_AF9_TIM12, {.Instance = TIM12}, TIM_CHANNEL_2, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, GPIO_PIN_14, GPIO_AF9_TIM12, {.Instance = TIM12}, TIM_CHANNEL_1, kTimPeriod}, // PWM_ENVA_B,
	{GPIOB, GPIO_PIN_9, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_4, kTimPeriod},	  // PWM_ENVB_R,
	{GPIOB, GPIO_PIN_7, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_2, kTimPeriod},	  // PWM_ENVB_G,
	{GPIOB, GPIO_PIN_8, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_3, kTimPeriod},	  // PWM_ENVB_B,
	{GPIOI, GPIO_PIN_5, GPIO_AF3_TIM8, {.Instance = TIM8}, TIM_CHANNEL_1, kTimPeriod},	  // PWM_CYCLEBUT_R,
	{GPIOB, GPIO_PIN_5, GPIO_AF2_TIM3, {.Instance = TIM3}, TIM_CHANNEL_2, kTimPeriod},	  // PWM_CYCLEBUT_G,
	{GPIOB, GPIO_PIN_6, GPIO_AF2_TIM4, {.Instance = TIM4}, TIM_CHANNEL_1, kTimPeriod},	  // PWM_CYCLEBUT_B,
	{GPIOI, GPIO_PIN_7, GPIO_AF3_TIM8, {.Instance = TIM8}, TIM_CHANNEL_3, kTimPeriod},	  // PWM_PINGBUT_R,
	{GPIOE, GPIO_PIN_6, GPIO_AF3_TIM9, {.Instance = TIM9}, TIM_CHANNEL_2, kTimPeriod},	  // PWM_PINGBUT_G,
	{GPIOE, GPIO_PIN_5, GPIO_AF3_TIM9, {.Instance = TIM9}, TIM_CHANNEL_1, kTimPeriod},	  // PWM_PINGBUT_B,
	{GPIOB, GPIO_PIN_10, GPIO_AF1_TIM2, {.Instance = TIM2}, TIM_CHANNEL_3, kTimPeriod},	  // PWM_EOF_LED,
};

namespace LEDPins
{
using PinNoInit = mdrivlib::PinNoInit;
using GPIO = mdrivlib::GPIO;
using PinNum = mdrivlib::PinNum;
using PinAF = mdrivlib::PinAF;
constexpr inline PinNoInit EnvA_r{GPIO::B, PinNum::_11, PinAF::AltFunc1};
constexpr inline PinNoInit EnvA_g{GPIO::B, PinNum::_15, PinAF::AltFunc9};
constexpr inline PinNoInit EnvA_b{GPIO::B, PinNum::_14, PinAF::AltFunc9};
constexpr inline PinNoInit EnvB_r{GPIO::B, PinNum::_9, PinAF::AltFunc2};
constexpr inline PinNoInit EnvB_g{GPIO::B, PinNum::_7, PinAF::AltFunc2};
constexpr inline PinNoInit EnvB_b{GPIO::B, PinNum::_8, PinAF::AltFunc2};
constexpr inline PinNoInit Cyclebut_r{GPIO::I, PinNum::_5, PinAF::AltFunc3};
constexpr inline PinNoInit Cyclebut_g{GPIO::B, PinNum::_5, PinAF::AltFunc2};
constexpr inline PinNoInit Cyclebut_b{GPIO::B, PinNum::_6, PinAF::AltFunc2};
constexpr inline PinNoInit Pingbut_r{GPIO::I, PinNum::_7, PinAF::AltFunc3};
constexpr inline PinNoInit Pingbut_g{GPIO::E, PinNum::_6, PinAF::AltFunc3};
constexpr inline PinNoInit Pingbut_b{GPIO::E, PinNum::_5, PinAF::AltFunc3};
constexpr inline PinNoInit Eof_led{GPIO::B, PinNum::_10, PinAF::AltFunc1};
} // namespace LEDPins

#define LED_EOF_Pin GPIO_PIN_10
#define LED_EOF_GPIO_Port GPIOB

#define ENVA_R_Pin GPIO_PIN_11
#define ENVA_R_GPIO_Port GPIOB
#define ENVA_G_Pin GPIO_PIN_15
#define ENVA_G_GPIO_Port GPIOB
#define ENVA_B_Pin GPIO_PIN_14
#define ENVA_B_GPIO_Port GPIOB

#define ENVB_R_Pin GPIO_PIN_9
#define ENVB_R_GPIO_Port GPIOB
#define ENVB_G_Pin GPIO_PIN_7
#define ENVB_G_GPIO_Port GPIOB
#define ENVB_B_Pin GPIO_PIN_8
#define ENVB_B_GPIO_Port GPIOB

#define PINGBUT_R_Pin GPIO_PIN_7
#define PINGBUT_R_GPIO_Port GPIOI
#define PINGBUT_G_Pin GPIO_PIN_6
#define PINGBUT_G_GPIO_Port GPIOE
#define PINGBUT_B_Pin GPIO_PIN_5
#define PINGBUT_B_GPIO_Port GPIOE

#define CYCLEBUT_R_Pin GPIO_PIN_5
#define CYCLEBUT_R_GPIO_Port GPIOI
#define CYCLEBUT_G_Pin GPIO_PIN_5
#define CYCLEBUT_G_GPIO_Port GPIOB
#define CYCLEBUT_B_Pin GPIO_PIN_6
#define CYCLEBUT_B_GPIO_Port GPIOB

static inline void populate_pwm_pins(struct PWMOutput p[]) {
	p[PWM_ENVA_R].gpio = ENVA_R_GPIO_Port;
	p[PWM_ENVA_R].pinnum = ENVA_R_Pin;
	p[PWM_ENVA_R].af = GPIO_AF1_TIM2;
	p[PWM_ENVA_R].tim.Instance = TIM2;
	p[PWM_ENVA_R].timchan = TIM_CHANNEL_4;
	p[PWM_ENVA_R].period = kTimPeriod;

	p[PWM_ENVA_G].gpio = ENVA_G_GPIO_Port;
	p[PWM_ENVA_G].pinnum = ENVA_G_Pin;
	p[PWM_ENVA_G].af = GPIO_AF9_TIM12;
	p[PWM_ENVA_G].tim.Instance = TIM12;
	p[PWM_ENVA_G].timchan = TIM_CHANNEL_2;
	p[PWM_ENVA_G].period = kTimPeriod;

	p[PWM_ENVA_B].gpio = ENVA_B_GPIO_Port;
	p[PWM_ENVA_B].pinnum = ENVA_B_Pin;
	p[PWM_ENVA_B].af = GPIO_AF9_TIM12;
	p[PWM_ENVA_B].tim.Instance = TIM12;
	p[PWM_ENVA_B].timchan = TIM_CHANNEL_1;
	p[PWM_ENVA_B].period = kTimPeriod;

	p[PWM_ENVB_R].gpio = ENVB_R_GPIO_Port;
	p[PWM_ENVB_R].pinnum = ENVB_R_Pin;
	p[PWM_ENVB_R].af = GPIO_AF2_TIM4;
	p[PWM_ENVB_R].tim.Instance = TIM4;
	p[PWM_ENVB_R].timchan = TIM_CHANNEL_4;
	p[PWM_ENVB_R].period = kTimPeriod;

	p[PWM_ENVB_G].gpio = ENVB_G_GPIO_Port;
	p[PWM_ENVB_G].pinnum = ENVB_G_Pin;
	p[PWM_ENVB_G].af = GPIO_AF2_TIM4;
	p[PWM_ENVB_G].tim.Instance = TIM4;
	p[PWM_ENVB_G].timchan = TIM_CHANNEL_2;
	p[PWM_ENVB_G].period = kTimPeriod;

	p[PWM_ENVB_B].gpio = ENVB_B_GPIO_Port;
	p[PWM_ENVB_B].pinnum = ENVB_B_Pin;
	p[PWM_ENVB_B].af = GPIO_AF2_TIM4;
	p[PWM_ENVB_B].tim.Instance = TIM4;
	p[PWM_ENVB_B].timchan = TIM_CHANNEL_3;
	p[PWM_ENVB_B].period = kTimPeriod;

	p[PWM_PINGBUT_R].gpio = PINGBUT_R_GPIO_Port;
	p[PWM_PINGBUT_R].pinnum = PINGBUT_R_Pin;
	p[PWM_PINGBUT_R].af = GPIO_AF3_TIM8;
	p[PWM_PINGBUT_R].tim.Instance = TIM8;
	p[PWM_PINGBUT_R].timchan = TIM_CHANNEL_3;
	p[PWM_PINGBUT_R].period = kTimPeriod;

	p[PWM_PINGBUT_G].gpio = PINGBUT_G_GPIO_Port;
	p[PWM_PINGBUT_G].pinnum = PINGBUT_G_Pin;
	p[PWM_PINGBUT_G].af = GPIO_AF3_TIM9;
	p[PWM_PINGBUT_G].tim.Instance = TIM9;
	p[PWM_PINGBUT_G].timchan = TIM_CHANNEL_2;
	p[PWM_PINGBUT_G].period = kTimPeriod;

	p[PWM_PINGBUT_B].gpio = PINGBUT_B_GPIO_Port;
	p[PWM_PINGBUT_B].pinnum = PINGBUT_B_Pin;
	p[PWM_PINGBUT_B].af = GPIO_AF3_TIM9;
	p[PWM_PINGBUT_B].tim.Instance = TIM9;
	p[PWM_PINGBUT_B].timchan = TIM_CHANNEL_1;
	p[PWM_PINGBUT_B].period = kTimPeriod;

	p[PWM_CYCLEBUT_R].gpio = CYCLEBUT_R_GPIO_Port;
	p[PWM_CYCLEBUT_R].pinnum = CYCLEBUT_R_Pin;
	p[PWM_CYCLEBUT_R].af = GPIO_AF3_TIM8;
	p[PWM_CYCLEBUT_R].tim.Instance = TIM8;
	p[PWM_CYCLEBUT_R].timchan = TIM_CHANNEL_1;
	p[PWM_CYCLEBUT_R].period = kTimPeriod;

	p[PWM_CYCLEBUT_G].gpio = CYCLEBUT_G_GPIO_Port;
	p[PWM_CYCLEBUT_G].pinnum = CYCLEBUT_G_Pin;
	p[PWM_CYCLEBUT_G].af = GPIO_AF2_TIM3;
	p[PWM_CYCLEBUT_G].tim.Instance = TIM3;
	p[PWM_CYCLEBUT_G].timchan = TIM_CHANNEL_2;
	p[PWM_CYCLEBUT_G].period = kTimPeriod;

	p[PWM_CYCLEBUT_B].gpio = CYCLEBUT_B_GPIO_Port;
	p[PWM_CYCLEBUT_B].pinnum = CYCLEBUT_B_Pin;
	p[PWM_CYCLEBUT_B].af = GPIO_AF2_TIM4;
	p[PWM_CYCLEBUT_B].tim.Instance = TIM4;
	p[PWM_CYCLEBUT_B].timchan = TIM_CHANNEL_1;
	p[PWM_CYCLEBUT_B].period = kTimPeriod;

	p[PWM_EOF_LED].gpio = LED_EOF_GPIO_Port;
	p[PWM_EOF_LED].pinnum = LED_EOF_Pin;
	p[PWM_EOF_LED].af = GPIO_AF1_TIM2;
	p[PWM_EOF_LED].tim.Instance = TIM2;
	p[PWM_EOF_LED].timchan = TIM_CHANNEL_3;
	p[PWM_EOF_LED].period = kTimPeriod;
}
