#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum DACs {
	DAC_ENVA,
	DAC_ENVB,
};

void init_dac(uint32_t freq);
void dac_out(enum DACs dac, uint16_t val);
void assign_dac_update_callback(void (*callbackfunc)(void));
void pause_dac_timer(void);
void resume_dac_timer(void);

#ifdef __cplusplus
}
#endif
