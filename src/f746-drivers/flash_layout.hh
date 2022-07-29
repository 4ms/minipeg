#include <stdint.h>

constexpr uint32_t NumFlashSectors = 6;

constexpr uint32_t get_sector_addr(uint32_t sector_num) {
	constexpr uint32_t F74xxEx_SECTORS[] = {
		0x08000000,
		0x08008000,
		0x08010000,
		0x08018000,
		0x08020000,
		0x08040000,
		0x08080000, //end
	};
	// constexpr uint32_t F74xxGx_SECTORS[] = {
	// 	0x08000000,
	// 	0x08008000,
	// 	0x08010000,
	// 	0x08018000,
	// 	0x08020000,
	// 	0x08040000,
	// 	0x08080000,
	// 	0x080C0000,
	// 	0x08100000,
	// };
	return (sector_num <= NumFlashSectors) ? F74xxEx_SECTORS[sector_num] : 0;
}

constexpr inline uint32_t BootloaderFlashAddr = get_sector_addr(0);
constexpr inline uint32_t SettingsFlashAddr = get_sector_addr(1);
constexpr inline uint32_t AppFlashAddr = get_sector_addr(2);
constexpr inline uint32_t BootloaderReceiveAddr = get_sector_addr(5);

static_assert(SettingsFlashAddr - BootloaderFlashAddr == 32 * 1024);				   //bootloader
static_assert(AppFlashAddr - SettingsFlashAddr == 32 * 1024);						   //Settings
static_assert(BootloaderReceiveAddr - AppFlashAddr == 192 * 1024);					   //Appl
static_assert(get_sector_addr(NumFlashSectors) - BootloaderReceiveAddr == 256 * 1024); //receiving firmware
static_assert(get_sector_addr(NumFlashSectors + 1) == 0);
