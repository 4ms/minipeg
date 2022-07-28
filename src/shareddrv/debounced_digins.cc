#include "debounced_digins.h"
#include "dig_inouts.h"
#include "drivers/timekeeper.hh"
#include "util/debouncer.hh"
#include "util/zip.hh"

std::array<DebouncedDigin, NUM_DEBOUNCED_DIGINS> digin;
static std::array<Debouncer<0x0001, 0xFFFE, 0xFFFF>, NUM_DEBOUNCED_DIGINS> debouncers;
static mdrivlib::Timekeeper digintmr;

static void debounce_irq(void);

void init_debouncer() {
	HAL_StatusTypeDef err;

	for (auto &d : debouncers)
		d.reset();

	mdrivlib::TimekeeperConfig conf{
		.TIMx = TIM7,
		.period_ns = 1'000'000'000 / 33000,
		.priority1 = 1,
		.priority2 = 2,
	};
	digintmr.init(conf, debounce_irq);
	digintmr.start();
}

static void debounce_irq(void) {
	uint32_t pin_read;

	debouncers[PING_BUTTON].register_state(PINGBUT ? 1 : 0);
	debouncers[CYCLE_BUTTON].register_state(CYCLEBUT ? 1 : 0);
	debouncers[TRIGGER_JACK].register_state(TRIG_JACK_READ ? 1 : 0);
	debouncers[CYCLE_JACK].register_state(AUXTRIG_JACK_READ ? 1 : 0);
	debouncers[PING_JACK].register_state(PING_JACK_READ ? 1 : 0);

	for (auto [dig, deb] : zip(digin, debouncers)) {
		dig.state = deb.is_high() ? 1 : 0;
		if (deb.is_just_pressed())
			dig.edge = 1;
		if (deb.is_just_released())
			dig.edge = -1;
	}
}
