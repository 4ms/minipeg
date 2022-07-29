#include "dig_inout_pins.hh"
#include "drivers/pin.hh"

static inline void init_buttons() {
	DigIO::PingBut pingbutinit;
	DigIO::CycleBut cyclebutinit;
}

enum class Button { Ping, Cycle };

static inline bool button1_pushed() {
	return DigIO::PingBut::read();
}

static inline bool button2_pushed() {
	return DigIO::CycleBut::read();
}

static inline bool button_pushed(Button button) {
	if (button == Button::Ping)
		return button1_pushed();
	else
		return button2_pushed();
}
