#include "dac.h"
#include "dig_inouts.h"
#include "hardware_test_util.h"
#include "stm32xx.h"

const uint32_t kNumSineWavePoints = 60;
const uint16_t sinewave[kNumSineWavePoints] = {
	0x07ff, 0x08cb, 0x0994, 0x0a5a, 0x0b18, 0x0bce, 0x0c79, 0x0d18, 0x0da8, 0x0e29, 0x0e98, 0x0ef4,
	0x0f3e, 0x0f72, 0x0f92, 0x0f9d, 0x0f92, 0x0f72, 0x0f3e, 0x0ef4, 0x0e98, 0x0e29, 0x0da8, 0x0d18,
	0x0c79, 0x0bce, 0x0b18, 0x0a5a, 0x0994, 0x08cb, 0x07ff, 0x0733, 0x066a, 0x05a4, 0x04e6, 0x0430,
	0x0385, 0x02e6, 0x0256, 0x01d5, 0x0166, 0x010a, 0x00c0, 0x008c, 0x006c, 0x0061, 0x006c, 0x008c,
	0x00c0, 0x010a, 0x0166, 0x01d5, 0x0256, 0x02e6, 0x0385, 0x0430, 0x04e6, 0x05a4, 0x066a, 0x0733};

static uint16_t interpolate(float phase) {
	while (phase >= 1.0f)
		phase -= 1.0f;
	while (phase < 0.0f)
		phase += 1.0f;
	phase *= (float)kNumSineWavePoints;

	uint8_t phase_i = (uint16_t)phase;
	float phase_f = phase - (float)phase_i;
	float inv_phase_f = 1.0f - phase_f;
	uint8_t next_i = (phase_i >= (kNumSineWavePoints - 1)) ? 0 : phase_i + 1;
	float interp_val = (inv_phase_f * sinewave[phase_i]) + (phase_f * sinewave[next_i]);
	return (uint16_t)interp_val;
}

const float kDacSampleRate = 21000.0f;
float freqHz_a = 40;
float freqHz_b = 40;

static void update_test_waves() {

	static float phase_a = 0.0f;
	static float phase_b = 0.0f;

	phase_a += freqHz_a / kDacSampleRate; //0.0002f;
	if (phase_a >= 1.0f)
		phase_a -= 1.0f;
	uint16_t enva = interpolate(phase_a);
	dac_out(DAC_ENVA, enva);

	phase_b += freqHz_b / kDacSampleRate; //0.001f;
	if (phase_b >= 1.0f)
		phase_b -= 1.0f;
	uint16_t envb = interpolate(phase_b);
	dac_out(DAC_ENVB, envb);
}

void test_dac() {
	init_dac((uint32_t)kDacSampleRate, &update_test_waves);
	resume_dac_timer();

	bool cycledown = false;
	while (!hardwaretest_continue_button()) {
		if (CYCLEBUT) {
			if (!cycledown) {
				cycledown = true;
				freqHz_a *= 2;
				freqHz_b *= 2;
				if (freqHz_a > 20000)
					freqHz_a = 1;
				if (freqHz_b > 20000)
					freqHz_b = 1;
			}
		} else
			cycledown = false;
	}
	pause_until_button_released();
}
