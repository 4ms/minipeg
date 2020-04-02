/*
 * envelope_out.h
 */
#pragma once
#include <stddef.h>

void update_envelope();
void output_envelope(uint32_t dacval);
void output_offset(void);
