#include <stdint.h>

constexpr uint32_t F74xxEx_SECTORS[] = {
	0x08000000,
	0x08008000,
	0x08010000,
	0x08018000,
	0x08020000,
	0x08040000,
	0,
};
constexpr uint32_t F74xxGx_SECTORS[] = {
	0x08000000,
	0x08008000,
	0x08010000,
	0x08018000,
	0x08020000,
	0x08040000,
	0x08080000,
	0x080C0000,
	0,
};

const uint32_t *const SECTORS = F74xxEx_SECTORS;

constexpr inline uint32_t SettingsFlashAddr = 0x08010000;
