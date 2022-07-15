#include "pwm.h"
#include "stm32xx.h"

constexpr inline uint32_t kTimPeriod = 4096;

// FIXME: change these values to match f746
PWMOutput pwm[NUM_PWMS] = {
	{GPIOD, 15, GPIO_AF2_TIM4, {.Instance = TIM4}, 4, kTimPeriod}, // PWM_ENVA_R,
	{GPIOA, 15, GPIO_AF1_TIM2, {.Instance = TIM2}, 1, kTimPeriod}, // PWM_ENVA_G,
	{GPIOB, 8, GPIO_AF2_TIM4, {.Instance = TIM4}, 3, kTimPeriod},  // PWM_ENVA_B,
	{GPIOB, 11, GPIO_AF1_TIM2, {.Instance = TIM2}, 4, kTimPeriod}, // PWM_ENVB_R,
	{GPIOE, 11, GPIO_AF1_TIM1, {.Instance = TIM1}, 2, kTimPeriod}, // PWM_ENVB_G,
	{GPIOB, 4, GPIO_AF2_TIM3, {.Instance = TIM3}, 1, kTimPeriod},  // PWM_ENVB_B,
	{GPIOC, 9, GPIO_AF2_TIM3, {.Instance = TIM3}, 4, kTimPeriod},  // PWM_CYCLEBUT_R,
	{GPIOA, 10, GPIO_AF1_TIM1, {.Instance = TIM1}, 3, kTimPeriod}, // PWM_CYCLEBUT_G,
	{GPIOA, 11, GPIO_AF1_TIM1, {.Instance = TIM1}, 4, kTimPeriod}, // PWM_CYCLEBUT_B,
	{GPIOB, 5, GPIO_AF2_TIM3, {.Instance = TIM3}, 2, kTimPeriod},  // PWM_PINGBUT_R,
	{GPIOC, 8, GPIO_AF2_TIM3, {.Instance = TIM3}, 3, kTimPeriod},  // PWM_PINGBUT_G,
	{GPIOD, 13, GPIO_AF2_TIM4, {.Instance = TIM4}, 2, kTimPeriod}, // PWM_PINGBUT_B,
	{GPIOE, 9, GPIO_AF1_TIM1, {.Instance = TIM1}, 1, kTimPeriod},  // PWM_EOF_LED,
};

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
