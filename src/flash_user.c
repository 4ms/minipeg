// char EOF_IS_TAPCLKOUT=0;
// char NO_FREERUNNING_PING=0;
// char EOR_IS_HALFRISE=0;
// char EO1_IS_TRIG=0;
// char EO2_IS_TRIG=0;
// char LIMIT_SKEW=0;
// char ROLLOFF_PING=1;
// char ASYNC_CAN_SUSTAIN=1;
#include "globals.h"

#define IS_CALIBRATED_ADDR 0x8000000
#define USER_FLASH_PAGE 0x08007C00

void write_calibration(uint8_t num_words, uint32_t *data)
{
	FLASH_Status status;
	uint32_t addr = USER_FLASH_PAGE;

	FLASH_Unlock();
	status = FLASH_ErasePage(addr);

	while (num_words--)
	{
		status = FLASH_ProgramWord(addr, *data++);
	}
	FLASH_Lock();
}

void read_calibration(uint8_t num_bytes, uint8_t *data)
{
	uint16_t midpt_array[19];

	midpt_array[0] = 68;
	midpt_array[1] = 262;
	midpt_array[2] = 509;
	midpt_array[3] = 743;
	midpt_array[4] = 973;
	midpt_array[5] = 1202;
	midpt_array[6] = 1427;
	midpt_array[7] = 1657;
	midpt_array[8] = 1882;
	midpt_array[9] = 2107;
	midpt_array[10] = 2341;
	midpt_array[11] = 2574;
	midpt_array[12] = 2802;
	midpt_array[13] = 3026;
	midpt_array[14] = 3262;
	midpt_array[15] = 3500;
	midpt_array[16] = 3734;
	midpt_array[17] = 3942;
	midpt_array[18] = 4095;

	for (uint8_t i=0;i<num_bytes;i++)
		data[i] = ((uint8_t *)midpt_array)[i];
}

uint8_t is_calibrated(void)
{
	return 1;
}