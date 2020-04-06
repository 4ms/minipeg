#pragma once
#include <stdint.h>
#include "pingable_env.h"

void reset_transition_counter(void);
void force_transition(void);
uint8_t check_to_start_transition(void);

void do_start_transition(struct PingableEnvelope *e);
void start_transition(struct PingableEnvelope *e, uint32_t elapsed_time);

