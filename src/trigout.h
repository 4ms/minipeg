/*
 * trigout.h
 */

#pragma once

#include <stm32g0xx.h>

void eor_on(void);
void eor_off(void);
void eof_on(void);
void eof_off(void);
void hr_on(void);
void hr_off(void);
void tapclkout_off(void);
void tapclkout_on(void);
void handle_trigout_trigfall(void);
