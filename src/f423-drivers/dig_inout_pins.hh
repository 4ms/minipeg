#pragma once
#include "drivers/pin.hh"

struct DigIO {
	using GPIO = mdrivlib::GPIO;
	using PinNum = mdrivlib::PinNum;
	using PinMode = mdrivlib::PinMode;
	using PinPolarity = mdrivlib::PinPolarity;

	using PingBut = mdrivlib::FPin<GPIO::E, PinNum::_1, PinMode::Input, PinPolarity::Inverted>;
	using CycleBut = mdrivlib::FPin<GPIO::C, PinNum::_14, PinMode::Input, PinPolarity::Inverted>;

	using PingJack = mdrivlib::FPin<GPIO::E, PinNum::_3, PinMode::Input, PinPolarity::Normal>;
	using CycleJack = mdrivlib::FPin<GPIO::E, PinNum::_4, PinMode::Input, PinPolarity::Normal>;
	using TrigJack = mdrivlib::FPin<GPIO::D, PinNum::_5, PinMode::Input, PinPolarity::Normal>;

	using EOJack = mdrivlib::FPin<GPIO::D, PinNum::_7, PinMode::Output, PinPolarity::Normal>;
	using ClockBusOut = mdrivlib::FPin<GPIO::A, PinNum::_12, PinMode::Output, PinPolarity::Normal>;

	//SWO pin can be debug out
	using DebugOut = mdrivlib::FPin<GPIO::B, PinNum::_3, PinMode::Output, PinPolarity::Normal>;
};
