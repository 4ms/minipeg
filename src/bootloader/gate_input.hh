#pragma once
#include "dig_inout_pins.hh"
#include "drivers/pin.hh"
#include "drivers/timekeeper.hh"

static inline void init_gate_in() {
	DigIO::TrigJack init;
	DigIO::EOJack debug_out_init;
}

static inline bool gate_in_read() {
	return DigIO::TrigJack::read();
}

static inline void start_reception(uint32_t sample_rate, void callback()) {
	const mdrivlib::TimekeeperConfig conf{
		.TIMx = TIM7,
		.period_ns = 1'000'000'000 / sample_rate,
		.priority1 = 1,
		.priority2 = 1,
	};
	mdrivlib::Timekeeper read_task(conf, callback);
	read_task.start();
}
