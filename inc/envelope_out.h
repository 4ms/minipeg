/*
 * envelope_out.h
 */

#pragma once

#include <stm32f0xx.h>

void output_envelope(uint32_t dacval);
void output_offset(void);
void test_envout(void);