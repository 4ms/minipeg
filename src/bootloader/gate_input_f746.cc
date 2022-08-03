#include "adc.h"
#include "adc_pins.h"
#include "analog_conditioning.h"
#include "dig_inout_pins.hh"
#include "drivers/arch.hh"
#include "drivers/pin.hh"
#include "drivers/timekeeper.hh"
#include <functional>

namespace
{
mdrivlib::Timekeeper read_task;
uint16_t buf;
} // namespace

void init_gate_in() {
	DigIO::TrigJack init;
	DigIO::EOJack debug_out_init;
}

bool gate_in_read() {
	bool j = DigIO::TrigJack::read();
	if (j)
		DigIO::EOJack::high();
	else
		DigIO::EOJack::low();
	return j;
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
