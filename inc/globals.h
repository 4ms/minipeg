/*
 * globals.h
 */

#pragma once

#define TRIGDEBUGMODE


#include <stm32g4xx.h>
#include "adc.h"
#include "calibration.h"
#include "dig_inouts.h"
#include "envelope_calcs.h"
#include "envelope_out.h"
#include "flash_user.h"
#include "leds.h"
#include "pwm.h"
#include "trigout.h"
#include "debounced_digins.h"
#include "analog_conditioning.h"
#include "system_mode.h"
#include "flash.h"
#include "system.h"


#define TICKS_PER_MS 1


#define HOLDTIMECLEAR 20000 //4800000
#define LIMIT_SKEW_TIME 50

//SYSTEM_MODE_HOLD_TIME: how long the ping button must be held down to enter System Mode
//200000 is about 5s
//150000 is about 3.75s
//103000 is about 2.5
#define SYSTEM_MODE_HOLD_TIME 130000
#define SYSTEM_MODE_EXIT_TIME 260000

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





