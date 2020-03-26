#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum DACs {
	DAC_ENVA,
	DAC_ENVB,
};

void init_dac();
void dac_out(enum DACs dac, uint16_t val);
void assign_dac_update_callback(void (*callbackfunc)(void));

#ifdef __cplusplus
}
#endif
