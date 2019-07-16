#include "globals.h"

extern volatile uint32_t pingtmr;
extern volatile uint32_t ping_irq_timestamp;
extern volatile uint8_t using_tap_clock;
// extern volatile uint32_t trig_irq_timestamp;
// extern volatile uint8_t reset_nextping_flag;
// extern volatile uint8_t sync_to_ping_mode;
// extern volatile uint8_t trig_jack_down;

void init_EXTI_inputs(void)
{
 	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	//Set Priority Grouping mode to 2-bits for priority and 2-bits for sub-priority
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_CLOCK_PING_GPIO, EXTI_CLOCK_PING_pin);
	EXTI_InitStructure.EXTI_Line = EXTI_CLOCK_PING_line;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI_CLOCK_PING_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}


void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_CLOCK_PING_line) != RESET)
	{
		if (PINGJACK){
			ping_irq_timestamp=pingtmr;
			pingtmr=0;
			using_tap_clock=0;
		}
		EXTI_ClearITPendingBit(EXTI_CLOCK_PING_line);
	}

	// if(EXTI_GetITStatus(EXTI_CLOCK_TRIG_line) != RESET)
	// {
	// 	if (TRIGJACK){
	// 		trig_irq_timestamp=1;
	// 		// Todo: these were moved to read_trig_jacks(), is that working?
	// 		// sync_to_ping_mode=1;
	// 		// reset_nextping_flag=1;
	// 		trig_jack_down=1;
	// 	} else {
	// 		trig_jack_down=0;
	// 	}
	// 	EXTI_ClearITPendingBit(EXTI_CLOCK_TRIG_line);
	// }
}
