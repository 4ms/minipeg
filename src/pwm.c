#include "globals.h"

struct PWMOutput {
	GPIO_TypeDef 	*gpio;
	uint8_t			pinnum;
	uint8_t			af;

	TIM_TypeDef 	*tim;
	uint8_t			timchan;

	uint32_t 		period;
};

struct PWMOutput pwm[NUM_PWMS];

//Private:
void init_pwm_out_pin(struct PWMOutput *p);
void populate_pwm_pins(struct PWMOutput *p);

void update_pwm(uint32_t pwmval, uint8_t pwmnum) {
	uint8_t channel;

	channel = pwm[pwmnum].timchan;

	if (channel==1)
		TIM_SetCompare1(pwm[pwmnum].tim, pwmval);

	else if (channel==2)
		TIM_SetCompare2(pwm[pwmnum].tim, pwmval);

	else if (channel==3)
		TIM_SetCompare3(pwm[pwmnum].tim, pwmval);

	else if (channel==4)
		TIM_SetCompare4(pwm[pwmnum].tim, pwmval);
}
	
void populate_pwm_pins(struct PWMOutput p[]) {
	p[PWM_ENVLED].gpio = GPIOA;
	p[PWM_ENVLED].pinnum = 7;
	p[PWM_ENVLED].af = GPIO_AF_1;
	p[PWM_ENVLED].tim = TIM3;
	p[PWM_ENVLED].timchan = 2;
	p[PWM_ENVLED].period = 1024;

	p[PWM_ENV].gpio = GPIOB;
	p[PWM_ENV].pinnum = 0;
	p[PWM_ENV].af = GPIO_AF_1;
	p[PWM_ENV].tim = TIM3;
	p[PWM_ENV].timchan = 3;
	p[PWM_ENV].period = 1024;

	p[PWM_5VENV].gpio = GPIOA;
	p[PWM_5VENV].pinnum = 6;
	p[PWM_5VENV].af = GPIO_AF_5;
	p[PWM_5VENV].tim = TIM16;
	p[PWM_5VENV].timchan = 1;
	p[PWM_5VENV].period = 1024;

	p[PWM_TRIGOUTLED].gpio = GPIOB;
	p[PWM_TRIGOUTLED].pinnum = 1;
	p[PWM_TRIGOUTLED].af = GPIO_AF_1;
	p[PWM_TRIGOUTLED].tim = TIM3;
	p[PWM_TRIGOUTLED].timchan = 4;
	p[PWM_TRIGOUTLED].period = 1024;

	p[PWM_CYCLEBUT_R].gpio = GPIOA;
	p[PWM_CYCLEBUT_R].pinnum = 8;
	p[PWM_CYCLEBUT_R].af = GPIO_AF_2;
	p[PWM_CYCLEBUT_R].tim = TIM1;
	p[PWM_CYCLEBUT_R].timchan = 1;
	p[PWM_CYCLEBUT_R].period = 1024;

	p[PWM_CYCLEBUT_G].gpio = GPIOA;
	p[PWM_CYCLEBUT_G].pinnum = 9;
	p[PWM_CYCLEBUT_G].af = GPIO_AF_2;
	p[PWM_CYCLEBUT_G].tim = TIM1;
	p[PWM_CYCLEBUT_G].timchan = 2;
	p[PWM_CYCLEBUT_G].period = 1024;

	p[PWM_CYCLEBUT_B].gpio = GPIOA;
	p[PWM_CYCLEBUT_B].pinnum = 10;
	p[PWM_CYCLEBUT_B].af = GPIO_AF_2;
	p[PWM_CYCLEBUT_B].tim = TIM1;
	p[PWM_CYCLEBUT_B].timchan = 3;
	p[PWM_CYCLEBUT_B].period = 1024;


	p[PWM_PINGBUT_R].gpio = GPIOA;
	p[PWM_PINGBUT_R].pinnum = 11;
	p[PWM_PINGBUT_R].af = GPIO_AF_2;
	p[PWM_PINGBUT_R].tim = TIM1;
	p[PWM_PINGBUT_R].timchan = 4;
	p[PWM_PINGBUT_R].period = 1024;

	p[PWM_PINGBUT_G].gpio = GPIOB;
	p[PWM_PINGBUT_G].pinnum = 4;
	p[PWM_PINGBUT_G].af = GPIO_AF_1;
	p[PWM_PINGBUT_G].tim = TIM3;
	p[PWM_PINGBUT_G].timchan = 1;
	p[PWM_PINGBUT_G].period = 1024;

	p[PWM_PINGBUT_B].gpio = GPIOB;
	p[PWM_PINGBUT_B].pinnum = 7;
	p[PWM_PINGBUT_B].af = GPIO_AF_2;
	p[PWM_PINGBUT_B].tim = TIM17;
	p[PWM_PINGBUT_B].timchan = 1;
	p[PWM_PINGBUT_B].period = 1024;
}

void init_pwm_out_pin(struct PWMOutput *p) {
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;

	gpio.GPIO_Pin = 1<<(p->pinnum);
	GPIO_Init(p->gpio, &gpio);
	GPIO_PinAFConfig(p->gpio, p->pinnum, p->af);
}

void init_pwm_tim(struct PWMOutput *p)
{
	//Todo: check if p->tim has already been initialized
	TIM_TimeBaseInitTypeDef  tim;
	TIM_OCInitTypeDef  tim_oc;

	tim.TIM_Prescaler = 0;
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Period = p->period;
	tim.TIM_ClockDivision = 0;
	tim.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(p->tim, &tim);

	tim_oc.TIM_OCMode = TIM_OCMode_PWM2; //Mode 2 means pin is low until OC value reached
	tim_oc.TIM_OutputState = TIM_OutputState_Enable;
	tim_oc.TIM_OutputNState = TIM_OutputNState_Enable;
	tim_oc.TIM_OCPolarity = TIM_OCPolarity_Low;
	tim_oc.TIM_OCNPolarity = TIM_OCNPolarity_High;
	tim_oc.TIM_OCIdleState = TIM_OCIdleState_Set;
	tim_oc.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	tim_oc.TIM_Pulse = 0;

	if (p->timchan==1)
		TIM_OC1Init(p->tim, &tim_oc);

	else if (p->timchan==2)
		TIM_OC2Init(p->tim, &tim_oc);

	else if (p->timchan==3)
		TIM_OC3Init(p->tim, &tim_oc);

	else if (p->timchan==4)
		TIM_OC4Init(p->tim, &tim_oc);
}

void start_timers(struct PWMOutput *p)
{
	TIM_Cmd(p->tim, ENABLE);
	TIM_CtrlPWMOutputs(p->tim, ENABLE); //FixMe: does nothing for TIM3?
}


void init_pwm(void)
{
	//Todo: turn on RCC for TIM automatically based on 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17 , ENABLE);

	populate_pwm_pins(pwm);

	for (uint8_t i=0;i<NUM_PWMS;i++)
	{
		init_pwm_out_pin(&pwm[i]);
		init_pwm_tim(&pwm[i]);
	}

	for (uint8_t i=0;i<NUM_PWMS;i++)
		start_timers(&pwm[i]);
}

