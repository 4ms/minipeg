#include "globals.h"

struct PWMOutput {
	GPIO_TypeDef 	*gpio;
	uint32_t		pinnum;
	uint8_t			af;

	TIM_TypeDef 	*tim;
	uint8_t			timchan;

	uint32_t 		period;
};

struct PWMOutput pwm[NUM_PWMS];

//Private:
void init_pwm_out_pin(struct PWMOutput *p);
void populate_pwm_pins(struct PWMOutput *p);

void update_pwm(uint32_t pwmval, enum PwmOutputs pwmnum) {
	uint8_t channel;

	channel = pwm[pwmnum].timchan;

	if (channel==TIM_CHANNEL_1)
		TIM_SetCompare1(pwm[pwmnum].tim, pwmval);

	else if (channel==TIM_CHANNEL_2)
		TIM_SetCompare2(pwm[pwmnum].tim, pwmval);

	else if (channel==TIM_CHANNEL_3)
		TIM_SetCompare3(pwm[pwmnum].tim, pwmval);

	else if (channel==TIM_CHANNEL_4)
		TIM_SetCompare4(pwm[pwmnum].tim, pwmval);
}

void populate_pwm_pins(struct PWMOutput p[]) {
	p[PWM_5VENV].gpio = GPIOA;
	p[PWM_5VENV].pinnum = GPIO_PIN_7;
	p[PWM_5VENV].af = GPIO_AF4_TIM14;
	p[PWM_5VENV].tim = TIM15;
	p[PWM_5VENV].timchan = TIM_CHANNEL_1;
	p[PWM_5VENV].period = 1024;

	p[PWM_5VENVLED_G].gpio = GPIOA;
	p[PWM_5VENVLED_G].pinnum = GPIO_PIN_2;
	p[PWM_5VENVLED_G].af = GPIO_AF5_TIM15;
	p[PWM_5VENVLED_G].tim = TIM15;
	p[PWM_5VENVLED_G].timchan = TIM_CHANNEL_1;
	p[PWM_5VENVLED_G].period = 1024;

	p[PWM_ENV].gpio = GPIOA;
	p[PWM_ENV].pinnum = GPIO_PIN_8;
	p[PWM_ENV].af = GPIO_AF2_TIM1;
	p[PWM_ENV].tim = TIM1;
	p[PWM_ENV].timchan = TIM_CHANNEL_1;
	p[PWM_ENV].period = 1024;

////
	p[PWM_ENVLED_B].gpio = GPIOA;
	p[PWM_ENVLED_B].pinnum = GPIO_PIN_8;
	p[PWM_ENVLED_B].af = GPIO_AF_2;
	p[PWM_ENVLED_B].tim = TIM1;
	p[PWM_ENVLED_B].timchan = TIM_CHANNEL_1;
	p[PWM_ENVLED_B].period = 1024;

	p[PWM_ENVLED_R].gpio = GPIOA;
	p[PWM_ENVLED_R].pinnum = GPIO_PIN_9;
	p[PWM_ENVLED_R].af = GPIO_AF_2;
	p[PWM_ENVLED_R].tim = TIM1;
	p[PWM_ENVLED_R].timchan = TIM_CHANNEL_2;
	p[PWM_ENVLED_R].period = 1024;

	p[PWM_CYCLEBUT_BG].gpio = GPIOA;
	p[PWM_CYCLEBUT_BG].pinnum = GPIO_PIN_10;
	p[PWM_CYCLEBUT_BG].af = GPIO_AF_2;
	p[PWM_CYCLEBUT_BG].tim = TIM1;
	p[PWM_CYCLEBUT_BG].timchan = TIM_CHANNEL_3;
	p[PWM_CYCLEBUT_BG].period = 1024;

	p[PWM_CYCLEBUT_R].gpio = GPIOA;
	p[PWM_CYCLEBUT_R].pinnum = GPIO_PIN_11;
	p[PWM_CYCLEBUT_R].af = GPIO_AF_2;
	p[PWM_CYCLEBUT_R].tim = TIM1;
	p[PWM_CYCLEBUT_R].timchan = TIM_CHANNEL_4;
	p[PWM_CYCLEBUT_R].period = 1024;

	p[PWM_PINGBUT_RG].gpio = GPIOB;
	p[PWM_PINGBUT_RG].pinnum = GPIO_PIN_4;
	p[PWM_PINGBUT_RG].af = GPIO_AF_1;
	p[PWM_PINGBUT_RG].tim = TIM3;
	p[PWM_PINGBUT_RG].timchan = TIM_CHANNEL_1;
	p[PWM_PINGBUT_RG].period = 1024;

	p[PWM_PINGBUT_B].gpio = GPIOB;
	p[PWM_PINGBUT_B].pinnum = GPIO_PIN_7;
	p[PWM_PINGBUT_B].af = GPIO_AF_2;
	p[PWM_PINGBUT_B].tim = TIM17;
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
	//Todo: check if p->tim has already been initialized
	TIM_HandleTypeDef  tim;
	TIM_OCInitTypeDef  tim_oc;

	tim.Instance = p->TIM;
	tim.Init.Prescaler = 0;
	tim.Init.Period = p->period;
	tim.Init.ClockDivision = 0;
	tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim.Init.RepetitionCounter = 0;
	tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_PWM_Init(&tim);

	tim_oc.OCMode = TIM_OCMODE_PWM2;
	tim_oc.OCFastMode = TIM_OCFAST_DISABLE;
	tim_oc.OCPolarity = TIM_OCPOLARITY_LOW;
	tim_oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	tim_oc.OCIdleState = TIM_OCIDLESTATE_SET;
	tim_oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	tim_oc.Pulse = 0;

	HAL_TIM_PWM_ConfigChannel(&tim, &tim_oc, p->timchan);

 	HAL_TIM_PWM_Start(&tim, p->timchan);
}

void start_timers(struct PWMOutput *p)
{
	TIM_Cmd(p->tim, ENABLE);
	TIM_CtrlPWMOutputs(p->tim, ENABLE); //FixMe: does nothing for TIM3?
}


void init_pwm(void)
{
	//Todo: turn on RCC for TIM automatically based on 
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM16_CLK_ENABLE();
  __HAL_RCC_TIM17_CLK_ENABLE();

	populate_pwm_pins(pwm);

	for (uint8_t i=0;i<NUM_PWMS;i++)
	{
		init_pwm_out_pin(&pwm[i]);
		init_pwm_tim(&pwm[i]);
	}

	for (uint8_t i=0;i<NUM_PWMS;i++)
		start_timers(&pwm[i]);
}

