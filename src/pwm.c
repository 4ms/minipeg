#include "globals.h"

struct PWMOutput {
	GPIO_TypeDef *gpio;
	uint32_t pinnum;
	uint8_t af;
	TIM_HandleTypeDef *tim;
	uint8_t timchan;
	uint32_t period;
};

struct PWMOutput pwm[NUM_PWMS];

//Private:
void init_pwm_out_pin(struct PWMOutput *p);
void populate_pwm_pins(struct PWMOutput *p);

void update_pwm(uint32_t pwmval, enum PwmOutputs pwmnum) {
	uint8_t channel;

	channel = pwm[pwmnum].timchan;

	if (channel==TIM_CHANNEL_1)
		pwm[pwmnum].tim->Instance->CCR1 = pwmval;

	else if (channel==TIM_CHANNEL_2)
		pwm[pwmnum].tim->Instance->CCR2 = pwmval;

	else if (channel==TIM_CHANNEL_3)
		pwm[pwmnum].tim->Instance->CCR3 = pwmval;

	else if (channel==TIM_CHANNEL_4)
		pwm[pwmnum].tim->Instance->CCR4 = pwmval;

	else if (channel==TIM_CHANNEL_5)
		pwm[pwmnum].tim->Instance->CCR5 = pwmval;

	else if (channel==TIM_CHANNEL_6)
		pwm[pwmnum].tim->Instance->CCR6 = pwmval;
}

void populate_pwm_pins(struct PWMOutput p[]) {
	p[PWM_5VENV].gpio = GPIOA;
	p[PWM_5VENV].pinnum = GPIO_PIN_7;
	p[PWM_5VENV].af = GPIO_AF4_TIM14;
	p[PWM_5VENV].tim->Instance = TIM15;
	p[PWM_5VENV].timchan = TIM_CHANNEL_1;
	p[PWM_5VENV].period = 1024;

	p[PWM_5VENVLED_G].gpio = GPIOA;
	p[PWM_5VENVLED_G].pinnum = GPIO_PIN_2;
	p[PWM_5VENVLED_G].af = GPIO_AF5_TIM15;
	p[PWM_5VENVLED_G].tim->Instance = TIM15;
	p[PWM_5VENVLED_G].timchan = TIM_CHANNEL_1;
	p[PWM_5VENVLED_G].period = 1024;

	p[PWM_ENV].gpio = GPIOA;
	p[PWM_ENV].pinnum = GPIO_PIN_8;
	p[PWM_ENV].af = GPIO_AF2_TIM1;
	p[PWM_ENV].tim->Instance = TIM1;
	p[PWM_ENV].timchan = TIM_CHANNEL_1;
	p[PWM_ENV].period = 1024;

	p[PWM_ENVLED_G].gpio = GPIOA;
	p[PWM_ENVLED_G].pinnum = GPIO_PIN_3;
	p[PWM_ENVLED_G].af = GPIO_AF5_TIM15;
	p[PWM_ENVLED_G].tim->Instance = TIM15;
	p[PWM_ENVLED_G].timchan = TIM_CHANNEL_2;
	p[PWM_ENVLED_G].period = 1024;

	p[PWM_ENVLED_B].gpio = GPIOA;
	p[PWM_ENVLED_B].pinnum = GPIO_PIN_9;
	p[PWM_ENVLED_B].af = GPIO_AF2_TIM1;
	p[PWM_ENVLED_B].tim->Instance = TIM1;
	p[PWM_ENVLED_B].timchan = TIM_CHANNEL_2;
	p[PWM_ENVLED_B].period = 1024;

	p[PWM_CYCLEBUT_G].gpio = GPIOA;
	p[PWM_CYCLEBUT_G].pinnum = GPIO_PIN_10;
	p[PWM_CYCLEBUT_G].af = GPIO_AF2_TIM1;
	p[PWM_CYCLEBUT_G].tim->Instance = TIM1;
	p[PWM_CYCLEBUT_G].timchan = TIM_CHANNEL_3;
	p[PWM_CYCLEBUT_G].period = 1024;

	p[PWM_CYCLEBUT_R].gpio = GPIOA;
	p[PWM_CYCLEBUT_R].pinnum = GPIO_PIN_11;
	p[PWM_CYCLEBUT_R].af = GPIO_AF2_TIM1;
	p[PWM_CYCLEBUT_R].tim->Instance = TIM1;
	p[PWM_CYCLEBUT_R].timchan = TIM_CHANNEL_4;
	p[PWM_CYCLEBUT_R].period = 1024;

	p[PWM_ENVLED_R].gpio = GPIOC;
	p[PWM_ENVLED_R].pinnum = GPIO_PIN_6;
	p[PWM_ENVLED_R].af = GPIO_AF1_TIM3;
	p[PWM_ENVLED_R].tim->Instance = TIM3;
	p[PWM_ENVLED_R].timchan = TIM_CHANNEL_1;
	p[PWM_ENVLED_R].period = 1024;

	p[PWM_5VENVLED_B].gpio = GPIOB;
	p[PWM_5VENVLED_B].pinnum = GPIO_PIN_0;
	p[PWM_5VENVLED_B].af = GPIO_AF1_TIM3;
	p[PWM_5VENVLED_B].tim->Instance = TIM3;
	p[PWM_5VENVLED_B].timchan = TIM_CHANNEL_3;
	p[PWM_5VENVLED_B].period = 1024;

	p[PWM_5VENVLED_R].gpio = GPIOB;
	p[PWM_5VENVLED_R].pinnum = GPIO_PIN_1;
	p[PWM_5VENVLED_R].af = GPIO_AF1_TIM3;
	p[PWM_5VENVLED_R].tim->Instance = TIM3;
	p[PWM_5VENVLED_R].timchan = TIM_CHANNEL_4;
	p[PWM_5VENVLED_R].period = 1024;

	p[PWM_EOF_LED].gpio = GPIOB;
	p[PWM_EOF_LED].pinnum = GPIO_PIN_5;
	p[PWM_EOF_LED].af = GPIO_AF1_TIM3;
	p[PWM_EOF_LED].tim->Instance = TIM3;
	p[PWM_EOF_LED].timchan = TIM_CHANNEL_2;
	p[PWM_EOF_LED].period = 1024;

	p[PWM_PINGBUT_R].gpio = GPIOB;
	p[PWM_PINGBUT_R].pinnum = GPIO_PIN_9;
	p[PWM_PINGBUT_R].af = GPIO_AF2_TIM17;
	p[PWM_PINGBUT_R].tim->Instance = TIM17;
	p[PWM_PINGBUT_R].timchan = TIM_CHANNEL_1;
	p[PWM_PINGBUT_R].period = 1024;

	p[PWM_PINGBUT_B].gpio = GPIOB;
	p[PWM_PINGBUT_B].pinnum = GPIO_PIN_8;
	p[PWM_PINGBUT_B].af = GPIO_AF2_TIM16;
	p[PWM_PINGBUT_B].tim->Instance = TIM16;
	p[PWM_PINGBUT_B].timchan = TIM_CHANNEL_1;
	p[PWM_PINGBUT_B].period = 1024;
}

void init_pwm_out_pin(struct PWMOutput *p) {
	GPIO_InitTypeDef gpio;

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Pull = GPIO_NOPULL;
	gpio.Pin = p->pinnum;
	gpio.Alternate = p->af;
	HAL_GPIO_Init(p->gpio, &gpio);
}

void init_pwm_tim(struct PWMOutput *p)
{
	//Todo: turn on RCC for TIM automatically based on 
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM16_CLK_ENABLE();
	__HAL_RCC_TIM17_CLK_ENABLE();

	TIM_OC_InitTypeDef  tim_oc;

	// p->tim.Instance = p->TIM;
	p->tim->Init.Prescaler = 0;
	p->tim->Init.Period = p->period;
	p->tim->Init.ClockDivision = 0;
	p->tim->Init.CounterMode = TIM_COUNTERMODE_UP;
	p->tim->Init.RepetitionCounter = 0;
	p->tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_PWM_Init(p->tim);

	tim_oc.OCMode = TIM_OCMODE_PWM2;
	tim_oc.OCFastMode = TIM_OCFAST_DISABLE;
	tim_oc.OCPolarity = TIM_OCPOLARITY_LOW;
	tim_oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	tim_oc.OCIdleState = TIM_OCIDLESTATE_SET;
	tim_oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	tim_oc.Pulse = 0;

	HAL_TIM_PWM_ConfigChannel(p->tim, &tim_oc, p->timchan);
}

void start_timers(struct PWMOutput *p)
{
	HAL_TIM_PWM_Start(p->tim, p->timchan);
	// TIM_Cmd(p->tim, ENABLE);
	// TIM_CtrlPWMOutputs(p->tim, ENABLE); //FixMe: does nothing for TIM3?
}


void init_pwm(void)
{

	populate_pwm_pins(pwm);

	for (uint8_t i=0;i<NUM_PWMS;i++)
	{
		init_pwm_out_pin(&pwm[i]);
		init_pwm_tim(&pwm[i]);
	}

	for (uint8_t i=0;i<NUM_PWMS;i++)
		start_timers(&pwm[i]);
}

