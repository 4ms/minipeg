/*
 * adc_builtin_driver.c - adc driver for built-in adcs
 * Uses DMA to dump ADC values into buffers
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

#include "drivers/rcc.hh"
#include "globals.h"
#include "system.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

static uint32_t HAL_RCC_ADC12_CLK_ENABLED = 0;

extern "C" void ADC_Init(ADC_TypeDef *ADCx,
						 uint16_t *adc_buffer,
						 uint32_t num_channels,
						 builtinAdcSetup *adc_setup,
						 uint32_t oversample_ratio) {
	GPIO_InitTypeDef gpio;
	HAL_StatusTypeDef err;
	uint8_t i;

	ADC_HandleTypeDef *hadc = ADCx == ADC1 ? &hadc1 : nullptr;
	if (!hadc)
		return;

	__HAL_RCC_DMA2_CLK_ENABLE();

	for (i = 0; i < num_channels; i++) {
		gpio.Pin = adc_setup[i].pin;
		gpio.Mode = GPIO_MODE_ANALOG;
		gpio.Pull = GPIO_NOPULL;
		mdrivlib::RCC_Enable::GPIO::enable(adc_setup[i].gpio);
		HAL_GPIO_Init(adc_setup[i].gpio, &gpio);
	}

	hadc->Instance = ADCx;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc->Init.ScanConvMode = ENABLE;
	hadc->Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc->Init.ContinuousConvMode = ENABLE;
	hadc->Init.NbrOfConversion = num_channels;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.NbrOfDiscConversion = 0;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.DMAContinuousRequests = ENABLE;
	err = HAL_ADC_Init(hadc);

	ADC_ChannelConfTypeDef sConfig;
	for (i = 0; i < num_channels; i++) {
		sConfig.Channel = adc_setup[i].channel;
		sConfig.Rank = i & 15;
		sConfig.SamplingTime = adc_setup[i].sample_time;
		err = HAL_ADC_ConfigChannel(hadc, &sConfig);
	}

	err = HAL_ADC_Start_DMA(hadc, (uint32_t *)adc_buffer, num_channels);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {
	if (adcHandle->Instance == ADC1) {
		__HAL_RCC_ADC1_CLK_ENABLE();

		hdma_adc1.Instance = DMA2_Stream0;
		hdma_adc1.Init.Channel = DMA_CHANNEL_0;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
		hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma_adc1);

		__HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);
	}
}

// void DMA2_Stream0_IRQHandler(void) {
// 	HAL_DMA_IRQHandler(&hdma_adc1);
// }

// void DMA2_Stream2_IRQHandler(void) {
// 	HAL_DMA_IRQHandler(&hdma_adc2);
// }