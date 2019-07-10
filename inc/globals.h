/*
 * globals.h
 */

#pragma once

#define TRIGDEBUGMODE


#include <stm32f0xx.h>
#include "adc.h"
#include "cal_detent.h"
#include "delay.h"
#include "dig_inouts.h"
#include "envelope_calcs.h"
#include "envelope_out.h"
#include "exti.h"
#include "flash_user.h"
#include "leds.h"
#include "pwm.h"
#include "trigout.h"
#include "buttons.h"
#include "analog_conditioning.h"


#define ADC_DRIFT 16
#define USER_INPUT_POLL_TIME 400
#define DIV_ADC_HYSTERESIS 16

#define HOLDTIMECLEAR 20000 //4800000
#define LIMIT_SKEW_TIME 50
#define NUM_ADC_CYCLES_BEFORE_TRANSITION 10 //10 is about 100ms

//SYSTEM_MODE_HOLD_TIME: how long the ping button must be held down to enter System Mode
//200000 is about 5s
//150000 is about 3.75s
//103000 is about 2.5
#define SYSTEM_MODE_HOLD_TIME 130000
#define SYSTEM_MODE_EXIT_TIME 0x00080000
#define SYSTEM_MODE_CYCLE_FLASH_BRIGHT_ON 15000
#define SYSTEM_MODE_CYCLE_FLASH_BRIGHT_OFF 25000
#define SYSTEM_MODE_CYCLE_FLASH_DIM_ON 5000
#define SYSTEM_MODE_CYCLE_FLASH_DIM_OFF 50000

//119 is 13.15ms
//91 is 10.0ms
//46 is 5ms
//37 is 4ms
//9 is 1.1ms
//4 is 500us
//1 is 212us
#define TRIGOUT_TRIG_TIME 91
#define TRIGOUT_MIN_GATE_TIME 37

//The following are not user modifiable, change at your own risk!
#define QNT_REPHASES_WHEN_CYCLE_OFF 0
#define CYCLE_REPHASES_DIV_PING 1

enum envelopeStates {
	WAIT = 0,
	RISE = 1,
	SUSTAIN = 2,
	FALL = 3,
	TRANSITION = 4
};


static inline uint16_t diff(uint16_t a, uint16_t b){
	if (a>b) return (a-b);
	else return (b-a);
}

static inline uint32_t diff32(uint32_t a, uint32_t b){
	if (a>b) return (a-b);
	else return(b-a);
}

