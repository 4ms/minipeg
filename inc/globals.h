#pragma once

#define ADC_DRIFT 1
#define USER_INPUT_POLL_TIME 400
#define DIV_ADC_HYSTERESIS 1

#define HOLDTIMECLEAR 4800000 //3000000 is about 1.25s
#define LIMIT_SKEW_TIME 15000 //12000 is 5ms
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

enum envelopeShapes {
	EXP,
	EXP25,
	EXP50,
	EXP75,
	LIN,
	LIN25,
	LIN50,
	LIN75,
	LOG
};

enum AdcChannels {
	POT_DIVMULT,
	CV_SHAPE,
	CV_DIVMULT,
	POT_SHAPE,
	POT_OFFSET,
	POT_SCALE,

	NUM_ADCS
};

// static inline uint8_t diff(uint8_t a, uint8_t b);
static inline uint16_t diff(uint16_t a, uint16_t b){
	if (a>b) return (a-b);
	else return (b-a);
}

// uint32_t diff32(uint32_t a, uint32_t b);
static inline uint32_t diff32(uint32_t a, uint32_t b){
	if (a>b) return (a-b);
	else return(b-a);
}
