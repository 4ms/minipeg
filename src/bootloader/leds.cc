#include "bootloader/leds.hh"
#include "pwm_pins.hh"
#include "stm32xx.h"

using PingR = mdrivlib::FPin<LEDPins::Pingbut_r.gpio, LEDPins::Pingbut_r.pin>;
using PingG = mdrivlib::FPin<LEDPins::Pingbut_g.gpio, LEDPins::Pingbut_g.pin>;
using PingB = mdrivlib::FPin<LEDPins::Pingbut_b.gpio, LEDPins::Pingbut_b.pin>;

using CycleR = mdrivlib::FPin<LEDPins::Cyclebut_r.gpio, LEDPins::Cyclebut_r.pin>;
using CycleG = mdrivlib::FPin<LEDPins::Cyclebut_g.gpio, LEDPins::Cyclebut_g.pin>;
using CycleB = mdrivlib::FPin<LEDPins::Cyclebut_b.gpio, LEDPins::Cyclebut_b.pin>;

using EnvAR = mdrivlib::FPin<LEDPins::EnvA_r.gpio, LEDPins::EnvA_r.pin>;
using EnvAG = mdrivlib::FPin<LEDPins::EnvA_g.gpio, LEDPins::EnvA_g.pin>;
using EnvAB = mdrivlib::FPin<LEDPins::EnvA_b.gpio, LEDPins::EnvA_b.pin>;

using EnvBR = mdrivlib::FPin<LEDPins::EnvB_r.gpio, LEDPins::EnvB_r.pin>;
using EnvBG = mdrivlib::FPin<LEDPins::EnvB_g.gpio, LEDPins::EnvB_g.pin>;
using EnvBB = mdrivlib::FPin<LEDPins::EnvB_b.gpio, LEDPins::EnvB_b.pin>;

void init_leds() {
	PingR pingr;
	PingG pingg;
	PingB pingb;

	CycleR cycler;
	CycleG cycleg;
	CycleB cycleb;

	EnvAR envar;
	EnvAG envag;
	EnvAB envab;

	EnvBR envbr;
	EnvBG envbg;
	EnvBB envbb;

	set_rgb_led(RgbLeds::Ping, Palette::Black);
	set_rgb_led(RgbLeds::Cycle, Palette::Black);
	set_rgb_led(RgbLeds::EnvA, Palette::Black);
	set_rgb_led(RgbLeds::EnvB, Palette::Black);
}

void set_rgb_led(RgbLeds rgb_led_id, Palette color_id) {

	uint32_t color = static_cast<uint32_t>(color_id);

	if (rgb_led_id == RgbLeds::Ping) {
		PingR::set(color & 0b100);
		PingG::set(color & 0b010);
		PingB::set(color & 0b001);
	} else if (rgb_led_id == RgbLeds::Cycle) {
		CycleR::set(color & 0b100);
		CycleG::set(color & 0b010);
		CycleB::set(color & 0b001);
	} else if (rgb_led_id == RgbLeds::EnvA) {
		EnvAR::set(color & 0b100);
		EnvAG::set(color & 0b010);
		EnvAB::set(color & 0b001);
	} else if (rgb_led_id == RgbLeds::EnvB) {
		EnvBR::set(color & 0b100);
		EnvBG::set(color & 0b010);
		EnvBB::set(color & 0b001);
	}
}
