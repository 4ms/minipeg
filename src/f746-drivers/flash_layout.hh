#include <stdint.h>

constexpr uint32_t NumFlashSectors = 6;

constexpr uint32_t get_sector_addr(uint32_t sector_num) {
	constexpr uint32_t F74xxEx_SECTORS[] = {
		0x08000000, //Sector 0: 32k
		0x08008000, //Sector 1: 32k
		0x08010000, //Sector 2: 32k
		0x08018000, //Sector 3: 32k
		0x08020000, //Sector 4: 128k
		0x08040000, //Sector 5: 256k
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

constexpr inline uint32_t BootloaderFlashAddr = get_sector_addr(0);	  // 32k Bootloader
constexpr inline uint32_t SettingsFlashAddr = get_sector_addr(1);	  //32k Settings
constexpr inline uint32_t AppFlashAddr = get_sector_addr(4);		  //128k app
constexpr inline uint32_t BootloaderReceiveAddr = get_sector_addr(4); //receive in same sector as app

//Note: sector size is 128kB, but we can make the firmware update wav file smaller by forcing the app max to be 64kB
constexpr inline uint32_t BootloaderReceiveSectorSize = 64 * 1024;
