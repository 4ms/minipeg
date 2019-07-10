#include "globals.h"


button_t buttons[NUM_BUTTONS];

void init_buttons(void)
{
	for (uint8_t i=0; i<NUM_BUTTONS; i++)
	{
		buttons[i].debounce_history = 0xFFFF;
		buttons[i].state = 0;
	}

	RCC_APB1PeriphClockCmd(DEBOUNCE_TIM_RCC, ENABLE);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = DEBOUNCE_IRQn;
	nvic.NVIC_IRQChannelPriority = 2;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	TIM_TimeBaseInitTypeDef  tim;
	TIM_TimeBaseStructInit(&tim);
	tim.TIM_Period = 30000;
	tim.TIM_Prescaler = 0;
	tim.TIM_ClockDivision = 0;
	tim.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(DEBOUNCE_TIMx, &tim);
	TIM_ITConfig(DEBOUNCE_TIMx, TIM_IT_Update, ENABLE);
	TIM_Cmd(DEBOUNCE_TIMx, ENABLE);
}

void BUTTON_DEBOUNCE_IRQ(void)
{
	uint16_t t;
	uint8_t but_read;

	if (TIM_GetITStatus(DEBOUNCE_TIMx, TIM_IT_Update) != RESET) {

		for (uint8_t i=0; i<NUM_BUTTONS; i++)
		{
			if (i==0) but_read = PINGBUT;
			else if (i==1) but_read = CYCLEBUT;

			if (but_read) t=0x0000;
			else t=0x0001;

			buttons[i].debounce_history=(buttons[i].debounce_history<<1) | t;

			if (buttons[i].debounce_history==0xff00)
			{
				buttons[i].state = 1;
				buttons[i].edge = 1;
			}
			else if (buttons[i].debounce_history==0x00ff)
			{
				buttons[i].state = 0;
				buttons[i].edge = -1;
			}

		}

		// Clear TIM update interrupt
		TIM_ClearITPendingBit(DEBOUNCE_TIMx, TIM_IT_Update);
	}
}
