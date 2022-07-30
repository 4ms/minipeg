#include "dig_inout_pins.hh"
#include "drivers/pin.hh"
#include "drivers/timekeeper.hh"

namespace
{
mdrivlib::Timekeeper read_task;
}

void init_gate_in() {
	DigIO::TrigJack init;
	DigIO::EOJack debug_out_init;
}

bool gate_in_read() {
	return DigIO::TrigJack::read();
}

void start_reception(uint32_t sample_rate, std::function<void()> &&callback) {
	const mdrivlib::TimekeeperConfig conf{
		.TIMx = TIM7,
		.period_ns = 1'000'000'000 / sample_rate,
		.priority1 = 1,
		.priority2 = 1,
	};
	read_task.init(conf, std::move(callback));
	read_task.start();
}
