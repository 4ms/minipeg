#include "globals.h"

extern __IO uint32_t systmr;

void delay_ms(uint32_t t)
{ 
	uint32_t start = systmr;
	while (systmr<(start+(t*TICKS_PER_MS))) {;}
}

void delay_ticks(uint32_t t)
{ 
	uint32_t start = systmr;
	while (systmr<(start+t)) {;}
}
