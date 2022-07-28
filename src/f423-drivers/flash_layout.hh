#include <stdint.h>

constexpr uint32_t SECTORS[] = {
	0x08000000,
	0x08004000,
	0x08008000,
	0x0800C000,
	0x08010000,
	0x08020000,
	0x08040000,
	0x08060000,
	0,
};

constexpr inline uint32_t SettingsFlashAddr = 0x08020000;
