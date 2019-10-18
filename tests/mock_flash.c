#include "HAL.h"

#define MOCK_FLASH_SIZE 1024
uint32_t mockFLASH[MOCK_FLASH_SIZE];

void init_mock_flash(void) {
	for (int i=0;i<MOCK_FLASH_SIZE;i++) 
		mockFLASH[i] = 0x00000000;
}

uint32_t read_mock_flash(uint32_t i) {
	return mockFLASH[i/4];
}

HAL_StatusTypeDef  HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data) {
	if (Address > MOCK_FLASH_SIZE)
		return HAL_ERROR;

	if (Address & 0b11)
		return HAL_ERROR;

	mockFLASH[Address/4] = (uint32_t)Data;
	mockFLASH[Address/4 + 1] = (uint32_t)(Data >> 32U);

	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data) {
	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_Unlock(void) {
	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_Lock(void) {
	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_OB_Unlock(void) {
	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_OB_Lock(void) {
	return HAL_OK;
}

HAL_StatusTypeDef  HAL_FLASH_OB_Launch(void) {
	return HAL_OK;
}

uint32_t HAL_FLASH_GetError(void) {
	return 0;
}

HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError) {
	return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit) {
	return HAL_OK;
}

void HAL_FLASHEx_EnableDebugger(void) {
}

void HAL_FLASHEx_DisableDebugger(void) {
}

uint32_t HAL_FLASHEx_FlashEmptyCheck(void) {
	return 0;
}

void HAL_FLASHEx_ForceFlashEmpty(uint32_t FlashEmpty) {
}

HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit) {
	return HAL_OK;
}

void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit) {
}


