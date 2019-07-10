/*
 * flash_user.h
 */

#pragma once

#include <stm32f0xx.h>

#define FLASH_SET 0b10101010
#define FLASH_CLEAR 0b00000000
#define FLASH_UNPROGRAMMED 0b11111111

#define TAPCLK_FLASHADDR 16
#define PING_FLASHADDR 17
#define LIMIT_SKEW_FLASHADDR 18
#define HALFRISE_FLASHADDR 19
#define TRIGOUT_TRIG_FLASHADDR 21
#define ASYNC_SUSTAIN_FLASHADDR 20

void write_calibration(uint8_t num_bytes, uint8_t *data);
void read_calibration(uint8_t num_bytes, uint8_t *data);
uint8_t is_calibrated(void);
