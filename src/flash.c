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

#include <globals.h>

const uint32_t first_page = 	0x08000000;
const uint32_t page_size = 		0x00000800; //2k
const uint8_t  num_pages = 		64;
 
HAL_StatusTypeDef _flash_erase(uint32_t address)
{
	HAL_StatusTypeDef status;
	uint32_t page = (address-first_page)/page_size;
	FLASH_EraseInitTypeDef eraseInit;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	eraseInit.Page = page;
	eraseInit.NbPages = 1;
	status = HAL_FLASHEx_Erase(&eraseInit, &page);
	if (page!=0xFFFFFFFF)
		return HAL_ERROR;
	else
		return status;
}

HAL_StatusTypeDef flash_erase_page(uint32_t address)
{
	HAL_StatusTypeDef status;

	HAL_FLASH_Unlock();
	status = _flash_erase(address);
	HAL_FLASH_Lock();
	return status;
}

HAL_StatusTypeDef flash_open_erase_page(uint32_t address)
{
	return _flash_erase(address);
}

void flash_begin_open_program(void)
{
	HAL_FLASH_Unlock();
}

HAL_StatusTypeDef flash_open_program_halfword(uint8_t halfword, uint32_t address)
{
	// return FLASH_ProgramHalfWord(address, halfword);
	return HAL_ERROR; // Not for STM32G0xx
}

HAL_StatusTypeDef flash_open_program_doubleword(uint64_t doubleword, uint32_t address)
{
	if (address & 0b111)
		return HAL_ERROR; //address must be double-word aligned

	return HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, doubleword);
}

HAL_StatusTypeDef flash_open_program_word(uint32_t word, uint32_t address)
{
	uint64_t padded_word;
	//address &= ~0b11; //align to word

	if (address & 0b100) {
		padded_word = (uint64_t)word;
		padded_word <<= 32;
		padded_word |= 0x00000000FFFFFFFF;
		// padded_word = ((uint64_t)word << 32) | 0x00000000FFFFFFFF;
		address -= 4;
	}
	else
		padded_word = (0xFFFFFFFF00000000) | ((uint64_t)word);	

	// printf("requested: %08x. writing: %016x at %i\n", word, padded_word, address);

	return HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, padded_word);
}

void flash_end_open_program(void)
{
	HAL_FLASH_Lock();
}


HAL_StatusTypeDef flash_open_program_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes)
{
	HAL_StatusTypeDef status = HAL_OK;
	// uint64_t doubleword;


	// while(num_bytes) {
	// 	doubleword = (uint16_t)(*arr++);

	// 	if (--num_bytes) 
	// 	{
	// 		halfword = (uint16_t)(*arr++);
	// 		halfword |= ((uint16_t)(*arr++) << 8);
	// 		num_bytes--;
	// 	}
	// 	else //when we have an odd number of bytes, pad the last one with the existing FLASH contents
	// 		halfword |= flash_read_halfword(address) & 0xFF00;

	// 	status |= flash_open_program_halfword(halfword, address);
	// 	address++;
	// }
	return status;
}

HAL_StatusTypeDef flash_open_program_doubleword_array(uint64_t* arr, uint32_t address, uint32_t num_doublewords)
{
	HAL_StatusTypeDef status = HAL_OK;

	if (!num_doublewords) 
		return HAL_ERROR;

	while(num_doublewords--) {
		status |= flash_open_program_doubleword(*arr++, address);
		address+=8;
	}
	return status;
}

HAL_StatusTypeDef flash_open_program_word_array(uint32_t* arr, uint32_t address, uint32_t num_words)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint64_t doubleword;

	if (address & 0b11)
		return HAL_ERROR; //address must be word-aligned

	if (!num_words)
		return HAL_ERROR;

	//First word not doubleword aligned
	if (address & 0b100) {
		// printf("not doublword aligned: %x at %i\n", *arr, address);
		flash_open_program_word((uint64_t)(*arr++), address);
		address+=4;
		num_words--;
	}

	while(num_words)
	{
		if (--num_words) 
		{
			doubleword = (uint64_t)(*arr++);
			doubleword |= (uint64_t)(*arr++) << 32;
			num_words--;
			status |= flash_open_program_doubleword(doubleword, address);
			address+=8;
		}
		else //Last word not doubleword aligned
			flash_open_program_word((uint64_t)(*arr++), address);
	}
	return status;
}


void flash_read_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes)
{
	while(num_bytes--) {
		*arr++ = (uint8_t)(*(__IO uint8_t*)address);
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

void flash_read_word_array(uint32_t* arr, uint32_t address, uint32_t num_words)
{
	while(num_words--) {
		*arr++ = (uint32_t)(*(__IO uint32_t*)address);
		address+=4;
	}
}

void flash_read_doubleword_array(uint64_t* arr, uint32_t address, uint32_t num_doublewords)
{
	while(num_doublewords--) {
		*arr++ = (uint64_t)(*(__IO uint64_t*)address);
		address+=8;
	}
}


uint32_t flash_read_word(uint32_t address)
{
    return( *(__IO uint32_t*)address);
}

uint16_t flash_read_halfword(uint32_t address)
{
    return((uint16_t) (*(__IO uint16_t*)address));
}

