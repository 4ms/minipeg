#include "trigout.h"
#include "globals.h"
#include "dig_inouts.h"

uint8_t trigout_high=0;
extern volatile uint32_t trigouttmr;

extern char TRIGOUT_IS_ENDOFRISE;
extern char TRIGOUT_IS_ENDOFFALL;
extern char TRIGOUT_IS_HALFRISE;
extern char TRIGOUT_IS_TAPCLKOUT;
extern char TRIGOUT_IS_TRIG;


//Private:
void trigout_on(void);
void trigout_off(void);


void trigout_on(void){
	if (!trigout_high){
		TRIGOUT_ON;
		trigout_high=1;
		trigouttmr=0;
	}
}
void trigout_off(void){
	if (!TRIGOUT_IS_TRIG && (trigouttmr>TRIGOUT_MIN_GATE_TIME)) {
		TRIGOUT_OFF;
		trigouttmr=0;
	}
	trigout_high=0;
}

void handle_trigout_trigfall(void){
	if (TRIGOUT_IS_TRIG && trigouttmr>TRIGOUT_TRIG_TIME) {
		TRIGOUT_OFF;
		trigouttmr=0;
	}
}

//Todo: verify these are inlined by the compiler, otherwise explicity do it
void eor_on(void){
	if (TRIGOUT_IS_ENDOFRISE)
		trigout_on();
}
void eor_off(void){
	if (TRIGOUT_IS_ENDOFRISE)
		trigout_off();
}

void eof_on(void){
	if (TRIGOUT_IS_ENDOFFALL)
		trigout_on();
}
void eof_off(void){
	if (TRIGOUT_IS_ENDOFFALL)
		trigout_off();
}

void hr_on(void){
	if (TRIGOUT_IS_HALFRISE)
		trigout_on();
}
void hr_off(void){
	if (TRIGOUT_IS_HALFRISE)
		trigout_off();
}

void tapclkout_on(void){
	if (TRIGOUT_IS_TAPCLKOUT) 
		trigout_on();
}
void tapclkout_off(void){
	if (TRIGOUT_IS_TAPCLKOUT) 
		trigout_off();
}
