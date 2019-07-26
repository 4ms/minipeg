/*
 * flash.c -
 *
 * Author: Dan Green (danngreen1@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * See http://creativecommons.org/licenses/MIT/ for more information.
 *
 * -----------------------------------------------------------------------------
 */

#include "globals.h"

// const uint32_t first_sector = 	0x08000000;
// const uint32_t sector_size = 	0x00001000;
// const uint8_t  num_sectors = 	8;

const uint32_t first_page = 	0x08000000;
const uint32_t page_size = 		0x00000400;
const uint8_t  num_pages = 		32;
 

FLASH_Status flash_erase_page(uint32_t address)
{
	FLASH_Status status;

	FLASH_Unlock();
	status = FLASH_ErasePage(address);
	FLASH_Lock();
	return status;
}

FLASH_Status flash_open_erase_page(uint32_t address)
{
	FLASH_Status status;

	status = FLASH_ErasePage(address);
	return status;
}

void flash_begin_open_program(void)
{
	FLASH_Unlock();
}

FLASH_Status flash_open_program_halfword(uint8_t halfword, uint32_t address)
{
	return FLASH_ProgramHalfWord(address, halfword);
}

FLASH_Status flash_open_program_word(uint32_t word, uint32_t address)
{
	return FLASH_ProgramWord(address, word);
}

void flash_end_open_program(void)
{
	FLASH_Lock();
}


FLASH_Status flash_open_program_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes)
{
	FLASH_Status status = FLASH_COMPLETE;
	uint16_t halfword;

	if (num_bytes & 1)
		num_bytes++; //must write even number of bytes

	while(num_bytes) {
		halfword = (uint16_t)(*arr++);

		if (--num_bytes) 
		{
			halfword = (uint16_t)(*arr++);
			halfword |= ((uint16_t)(*arr++) << 8);
			num_bytes--;
		}
		else //when we have an odd number of bytes, pad the last one with the existing FLASH contents
			halfword |= flash_read_halfword(address) & 0xFF00;

		status |= FLASH_ProgramHalfWord(address, halfword);
		address++;
	}
	return status;
}

FLASH_Status flash_open_program_halfword_array(uint16_t* arr, uint32_t address, uint32_t num_halfwords)
{
	FLASH_Status status = FLASH_COMPLETE;

	while(num_halfwords--) {
		status |= FLASH_ProgramHalfWord(address, *arr++);
		address+=2;
	}
	return status;
}


void flash_read_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes)
{
	while(num_bytes--) {
		*arr++ = (uint8_t)(*(__IO uint16_t*)address);
		address++;
	}
}

void flash_read_halfword_array(uint16_t* arr, uint32_t address, uint32_t num_halfwords)
{
	while(num_halfwords--) {
		*arr++ = (uint16_t)(*(__IO uint16_t*)address);
		address+=2;
	}
}


uint32_t flash_read_word(uint32_t address)
{
    return( *(__IO uint16_t*)address);
}

uint16_t flash_read_halfword(uint32_t address)
{
    return((uint16_t) (*(__IO uint16_t*)address));
}

