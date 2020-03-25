#include "dac.h"
#include <stm32g4xx.h>

DAC_HandleTypeDef hdac1;
DAC_HandleTypeDef hdac3;
DMA_HandleTypeDef hdma_dac1_ch1;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim6;


#define ENVA_Pin GPIO_PIN_4
#define ENVB_Pin GPIO_PIN_5
#define ENV_GPIO_Port GPIOA

const uint16_t sinewave[60] = {
0x07ff,0x08cb,0x0994,0x0a5a,0x0b18,0x0bce,0x0c79,0x0d18,0x0da8,0x0e29,0x0e98,0x0ef4,0x0f3e,0x0f72,0x0f92,0x0f9d,
0x0f92,0x0f72,0x0f3e,0x0ef4,0x0e98,0x0e29,0x0da8,0x0d18,0x0c79,0x0bce,0x0b18,0x0a5a,0x0994,0x08cb,0x07ff,0x0733,
0x066a,0x05a4,0x04e6,0x0430,0x0385,0x02e6,0x0256,0x01d5,0x0166,0x010a,0x00c0,0x008c,0x006c,0x0061,0x006c,0x008c,
0x00c0,0x010a,0x0166,0x01d5,0x0256,0x02e6,0x0385,0x0430,0x04e6,0x05a4,0x066a,0x0733};

void dac_out(enum DACs dac, uint16_t val) {

}

static void init_dac_trigtmr() {
	__HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_TIM7_CLK_ENABLE();

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig7 = {0};

	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 149;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 999;
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim7);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim7, &sClockSourceConfig);

	sMasterConfig7.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig7.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig7);

	TIM_MasterConfigTypeDef sMasterConfig6 = {0};

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 0;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 2499;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim6);

	sMasterConfig6.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig6.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig6);

	HAL_TIM_Base_Start(&htim7);
	HAL_TIM_Base_Start(&htim6);
}

void init_dac() {
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	DAC_ChannelConfTypeDef sConfig = {0};

	hdac1.Instance = DAC1;
	HAL_DAC_Init(&hdac1);

	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE;//_AUTOMATIC
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_T7_TRGO;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_T6_TRGO;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;

	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1);

	// sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	// HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2);

	init_dac_trigtmr();

	HAL_DACEx_SawtoothWaveGenerate(&hdac1, DAC_CHANNEL_1, DAC_SAWTOOTH_POLARITY_INCREMENT, 0, 0x10000/60UL);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

	// HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)sinewave, 60, DAC_ALIGN_12B_R);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hdac->Instance==DAC1) {
    __HAL_RCC_DAC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = ENVA_Pin | ENVB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ENV_GPIO_Port, &GPIO_InitStruct);

    hdma_dac1_ch1.Instance = DMA1_Channel1;
    hdma_dac1_ch1.Init.Request = DMA_REQUEST_DAC1_CHANNEL1;
    hdma_dac1_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_dac1_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dac1_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dac1_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dac1_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_dac1_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_dac1_ch1.Init.Priority = DMA_PRIORITY_LOW;
    HAL_DMA_Init(&hdma_dac1_ch1);

    __HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac1_ch1);
  }
  else if(hdac->Instance==DAC3)
  {
    __HAL_RCC_DAC3_CLK_ENABLE();
  }

}

extern "C" void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_dac1_ch1);
}

