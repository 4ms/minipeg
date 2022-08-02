#include <stdint.h>

constexpr uint32_t NumFlashSectors = 8;

constexpr uint32_t get_sector_addr(uint32_t sector_num) {
	constexpr uint32_t SECTORS[] = {
		0x08000000, //Sector 0: 16kB
		0x08004000, //Sector 1: 16kB
		0x08008000, //Sector 2: 16kB
		0x0800C000, //Sector 3: 16kB
		0x08010000, //Sector 4: 64kB
		0x08020000, //Sector 5: 128kB
		0x08040000, //Sector 6: 128kB
		0x08060000, //Sector 7: 128kB
		0x08080000, //end
	};
	return (sector_num <= NumFlashSectors) ? SECTORS[sector_num] : 0;
}

constexpr inline uint32_t BootloaderFlashAddr = get_sector_addr(0);	  //32kB for bootloader
constexpr inline uint32_t SettingsFlashAddr = get_sector_addr(2);	  //16kB for settings
constexpr inline uint32_t AppFlashAddr = get_sector_addr(5);		  //128kB for app
constexpr inline uint32_t BootloaderReceiveAddr = get_sector_addr(5); //Receive into same address, no point in copying

//Note: sector size is 128kB, but we can make the firmware update wav file smaller by forcing the app max to be 64kB
constexpr inline uint32_t BootloaderReceiveSectorSize = 64 * 1024;
