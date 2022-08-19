#include <cstdint>
namespace BootloaderConf
{
static constexpr bool UseGateInThreshold = false;
static constexpr uint32_t SampleRate = 22050;

//Note: sector size is 128kB, but we can make the firmware update wav file smaller by forcing the app max to be 64kB
static constexpr uint32_t ReceiveSectorSize = 64 * 1024;

struct FskEncoding {
	uint32_t blank;
	uint32_t one;
	uint32_t zero;
};
static constexpr FskEncoding Encoding{16, 6, 3};

}; // namespace BootloaderConf
