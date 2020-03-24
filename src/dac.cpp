#include "dac.h"
#include <stm32g4xx.h>

DAC_HandleTypeDef hdac1;
DAC_HandleTypeDef hdac3;
DMA_HandleTypeDef hdma_dac1_ch1;

void dac_out(enum DACs dac, uint16_t val) {

}

void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_dac1_ch1);
}
