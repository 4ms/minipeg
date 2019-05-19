/*
 * dig_inouts.c
 */

#include "dig_inouts.h"

struct PWMOutputPin{
	GPIO_TypeDef 	*gpio;
	uint8_t			pinnum;
	uint8_t			af;
};

void init_pwm_out_pin(struct PWMOutputPin *p){
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

void init_dig_inouts(void){
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);

	RCC_AHBPeriphClockCmd(ALL_GPIO_RCC, ENABLE);

	//Configure outputs
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	gpio.GPIO_Pin = EOF_pin;	GPIO_Init(EOF_GPIO, &gpio);


	//PWM output pins
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;

	gpio.GPIO_Pin = ENV_PWM_pin; GPIO_Init(ENV_PWM_GPIO, &gpio);
	GPIO_PinAFConfig(ENV_PWM_GPIO, ENV_PWM_source, ENV_PWM_AF);

	//Configure inputs
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;

	//TAP Buttons
	RCC_AHBPeriphClockCmd(TAPBUT_RCC, ENABLE);
	gpio.GPIO_Pin = TAPBUT1_pin | TAPBUT2_pin | TAPBUT3_pin | TAPBUT4_pin;	GPIO_Init(TAPBUT_GPIO, &gpio);

	//Ping jacks
	RCC_AHBPeriphClockCmd(PING_RCC, ENABLE);
	gpio.GPIO_Pin = PING1_pin | PING2_pin | PING3_pin | PING4_pin;	GPIO_Init(PING_GPIO, &gpio);

	//Reset jacks
	RCC_AHBPeriphClockCmd(RESET_RCC, ENABLE);
	gpio.GPIO_Pin = RESET1_pin | RESET2_pin | RESET3_pin;	GPIO_Init(RESET123_GPIO, &gpio);
	gpio.GPIO_Pin = RESET4;	GPIO_Init(RESET4_GPIO, &gpio);

}

/**exti_ins.c**/

void init_EXTI_inputs(void)
{
/*
 	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	  //Set Priority Grouping mode to 2-bits for priority and 2-bits for sub-priority
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  SYSCFG_EXTILineConfig(EXTI_PINGJACK_GPIO, EXTI_PINGJACK_pin);
	  EXTI_InitStructure.EXTI_Line = EXTI_PINGJACK_line;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  NVIC_InitStructure.NVIC_IRQChannel = EXTI_PINGJACK_IRQ;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	  NVIC_Init(&NVIC_InitStructure);
*/

}

/*
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_PINGJACK_line) != RESET)
  {
	  //This should be software de-bounced, we can't rely on the external clock having a perfectly sharp rising edge...
	  //Probably need to set the interrupt to record both edges, and ignore the up if a down happens shortly thereafter...?
	  //Or grab PEG's ping code, which grabs the ping_tmr in the interrupt uses that to flag the main loop to check again if the jack is still high
	  //..and if so, then it initiates the new timing information


	  ping_button_state = 0;

		if (ping_jack_state==1){ //second time we got a rising edge
			ping_jack_state = 0;

			//Log how much time has elapsed since last ping
			ping_time=ping_tmr;

			//Reset the timers
			ping_ledbut_tmr=0;
			clkout_trigger_tmr=0;

			//Flag to update the divmult parameters
			flag_ping_was_changed=1;
		} else {

			// This is the first rising edge, so start the ping timer
			ping_tmr = 0;
			ping_jack_state = 1;
		}

    EXTI_ClearITPendingBit(EXTI_PINGJACK_line);
  }

}
*/









/*


void debounce_inputs(void)
{
	static uint16_t State[2] = {0,0,0,0,0,0,0,0,0,0}; // Current debounce status

	if (INF1BUT) t=0xe000; else t=0xe001;
	State[1]=(State[1]<<1) | t;
	if (State[1]==0xf000){
		//handle pin going high
	}
}

*/
