#include "globals.h"
extern volatile uint32_t pingtmr;
extern volatile uint32_t ping_irq_timestamp;
extern volatile uint8_t using_tap_clock;


debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];

void init_debouncer(void)
{
	for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
	{
		digin[i].history = 0xFFFF;
		digin[i].state = 0;
		digin[i].edge = 0;
	}

	RCC_APB1PeriphClockCmd(DEBOUNCE_TIM_RCC, ENABLE);

	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = DEBOUNCE_IRQn;
	nvic.NVIC_IRQChannelPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	TIM_TimeBaseInitTypeDef  tim;
	TIM_TimeBaseStructInit(&tim);
	tim.TIM_Period = 5000;
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

			else if (i==PING_JACK)
				pin_read = PINGJACK;

			if (pin_read) t=0x0000;
			else t=0x0001;

			digin[i].history=(digin[i].history<<1) | t;

			if (digin[i].history==0xFFFE)
			{
				digin[i].state = 1;
				digin[i].edge = 1;
				if (i==PING_JACK) {
					ping_irq_timestamp=pingtmr;
					pingtmr=0;
					using_tap_clock=0;
				}
			}
			else if (digin[i].history==0x0001)
			{
				digin[i].state = 0;
				digin[i].edge = -1;
			}

		}

		// Clear TIM update interrupt
		TIM_ClearITPendingBit(DEBOUNCE_TIMx, TIM_IT_Update);
	}
}
