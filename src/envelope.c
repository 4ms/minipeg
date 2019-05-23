#include "envelope.h"
#include "log4096.h"

extern const uint16_t loga[4096];

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
