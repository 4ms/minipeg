#pragma once
#include "drivers/pin.hh"

struct DigIO {
	using GPIO = mdrivlib::GPIO;
	using PinNum = mdrivlib::PinNum;
	using PinMode = mdrivlib::PinMode;
	using PinPolarity = mdrivlib::PinPolarity;

	using PingBut = mdrivlib::FPin<GPIO::C, PinNum::_14, PinMode::Input, PinPolarity::Inverted>;
	using CycleBut = mdrivlib::FPin<GPIO::C, PinNum::_13, PinMode::Input, PinPolarity::Inverted>;

	using PingJack = mdrivlib::FPin<GPIO::C, PinNum::_15, PinMode::Input, PinPolarity::Normal>;
	using CycleJack = mdrivlib::FPin<GPIO::A, PinNum::_0, PinMode::Input, PinPolarity::Normal>;
	using TrigJack = mdrivlib::FPin<GPIO::B, PinNum::_13, PinMode::Input, PinPolarity::Normal>;

	using EOJack = mdrivlib::FPin<GPIO::A, PinNum::_12, PinMode::Output, PinPolarity::Normal>;
	using ClockBusOut =
		mdrivlib::FPin<GPIO::A, PinNum::_2, PinMode::Output, PinPolarity::Normal>; //Not connected on PCB!

	//SWO pin can be debug out
	using DebugOut = mdrivlib::FPin<GPIO::B, PinNum::_3, PinMode::Output, PinPolarity::Normal>;
};
