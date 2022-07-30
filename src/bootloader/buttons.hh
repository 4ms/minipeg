#include "dig_inout_pins.hh"
#include "drivers/pin.hh"

static inline void init_buttons() {
	DigIO::PingBut pingbutinit(mdrivlib::PinPull::Up);
	DigIO::CycleBut cyclebutinit(mdrivlib::PinPull::Up);
}

enum class Button { Ping, Cycle };

static inline bool button_pushed(Button button) {
	if (button == Button::Ping)
		return DigIO::PingBut::read();
	else
		return DigIO::CycleBut::read();
}
