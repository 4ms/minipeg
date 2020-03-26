#include "globals.h"
#include "debounced_digins.h"
#include "timekeeper.h"

#define DEBOUNCE_TIM_NUM 7


debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];

static void debounce_irq(void);

void init_debouncer(void)
{
    HAL_StatusTypeDef err;

    for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
    {
        digin[i].history = 0xFFFF;
        digin[i].state = 0;
        digin[i].edge = 0;
    }
   	
	TimerITInitStruct debounce_timer_config;

	debounce_timer_config.priority1 = 1;
	debounce_timer_config.priority2 = 0;
	debounce_timer_config.period = 5000;
	debounce_timer_config.prescaler = 0;
	debounce_timer_config.clock_division = 0;

	init_timer_IRQ(DEBOUNCE_TIM_NUM, &debounce_timer_config);
	start_timer_IRQ(DEBOUNCE_TIM_NUM, &debounce_irq);
}

extern volatile uint32_t pingtmr;
extern volatile uint32_t ping_irq_timestamp;
extern volatile uint8_t using_tap_clock;

static void debounce_irq(void) {
	uint8_t pin_read;
	uint8_t t;

	for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
	{
		if (i==PING_BUTTON)
			pin_read = PINGBUT;

		else if (i==CYCLE_BUTTON)
			pin_read = CYCLEBUT;

		else if (i==TRIGGER_JACK)
			pin_read = TRIG_JACK_READ;

		else if (i==CYCLE_JACK)
			pin_read = CYCLE_JACK_READ;

		else if (i==PING_JACK)
			pin_read = PING_JACK_READ;

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
}
