#include "drivers/clocks.hh"
#include "drivers/tim_pwm.hh"
#include "globals.h"
#include "stm32xx.h"

struct PWMOutput {
	GPIO_TypeDef *gpio;
	uint32_t pinnum;
	uint8_t af;
	TIM_HandleTypeDef tim;
	uint8_t timchan;
	uint32_t period;
};

struct PWMOutput pwm[NUM_PWMS];

//Private:
void init_pwm_out_pin(struct PWMOutput *p);
void populate_pwm_pins(struct PWMOutput *p);

void update_pwm(uint32_t pwmval, enum PwmOutputs pwmnum) {
	if (pwmnum >= NUM_PWMS)
		return;

	uint8_t channel;

	channel = pwm[pwmnum].timchan;

	if (channel == TIM_CHANNEL_1)
		pwm[pwmnum].tim.Instance->CCR1 = pwmval;

	else if (channel == TIM_CHANNEL_2)
		pwm[pwmnum].tim.Instance->CCR2 = pwmval;

	else if (channel == TIM_CHANNEL_3)
		pwm[pwmnum].tim.Instance->CCR3 = pwmval;

	else if (channel == TIM_CHANNEL_4)
		pwm[pwmnum].tim.Instance->CCR4 = pwmval;

	else if (channel == TIM_CHANNEL_5)
		pwm[pwmnum].tim.Instance->CCR5 = pwmval;

	else if (channel == TIM_CHANNEL_6)
		pwm[pwmnum].tim.Instance->CCR6 = pwmval;
}

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

const uint32_t kTimPeriod = 4095;
void populate_pwm_pins(struct PWMOutput p[]) {
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

void init_pwm_out_pin(struct PWMOutput *p) {
	GPIO_InitTypeDef gpio;

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Pull = GPIO_NOPULL;
	gpio.Pin = p->pinnum;
	gpio.Alternate = p->af;
	mdrivlib::RCC_Enable::GPIO::enable(p->gpio);
	HAL_GPIO_Init(p->gpio, &gpio);
}

void init_pwm_tim(struct PWMOutput *p) {
	mdrivlib::Clocks::TIM::enable(p->tim.Instance);
	TIM_OC_InitTypeDef tim_oc;

	p->tim.Init.Prescaler = 0;
	p->tim.Init.Period = p->period;
	p->tim.Init.ClockDivision = 0;
	p->tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	p->tim.Init.RepetitionCounter = 0;
	p->tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_PWM_Init(&p->tim);

	tim_oc.OCMode = TIM_OCMODE_PWM1;
	tim_oc.OCFastMode = TIM_OCFAST_DISABLE;
	tim_oc.OCPolarity = TIM_OCPOLARITY_LOW;
	tim_oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	tim_oc.OCIdleState = TIM_OCIDLESTATE_SET;
	tim_oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	tim_oc.Pulse = 0;

	HAL_TIM_PWM_ConfigChannel(&p->tim, &tim_oc, p->timchan);
}

void start_timers(struct PWMOutput *p) {
	HAL_TIM_PWM_Start(&p->tim, p->timchan);
}

void init_pwm(void) {
	populate_pwm_pins(pwm);

	for (uint8_t i = 0; i < NUM_PWMS; i++) {
		init_pwm_out_pin(&pwm[i]);
		init_pwm_tim(&pwm[i]);
	}

	for (uint8_t i = 0; i < NUM_PWMS; i++)
		start_timers(&pwm[i]);
}
