#include "dig_inout_pins.hh"
#include "stm32xx.h"

void init_dig_inouts(void) {

	DigIO::PingBut init_pingbut{mdrivlib::PinPull::Up};
	DigIO::CycleBut init_cyclebut{mdrivlib::PinPull::Up};
	DigIO::PingJack init_pingjack{mdrivlib::PinPull::Down};
	DigIO::CycleJack init_cyclejack{mdrivlib::PinPull::Down};
	DigIO::TrigJack init_trigjack{mdrivlib::PinPull::Down};
	DigIO::EOJack init_eof{mdrivlib::PinPull::None, mdrivlib::PinSpeed::Medium, mdrivlib::PinOType::PushPull};
	DigIO::ClockBusOut init_clockbus{mdrivlib::PinPull::None, mdrivlib::PinSpeed::Medium, mdrivlib::PinOType::PushPull};
}
