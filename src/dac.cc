#include "dac.h"
extern "C" {
#include "timekeeper.h"
}
#include <stm32g4xx.h>

DAC_HandleTypeDef hdac1;
TIM_HandleTypeDef htim6;

#define ENVA_Pin GPIO_PIN_4
#define ENVB_Pin GPIO_PIN_5
#define ENV_GPIO_Port GPIOA
#define DAC_UPDATE_TIMER_NUM 6

static void init_dac_update_tmr(uint32_t freq);

void dac_out(enum DACs dac, uint16_t val) {
	if (dac==DAC_ENVA) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 4095-val);
	}
	else if (dac==DAC_ENVB) {
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 4095-val);
	}
}

void init_dac(uint32_t freq) {
	hdac1.Instance = DAC1;
	HAL_DAC_Init(&hdac1);

	DAC_ChannelConfTypeDef sConfig = {0};
	sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE;//_AUTOMATIC
	sConfig.DAC_DMADoubleDataMode = DISABLE;
	sConfig.DAC_SignedFormat = DISABLE;
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_Trigger2 = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;

	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1);

	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2);

	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

	init_dac_update_tmr(freq);
}

static void init_dac_update_tmr(uint32_t freq) {
	TimerITInitStruct dac_update_timer_config;

	uint32_t sysclockfreq = HAL_RCC_GetSysClockFreq() * 2;
	dac_update_timer_config.priority1 = 0;
	dac_update_timer_config.priority2 = 1;
	dac_update_timer_config.period = sysclockfreq / freq;// 168MHZ / 21kHz = 8000
	dac_update_timer_config.prescaler = 0;
	dac_update_timer_config.clock_division = 0;

	init_timer_IRQ(DAC_UPDATE_TIMER_NUM, &dac_update_timer_config);
}

void assign_dac_update_callback(void (*callbackfunc)(void)) {
	start_timer_IRQ(DAC_UPDATE_TIMER_NUM, reinterpret_cast<void *>(callbackfunc));
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac) {

  GPIO_InitTypeDef gpio = {0};

  if (hdac->Instance==DAC1) {
    __HAL_RCC_DAC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio.Pin = ENVA_Pin | ENVB_Pin;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ENV_GPIO_Port, &gpio);
  }
}

