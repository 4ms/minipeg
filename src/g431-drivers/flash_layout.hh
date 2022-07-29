#include <cstdint>

constexpr uint32_t NumFlashSectors = 64;

constexpr uint32_t get_sector_addr(uint32_t sector_num) {
	if (sector_num <= NumFlashSectors)
		return 0x08000000 + sector_num * 0x800;
	else
		return 0; //error
}

constexpr inline uint32_t BootloaderFlashAddr = get_sector_addr(0);
constexpr inline uint32_t SettingsFlashAddr = get_sector_addr(7); //14k for bootloader
constexpr inline uint32_t AppFlashAddr = get_sector_addr(8);	  //14k for BL + 2k for settings
constexpr inline uint32_t BootloaderReceiveAddr =
	get_sector_addr(36); //14k BL + 2k Settings + 56k for app = 72k, leaves 56k for receiving

static_assert(SettingsFlashAddr - BootloaderFlashAddr == 14 * 1024);				  //bootloader
static_assert(AppFlashAddr - SettingsFlashAddr == 2 * 1024);						  //Settings
static_assert(BootloaderReceiveAddr - AppFlashAddr == 56 * 1024);					  //App
static_assert(get_sector_addr(NumFlashSectors) - BootloaderReceiveAddr == 56 * 1024); //receiving firmware
static_assert(get_sector_addr(NumFlashSectors + 1) == 0);
