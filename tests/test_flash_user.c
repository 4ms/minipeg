#include "HAL.h"
#include "../src/flash.h"
#include "unity.h"

void init_mock_flash(void);
uint32_t read_mock_flash(uint32_t i);


void setUp()
{
	init_mock_flash();
}

void tearDown()
{
	
}

void testFlashWrites(void)
{
	HAL_StatusTypeDef status;
	TEST_ASSERT_EQUAL_INT(HAL_OK, flash_open_program_word(0x10101010,0));
	TEST_ASSERT_EQUAL_HEX(0x10101010, read_mock_flash(0));
	TEST_ASSERT_EQUAL_HEX(0xFFFFFFFF, read_mock_flash(4));

	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_word(0x10101010,1));
	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_word(0x21212121,2));
	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_word(0x31313131,3));
	TEST_ASSERT_EQUAL_INT(HAL_OK, flash_open_program_word(0x41414141,4));
	TEST_ASSERT_EQUAL_HEX(0xFFFFFFFF, read_mock_flash(0));
	TEST_ASSERT_EQUAL_HEX(0x41414141, read_mock_flash(4));

	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_doubleword(0x5454545451515151,5));
	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_doubleword(0x5454545451515151,6));
	TEST_ASSERT_EQUAL_INT(HAL_ERROR, flash_open_program_doubleword(0x5454545451515151,7));
	TEST_ASSERT_EQUAL_INT(HAL_OK, flash_open_program_doubleword(0x12345678ABCDEF01,8));
	TEST_ASSERT_EQUAL_HEX(0xABCDEF01, read_mock_flash(8));
	TEST_ASSERT_EQUAL_HEX(0x12345678, read_mock_flash(12));

	uint8_t ta[64];
	for (int i=0;i<64;i++) ta[i] = i;

	TEST_ASSERT_EQUAL_INT(HAL_OK, flash_open_program_doubleword_array((uint64_t *)ta, 0, 64/8));
	for(int i=0; i<(64/8); i++) {
		uint32_t w = ta[i*4 + 3];
		w <<= 8;
		w |= ta[i*4 + 2];
		w <<= 8;
		w |= ta[i*4 + 1];
		w <<= 8;
		w |= ta[i*4];
		TEST_ASSERT_EQUAL_HEX(w, read_mock_flash(i*4));
	}

	for (int i=0;i<64;i++) ta[i] = i+2;

	TEST_ASSERT_EQUAL_INT(HAL_OK, flash_open_program_word_array((uint32_t *)ta, 4, 64/4));
	for(int i=0; i<(64/4); i++) {
		uint32_t w = ta[i*4 + 3];
		w <<= 8;
		w |= ta[i*4 + 2];
		w <<= 8;
		w |= ta[i*4 + 1];
		w <<= 8;
		w |= ta[i*4];
		TEST_ASSERT_EQUAL_HEX(w, read_mock_flash(i*4));
	}


}