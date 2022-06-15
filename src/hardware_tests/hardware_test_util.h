#pragma once
#include <stdint.h>
#include "settings.h"

const uint16_t max_pwm = 4095;
const uint16_t min_pwm = 0;

void set_button_led(uint8_t button_num, bool turn_on);
void set_led(uint8_t led_num, bool newstate);
bool read_button(uint8_t button_num);

bool hardwaretest_continue_button(void);
void pause_until_button_released(void);
void pause_until_button_pressed(void);
