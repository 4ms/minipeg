/*
 * cal_detent.h
 */

#pragma once

#include <stm32g0xx.h>

#define FIRST_CD_POT POT_SCALE

enum CenterDetentPots{
	DET_SCALE,
	DET_OFFSET,
	DET_SHAPE,

	NUM_CENTER_DETENT_POTS
};

void check_calibration(void);
void default_calibration(void);
