#include "debounced_digins.h"
#include "dig_inouts.h"
#include "drivers/timekeeper.hh"
#include "util/debouncer.hh"

debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
static std::array<Debouncer<0x0001, 0xFFFE, 0xFFFF>, NUM_DEBOUNCED_DIGINS> debouncers;
static mdrivlib::Timekeeper digintmr;

static void debounce_irq(void);

extern "C" void init_debouncer() {
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

// extern volatile uint32_t pingtmr;
// extern volatile uint32_t ping_irq_timestamp;
// extern volatile uint8_t using_tap_clock;

static void debounce_irq(void) {
	uint32_t pin_read;

	debouncers[PING_BUTTON].register_state(PINGBUT ? 1 : 0);
	debouncers[CYCLE_BUTTON].register_state(CYCLEBUT ? 1 : 0);
	debouncers[TRIGGER_JACK].register_state(TRIG_JACK_READ ? 1 : 0);
	debouncers[CYCLE_JACK].register_state(AUXTRIG_JACK_READ ? 1 : 0);
	debouncers[PING_JACK].register_state(PING_JACK_READ ? 1 : 0);

	for (unsigned i = 0; i < NUM_DEBOUNCED_DIGINS; i++) {
		digin[i].state = debouncers[i].is_high() ? 1 : 0;
		digin[i].edge = debouncers[i].is_just_pressed() ? 1 : debouncers[i].is_just_released() ? -1 : 0;
	}
}
