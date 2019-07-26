#pragma once

#include <stm32f0xx.h>


void flash_begin_open_program(void);
FLASH_Status flash_open_erase_page(uint32_t address);
FLASH_Status flash_open_program_halfword(uint8_t halfword, uint32_t address);
FLASH_Status flash_open_program_word(uint32_t word, uint32_t address);
FLASH_Status flash_open_program_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes);
FLASH_Status flash_open_program_halfword_array(uint16_t* arr, uint32_t address, uint32_t num_halfwords);
void flash_end_open_program(void);

FLASH_Status flash_erase_page(uint32_t address);
uint16_t flash_read_halfword(uint32_t address);
uint32_t flash_read_word(uint32_t address);
void flash_read_byte_array(uint8_t* arr, uint32_t address, uint32_t num_bytes);
void flash_read_halfword_array(uint16_t* arr, uint32_t address, uint32_t num_halfwords);

