/*
 * globals.h
 */

#pragma once
#include "adc.h"
#include "analog_conditioning.h"
#include "calibration.h"
#include "debounced_digins.h"
#include "dig_inouts.h"
#include "envelope_calcs.h"
#include "flash.h"
#include "flash_user.h"
#include "leds.h"
#include "pwm.h"
#include "settings.h"
#include "system.h"
#include "system_mode.h"
#include "trigout.h"
#if defined(STM32G431xx)
#include <stm32f7xx.h>
#elif defined(STM32F746xx)
#include <stm32f7xx.h>
#else
#error "No STM32xx defined. Please add the build flag -DSTM32G431xx or -DSTM32F746xx"
#endif
