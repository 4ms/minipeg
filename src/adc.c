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
 
#include "globals.h"

ADC_HandleTypeDef hadc;

void ADC_Init(uint16_t *adc_buffer, uint32_t num_channels, builtinAdcSetup *adc_setup)
{
	ADC_ChannelConfTypeDef 	sConfig;
	GPIO_InitTypeDef 		gpio;
	uint8_t i;

	__HAL_RCC_DMA1_CLK_ENABLE();

	//Set GPIO pins to analog
	for (i=0; i<num_channels; i++)
	{
	    gpio.Pin = adc_setup[i].pin;
	    gpio.Mode = GPIO_MODE_ANALOG;
	    gpio.Pull = GPIO_NOPULL;
	    HAL_GPIO_Init(adc_setup[i].gpio, &gpio);
	}

	//Initialize ADC1 peripheral
	hadc.Instance 						= ADC1;
	hadc.Init.ClockPrescaler 			= ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc.Init.Resolution 				= ADC_RESOLUTION_12B;
	hadc.Init.ScanConvMode 				= ENABLE;
	hadc.Init.ContinuousConvMode 		= ENABLE;
	hadc.Init.DiscontinuousConvMode 	= DISABLE;
	hadc.Init.ExternalTrigConvEdge 		= ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv 			= ADC_SOFTWARE_START;//ADC_EXTERNALTRIGCONV_T1_CC1;  //Todo: why did T1_CC1 work on F7?
	hadc.Init.DataAlign 				= ADC_DATAALIGN_RIGHT;
	hadc.Init.NbrOfConversion 			= num_channels;
	hadc.Init.DMAContinuousRequests 	= ENABLE;//DISABLE;
	hadc.Init.EOCSelection 				= ADC_EOC_SEQ_CONV;//ADC_EOC_SINGLE_CONV;

  	hadc.Init.LowPowerAutoWait 			= DISABLE;
	hadc.Init.LowPowerAutoPowerOff 		= DISABLE;
	hadc.Init.Overrun 					= ADC_OVR_DATA_PRESERVED; //Todo: what is this?
	hadc.Init.SamplingTimeCommon1 		= ADC_SAMPLETIME_160CYCLES_5;
	hadc.Init.SamplingTimeCommon2 		= ADC_SAMPLETIME_79CYCLES_5;
	hadc.Init.OversamplingMode 			= DISABLE;  //Todo: what is this?
	hadc.Init.TriggerFrequencyMode 		= ADC_TRIGGER_FREQ_HIGH; 

	HAL_ADC_Init(&hadc);

	for (i=0; i<num_channels; i++)
	{
		sConfig.Channel 		= adc_setup[i].channel;
		sConfig.Rank 			= ADC_REGULAR_RANK_1 + i;
		sConfig.SamplingTime	= adc_setup[i].sample_time;
		HAL_ADC_ConfigChannel(&hadc, &sConfig);
	}

	//__HAL_ADC_DISABLE_IT(&hadc1, (ADC_IT_EOC | ADC_IT_OVR));

	HAL_ADC_Start(&hadc);
	HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_buffer, num_channels);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	if(adcHandle->Instance==ADC1)
	{
		__HAL_RCC_ADC1_CLK_ENABLE();

		hdma_adc.Instance 					= DMA1_Stream4;
		hdma_adc.Init.Channel 				= DMA_CHANNEL_0;
		hdma_adc.Init.Direction 			= DMA_PERIPH_TO_MEMORY;
		hdma_adc.Init.PeriphInc 			= DMA_PINC_DISABLE;
		hdma_adc.Init.MemInc 				= DMA_MINC_ENABLE;
		hdma_adc.Init.PeriphDataAlignment 	= DMA_PDATAALIGN_HALFWORD;
		hdma_adc.Init.MemDataAlignment 	= DMA_MDATAALIGN_HALFWORD;
		hdma_adc.Init.Mode 				= DMA_CIRCULAR;
		hdma_adc.Init.Priority 			= DMA_PRIORITY_MEDIUM;
		hdma_adc.Init.FIFOMode 			= DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
			_Error_Handler(__FILE__, __LINE__);

		__HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc);
	}
}

void ADC_Init(uint16_t *adc_buffer, uint32_t num_channels, builtinAdcSetup *adc_setup)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA2 stream4 channel0 configuration ----------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = adc_buffer_size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 Init ------------------------------------------------------------*/
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* Configure analog input pins ------------------------------------------*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_71_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_71_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_71_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_71_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_4, ADC_SampleTime_71_5Cycles);
	ADC_ChannelConfig(ADC1, ADC_Channel_5, ADC_SampleTime_71_5Cycles);
	
	//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
//	ADC_ContinuousModeCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_StartOfConversion(ADC1);
}
