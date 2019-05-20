#include "pwm.h"

struct PWMOutput{
	GPIO_TypeDef 	*gpio;
	uint8_t			pinnum;
	uint8_t			af;

	TIM_TypeDef 	*tim;
	uint8_t			timchan;

	uint32_t 		period;
};

enum PwmOutputs {
	PWM_ENV,
	PWM_ENVLED,
	PWM_5VENV,
	PWM_EOFLED,
	PWM_CYCLEBUT_R,
	PWM_CYCLEBUT_G,
	PWM_CYCLEBUT_B,
	PWM_PINGBUT_R,
	PWM_PINGBUT_G,
	PWM_PINGBUT_B,

	NUM_PWMS
}

struct PWMOutput pwm[NUM_PWMS];

//Private:
void init_pwm_out_pin(struct PWMOutput *p);
void populate_pwm_pins(void);


void update_pwm(uint32_t pwmval, uint8_t pwmnum)
{
	pwmval = pwmval >> 2; //12-bit to 10-bit

	channel = pwm[pwmnum].timchan;

	if (channel==0)
		TIM_SetCompare1(pwm[pwmnum].tim, pwmval);

	else if (channel==1)
		TIM_SetCompare2(pwm[pwmnum].tim, pwmval);

	else if (channel==2)
		TIM_SetCompare3(pwm[pwmnum].tim, pwmval);

	else if (channel==3)
		TIM_SetCompare4(pwm[pwmnum].tim, pwmval);

}




void populate_pwm_pins(struct PWMOutput **p) {
	p[PWM_ENV]->gpio = GPIOB;
	p[PWM_ENV]->pinnum = 0;
	p[PWM_ENV]->af = GPIO_AF_2;
	p[PWM_ENV]->tim = TIM3;
	p[PWM_ENV]->timchan = 3;
	p[PWM_ENV]->period = 1024;

	p[PWM_EOFLED]->gpio = GPIOB;
	p[PWM_EOFLED]->pinnum = 1;
	p[PWM_EOFLED]->af = GPIO_AF_2;
	p[PWM_EOFLED]->tim = TIM3;
	p[PWM_EOFLED]->timchan = 4;
	p[PWM_EOFLED]->period = 1024;

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
	tim.TIM_Period = 1024;
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

