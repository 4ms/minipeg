#include "stm32f0xx_conf.h"
#include "globals.h"
#include "dig_inouts.h"
#include "pwm.h"
#include "adc.h"
#include "log4096.h"



/************************
 * Time keeping			*
 ************************/
volatile uint32_t tapouttmr;
volatile uint32_t tapintmr;
volatile uint32_t pingtmr;
volatile uint32_t divpingtmr;
volatile uint32_t eoftmr;
volatile uint8_t timer_overflowed=0;

volatile uint32_t ping_irq_timestamp=0
volatile uint32_t trig_irq_timestamp=0;
volatile uint8_t reset_nextping_flag=0;
volatile uint8_t sync_to_ping_mode=1;
volatile uint8_t trig_jack_down=0;
volatile uint8_t using_tap_clock=0;

uint8_t eof_high=0;

//Settings
char EOF_IS_TAPCLKOUT=0;
char NO_FREERUNNING_PING=0;
char EOF_IS_HALFRISE=0;
char EOF_IS_TRIG=0;
char LIMIT_SKEW=0;
char ROLLOFF_PING=1;
char TRIG_IS_ASYNC=1;
char TRIG_CAN_SUSTAIN=1;


volatile uint16_t adc_buffer[6];
int16_t t_dacout[6]={0};


void SysTick_Handler(void)
{
	tapoutmr++;
	tapintmr++;
	pingtmr++;
	divpingtmr++;
	eortmr++;
	timer_overflowed++;
}

void init_tmrs(void)
{
	tapoutmr=0;
	tapintmr=0;
	pingtmr=0;
	divpingtmr=0;
	eortmr=0;
}

int8_t get_clk_div_nominal(uint8_t adc_val){
	if (adc_val<=1) 	  // /8 <=0..2 (3)
		return(8);
	else if (adc_val<=10) // /7 <= 3..15 (13)
		return(7);
	else if (adc_val<=26) // /6 <= 16..33 (18)
		return(6);
	else if (adc_val<=46) // /5 <= 34..53 (20)
		return(5);
	else if (adc_val<=65) // /4 <= 54..78 (27)
		return(4);
	else if (adc_val<=85) // /3 <= 79..97 (19)
		return(3);
	else if (adc_val<=100) // /2 <= 98..114 (19)
		return(2);
	else if (adc_val<=118) // =1 <= 117..134 (18)
		return(1);
	else if (adc_val<=128) // x2 <= 135..154 (20)
		return(-2);	
	else if (adc_val<=150) // x3 <= 155..177 (23)
		return(-3);	
	else if (adc_val<=170) // x4 was 196 <= 178..200 (23)
		return(-4);
	else if (adc_val<=188) // x5 <= 201..220 (20)
		return(-5);
	else if (adc_val<=207) // x6 was 246 <= 221..242 ()
		return(-6);
	else if (adc_val<=220) // x7 was 251 <= 243..253 ()
		return(-7);
	else /*if (adc_val<=255)*/ // x8 <=253..255 (3)
		return(-8);
}

uint32_t get_clk_div_time(int8_t clock_divide_amount, uint32_t clk_time){
	if (clock_divide_amount>1)
		return clk_time*clock_divide_amount;
	else if (clock_divide_amount<-1)
		return clk_time/clock_divide_amount;
	else
		return clock_divide_amount;
}


uint32_t get_fall_time(uint8_t skew_adc, uint32_t div_clk_time){
	uint32_t t,u;
	uint8_t rev_skew;

	if (!LIMIT_SKEW || (div_clk_time<(LIMIT_SKEW_TIME>>1)) ){
		if (skew_adc==0)
			return (768);

		else if (skew_adc==1)
			return(1024);

		else if (skew_adc==2)
			return(1280);

		else if (skew_adc<=25){
			t=(skew_adc) * (div_clk_time >> 8);
			u=(skew_adc*skew_adc*64)+960;
		
			if (t<1280) t=1280;
			if (t<u) return t;
			else 
			return u;

		}
		else if (skew_adc>=220) 
			return(div_clk_time-256);
		
		else if (skew_adc>200){
			t=(skew_adc) * (div_clk_time >> 8);
			if (t>(div_clk_time-256)) t= div_clk_time-256;

			rev_skew=255-skew_adc;
			u=rev_skew*rev_skew*64;

			if (u>(div_clk_time-256)){
					return t;
			} else {
				u=div_clk_time-u;
				if(t>u)	return t;
				else return u;
			}
		}
		else if ((skew_adc>101) && (skew_adc<=114))
			return(div_clk_time>>1);
		
		else 
			return ((skew_adc) * (div_clk_time >> 8));
	}

	else { //LIMIT_SKEW

		if ((skew_adc>101) && (skew_adc<=114)){
			return(div_clk_time>>1);
		} else {
			t=(skew_adc) * (div_clk_time >> 8);

			if (t<LIMIT_SKEW_TIME) t=LIMIT_SKEW_TIME;
			if (t>(div_clk_time-LIMIT_SKEW_TIME)) t=div_clk_time-LIMIT_SKEW_TIME;

			return(t);
		}
	}
}

int16_t calc_curve(int16_t t_dacout, char cur_curve){
	//Todo: crossfade between values, cur_curve should be 0..255
	uint16_t t_loga, t_inv_loga;

	t_loga=loga[t_dacout];
	t_inv_loga=4095-loga[4095-t_dacout];

	if (cur_curve==LIN)
		return (t_dacout);
	
	else if (cur_curve==LOG)
		return(t_loga);

	else if (cur_curve==EXP)
		return(t_inv_loga);

	else if (cur_curve==EXP25){	//25% exp 75% lin
		return((t_dacout >> 1) + (t_dacout >> 2) + (t_inv_loga >> 2));
	}
	else if (cur_curve==EXP50)		//50% exp 50% lin
		return((t_inv_loga >> 1) + (t_dacout>>1));

	else if (cur_curve==EXP75){	//75% exp 25% lin
		return((t_inv_loga >> 1) + (t_inv_loga >> 2) + (t_dacout >> 2));
	}

	else if (cur_curve==LIN25){	//25% lin 75% log
		return((t_loga >> 1) + (t_loga >> 2) + (t_dacout >> 2));
	}
	else if (cur_curve==LIN50)	//50% lin 50% log
		return((t_loga >> 1) + (t_dacout>>1));

	else if (cur_curve==LIN75){	//75% lin 25% log
		return((t_dacout >> 1) + (t_dacout >> 2) + (t_loga >> 2));
	}
	else return(t_dacout);
}


void eo1_on(void);
void eo2_on(void);
void eo1_off(void);
void eo2_off(void);

void eor_on(void);
void eor_off(void);
void eof_on(void);
void eof_off(void);
void hr_on(void);
void hr_off(void);
void tapclkout_off(void);
void tapclkout_on(void);

void eo1_on(void){
	if (!eo1_high){
		EO1_ON;
		eo1_high=1;
		reset_eo1tmr();
	}
}

void eo2_on(void){
	if (!eo2_high){
		EO2_ON;
		eo2_high=1;
		reset_eo2tmr();
	}
}

void eo1_off(void){
	if (!EO1_IS_TRIG && (eo1tmr>EO_GATE_TIME)) {
		EO1_OFF;
		reset_eo1tmr();
	}
	eo1_high=0;
}
void eo2_off(void){
	if (!EO2_IS_TRIG && (eo2tmr>EO_GATE_TIME)) {
		EO2_OFF;
		reset_eo2tmr();
	}
	eo2_high=0;
}

inline void eor_on(void){
	if (!EO_IS_HALFRISE)
		eo_on();
}
inline void eor_off(void){
	if (!EO_IS_HALFRISE)
		eo_off();
}

inline void eof_on(void){
	if (!EO_IS_TAPCLKOUT)
		eo_on();
}
inline void eof_off(void){
	if (!EO_IS_TAPCLKOUT)
		eo_off();
}

inline void hr_on(void){
	if (EO_IS_HALFRISE)
		eo_on();
}
inline void hr_off(void){
	if (EO_IS_HALFRISE)
		eo_off();
}

inline void tapclkout_on(void){
	if (EO_IS_TAPCLKOUT) 
		eo_on();
}
inline void tapclkout_off(void){
	if (EO_IS_TAPCLKOUT) 
		eo_off();
}




int main(void)
{



	init_tmrs();
	init_dig_inouts();
	init_adc((uint16_t *)adc_buffer);

	//delay();

	init_pwm();
	SysTick_Config(SystemCoreClock/10000); //1000 is 1ms ticks, 10000 is 100us ticks, 100000 is 10us ticks

	init_EXTI_inputs();









	TAPLED_OFF(1);
	TAPLED_OFF(2);
	TAPLED_OFF(3);
	TAPLED_OFF(0);
	tapled_state[0]=0;
	tapled_state[1]=0;
	tapled_state[2]=0;
	tapled_state[3]=0;

	running[0]=0;
	running[1]=0;
	running[2]=0;
	running[3]=0;

	clk_time[0]=0;
	clk_time[1]=0;
	clk_time[2]=0;
	clk_time[3]=0;

	while (1){

		if (++chan>=4) chan=0;

		/***************** READ PING *********************
		 *  On rising edge of input ping, record the time
		 *  since the last ping. Save this into clk_time
		 *************************************************/

		if (TAPBUT(chan))
		{
			if (!(tapin_up[chan]))
			{
				if (running[chan]==1) running[chan]=2;
				else if (running[chan]==0) running[chan]=1;

				tapled_state[chan]=1;
				TAPLED_ON(chan);
				tapin_up[chan]=1;

				clk_time[chan] = tmr_tapin[chan];

				tmr_tapin[chan]=0;
				tmr_tapout[chan]=0;
				tmr_reset[chan]=0;
				ready_to_reset[chan]=1;

				clk_time_changed[chan]=1;
			}
			//Check for long hold of tap button
			if (tmr_tapin[chan] > HOLDTIMECLEAR)
			{
				if (running[chan])
				{
					TAPLED_OFF(chan);
					tapled_state[chan]=0;
				}
				running[chan]=0;
				//clk_time[chan]=0;
			}
		} else {
			if (tapin_up[chan])
			{
				tapin_up[chan]=0;
				tapled_state[chan]=0;
				TAPLED_OFF(chan);
			}
		}


		if (PING(chan))
		{
			if (!(ping_high[chan])){
				running[chan]=2;

				tapled_state[chan]=1;
				TAPLED_ON(chan);
				ping_high[chan]=1;

				new_clk_time = tmr_ping[chan];

				if (new_clk_time > clk_time[chan])	t32=new_clk_time - clk_time[chan];
				else t32=clk_time[chan]-new_clk_time;

				if (t32>10){ //better would be to make it a percent of clk_time
					clk_time[chan] = new_clk_time;
					clk_time_changed[chan]=1;
				}

				tmr_ping[chan]=0;
				tmr_tapout[chan]=0;
				tmr_reset[chan]=0;
				ready_to_reset[chan]=1;

			}
		} else {
			if (ping_high[chan]){
				ping_high[chan]=0;
				tapled_state[chan]=0;
				TAPLED_OFF(chan);
			}
		}


		//
		// Flash the TAP button LED with a 50% duty cycle
		// Reset Tap LED timer if it overflows the channel's period
		//
		if (running[chan]==2){
			now = tmr_tapout[chan];
			if (tapled_state[chan] && (now >= (clk_time[chan] >> 1)))
			{
				tapled_state[chan]=0;
				TAPLED_OFF(chan);
			}

			if (now >= clk_time[chan]){
				t32 = (now - clk_time[chan]);
				tmr_tapout[chan]=t32;
				tmr_reset[chan]=t32;

				//tmr_tapout[chan]=0;

				tapled_state[chan]=1;
				ready_to_reset[chan]=1;
				TAPLED_ON(chan);
			}
		}


		if (clk_time_changed[chan])
		{
			clk_time_changed[chan]=0;

			tmr_reset[chan]=0;

			//clk_time changed, so see if reset_offset_time is still valid
			//reset it to 0 if it's greater than the ping period or 2ms under the period
			if ((reset_offset_time[chan]+20)>=clk_time[chan]) {
				reset_offset_time[chan]=0;
			}

			ready_to_reset[chan]=1;

			rise_time[chan] = calc_rise_time(skew_adc[chan], clk_time[chan]);
			fall_time[chan] = clk_time[chan] - rise_time[chan];

			rise_inc[chan] = (1<<28)/(rise_time[chan]);
			fall_inc[chan] = (1<<28)/(fall_time[chan]);

		}



		if (RESETJACK(chan))
		{
			if (!reset_up[chan])
			{
			//	if (env_state!=WAIT){
					reset_offset_time[chan]=tmr_reset[chan];

					//see if the new reset_offset_time is still valid
					//reset it to 0 if it's greater than the ping period or 2ms under the period
					if ((reset_offset_time[chan]+20)>=clk_time[chan]) {
						reset_offset_time[chan]=0;
					}
			//	}

				reset_now_flag[chan]=1;
				reset_up[chan]=1;
				ready_to_reset[chan]=0;
			}
			else
			{
				if (env_state[chan]!=WAIT)
					ready_to_reset[chan]=0; //disable resetting if RESET is held high "analog mode"
			}
		}
		else
		{
			reset_up[chan]=0;
		}



		/*******************
		*
		* Reset Lock Point
		*
		********************/

		t32=tmr_reset[chan];

		if (ready_to_reset[chan] && (t32>reset_offset_time[chan]))
		{
			//If we haven't recently modulated skew, then force a reset
			if (is_modulating_skew[chan]==0)
				reset_now_flag[chan]=1;

			if (env_state[chan]==WAIT)
				reset_now_flag[chan]=1;
			ready_to_reset[chan]=0;
		}



		/********************* READ SKEW ADC ************************
		 * 		Read the ADC for the Skew value.       			*
		 *		If it's changed more than ADC_DRIFT,    		*
		 *		re-cacluate the rise and fall times     		*
		 ********************************************************/

		t16=adc_buffer[chan];
		if (skew_adc[chan]>t16)
			t=skew_adc[chan]-t16;
		else
			t=t16-skew_adc[chan];

		if (t>ADC_DRIFT)
		{
			is_modulating_skew[chan]=SKEW_TRACKING_TIME;
			skew_adc[chan]=adc_buffer[chan];

			rise_time[chan] = calc_rise_time(skew_adc[chan], clk_time[chan]);
			fall_time[chan]=clk_time[chan]-rise_time[chan];

			rise_inc[chan]=(1<<28)/(rise_time[chan]);
			fall_inc[chan]=(1<<28)/(fall_time[chan]);

		}
		else
		{
			if (is_modulating_skew[chan]>0)
				is_modulating_skew[chan]--;
		}


		/**************** UPDATE THE ENVELOPE *******************
		 *  Update only when timer has overflowed
		 * -restart if needed
		 * -calculate new position
		 * -change curve step (RISE/FALL)
		 ********************************************************/

		if (chan==3)
		{

			t_of=timer_overflowed;

			if (t_of /*|| reset_now_flag[i]*/)
			{

				for (i=0;i<4;i++)
				{

					// Handle the reset_now_flag by resetting the envelope
					if (reset_now_flag[i])
					{
						reset_now_flag[i]=0;

						env_state[i]=RISE;

						if (skew_adc[i]<=PLUCKY_CURVE_ADC)
						{
							if (skew_adc[i]<=TRIGOUT_CURVE_ADC)
								do_plucky_curve[i]=1;
							else
								do_plucky_curve[i]=skew_adc[i];
						}
						else
						{
							do_plucky_curve[i]=0;
						}

						accum[i]=0;
						sample_ctr[i]=0;
					}

					t_dacout[i]=0;
					sample_ctr[i]++;
					switch (env_state[i])
					{
						case(RISE):
							accum[i] += rise_inc[i] * t_of;
							t_dacout[i]=accum[i]>>18;

							if (t_dacout[i]>=1023)
							{
								accum[i]=1023<<18;
								t_dacout[i]=1023;
								env_state[i]=FALL;
							}
						break;


						case(FALL):

							if (accum[i] > (fall_inc[i] * t_of))
							accum[i] -= fall_inc[i] * t_of;
							else accum[i]=0;

							t_dacout[i] = accum[i]>>18;

							if ((t_dacout[i]<1) || (t_dacout[i]>1023)){
								accum[i]=0;
								t_dacout[i]=0;

								#ifndef FREERUN
									env_state[i]=WAIT;

									//when modulating skew with a slower LFO:
									//usually we don't get to env_state=RISE because of the clk_time comparison, but sometimes it's because is_mod_skew==0
									if (RESETJACK(i) || (is_modulating_skew[i]>0)){

										t32=tmr_reset[i]>>1; //half the time since the last ping

										if (t32<clk_time[i])//if time since last ping is less than twice the LFO period
										{
											env_state[i]=RISE;

											if (skew_adc[i]<=PLUCKY_CURVE_ADC)
											{
												if (skew_adc[i]<=TRIGOUT_CURVE_ADC)
													do_plucky_curve[i]=1;
												else
													do_plucky_curve[i] = skew_adc[i];
											}
											else
											{
												do_plucky_curve[i]=0;
											}
											sample_ctr[i]=0;

										}
									}
								#else
									reset_now_flag[i]=1;
								#endif
								//clk_time_changed[i]=0;
							}
						break;

						default:
							env_state[i]=WAIT;

							accum[i]=0;
							t_dacout[i]=0;
						break;

					}

					if (do_plucky_curve[i])
					{
						//TRIGOUT
						if (do_plucky_curve[i]==1)
						{
							if (sample_ctr[i]<TRIGOUT_WIDTH)
								t_dacout[i]=1023;
							else
								t_dacout[i]=1;
						}
						else //CURVEOUT
						{
							//square
							t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;

							//cubic
							if (do_plucky_curve[i]<=PLUCKY_CUBIC_ADC)
								t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;

							if (do_plucky_curve[i]<=PLUCKY_QUADRATIC_ADC)
								t_dacout[i]=(((int32_t)t_dacout[i])*((int32_t)t_dacout[i]))>>10;
						}

					}

				} //loop i

				timer_overflowed-=t_of;

				TIM_SetCompare1(TIM1, t_dacout[0]);
				TIM_SetCompare2(TIM1, t_dacout[1]);
				TIM_SetCompare3(TIM1, t_dacout[2]);
				TIM_SetCompare4(TIM1, t_dacout[3]);

			} //if (t_of)
		} //chan==3
	}
}
