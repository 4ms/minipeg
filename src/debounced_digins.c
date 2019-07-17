#include "globals.h"


debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];

void init_debouncer(void)
{
	for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
	{
		digin[i].debounce_history = 0xFFFF;
		digin[i].state = 0;
		digin[i].edge = 0;
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

void DEBOUNCE_IRQ(void)
{
	uint16_t t;
	uint16_t pin_read;

	if (TIM_GetITStatus(DEBOUNCE_TIMx, TIM_IT_Update) != RESET) {

		for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
		{
			if (i==PING_BUTTON)
				pin_read = PINGBUT;
			
			else if (i==CYCLE_BUTTON)
				pin_read = CYCLEBUT;

			else if (i==TRIGGER_JACK)
				pin_read = TRIGJACK_READ;

			else if (i==CYCLE_JACK)
				pin_read = CYCLEJACK_READ;

			if (pin_read) t=0x0000;
			else t=0x0001;

			digin[i].debounce_history=(digin[i].debounce_history<<1) | t;

			if (digin[i].debounce_history==0xFFFE)
			{
				digin[i].state = 1;
				digin[i].edge = 1;
			}
			else if (digin[i].debounce_history==0x0001)
			{
				digin[i].state = 0;
				digin[i].edge = -1;
			}

		}

		// Clear TIM update interrupt
		TIM_ClearITPendingBit(DEBOUNCE_TIMx, TIM_IT_Update);
	}
}
