#include "system.h"
#include <stm32g4xx.h>
#include <stm32g4xx_ll_bus.h>
#include <stm32g4xx_ll_pwr.h>

static void SetVectorTable(uint32_t reset_address);
static void SystemClock_Config(void);

void Error_Handler(void) {
	//
	__BKPT();
}

static void set_option_bytes_ignore_BOOT0_pin();

void system_init(void) {
	SetVectorTable(0x08000000);
	HAL_Init();

	SystemClock_Config();

	//set_option_bytes_ignore_BOOT0_pin();

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	__HAL_DBGMCU_FREEZE_TIM7();
}

void HAL_MspInit(void) {
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_SYSCFG_DisableVREFBUF();
	HAL_SYSCFG_VREFBUF_HighImpedanceConfig(SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE);
	LL_PWR_DisableDeadBatteryPD();
}

static void set_option_bytes_ignore_BOOT0_pin() {
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	HAL_FLASH_OB_Unlock();

	FLASH_OBProgramInitTypeDef OBInit;

	OBInit.OptionType = OPTIONBYTE_USER;
	OBInit.USERType = OB_USER_nBOOT0 | OB_USER_nSWBOOT0;
	OBInit.USERConfig = OB_BOOT0_FROM_OB | OB_nBOOT0_SET;
	HAL_FLASHEx_OBProgram(&OBInit);
	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();
}

void enable_gpio_rcc(GPIO_TypeDef *port) {
	if (port == (GPIO_TypeDef *)(0x00000000))
		return;
#ifdef GPIOA
	else if (port == GPIOA && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOA))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
#endif
#ifdef GPIOB
	else if (port == GPIOB && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOB))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
#endif
#ifdef GPIOC
	else if (port == GPIOC && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOC))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
#endif
#ifdef GPIOD
	else if (port == GPIOD && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOD))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
#endif
#ifdef GPIOE
	else if (port == GPIOE && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOE))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
#endif
#ifdef GPIOF
	else if (port == GPIOF && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOF))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
#endif
#ifdef GPIOG
	else if (port == GPIOG && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOG))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG);
#endif
#ifdef GPIOH
	else if (port == GPIOH && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOH))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
#endif
#ifdef GPIOI
	else if (port == GPIOI && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOI))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOI);
#endif
#ifdef GPIOJ
	else if (port == GPIOJ && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOJ))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOJ);
#endif
#ifdef GPIOK
	else if (port == GPIOK && !LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOK))
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOK);
#endif
}

void enable_adc_rcc(ADC_TypeDef *ADCx) {
	if (ADCx == (ADC_TypeDef *)(0x00000000))
		return;
#ifdef ADC1
	if (ADCx == ADC1)
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
#endif
#ifdef ADC2
	else if (ADCx == ADC2)
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
#endif
#ifdef ADC3
	else if (ADCx == ADC3)
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
#endif
#ifdef ADC4
	else if (ADCx == ADC4)
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
#endif
#ifdef ADC5
	else if (ADCx == ADC5)
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
#endif
}

void enable_dma_rcc(DMA_TypeDef *DMAx) {
	if (DMAx == (DMA_TypeDef *)(0x00000000))
		return;
#ifdef DMA1
	else if (DMAx == DMA1)
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
#endif
#ifdef DMA2
	else if (DMAx == DMA2)
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
#endif
#ifdef DMA3
	else if (DMAx == DMA3)
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA3);
#endif
}

void enable_tim_rcc(TIM_TypeDef *TIM) {
#ifdef TIM1
	if (TIM == TIM1)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
#endif
#ifdef TIM2
	if (TIM == TIM2)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
#endif
#ifdef TIM3
	if (TIM == TIM3)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
#endif
#ifdef TIM4
	if (TIM == TIM4)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
#endif
#ifdef TIM5
	if (TIM == TIM5)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
#endif
#ifdef TIM6
	if (TIM == TIM6)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
#endif
#ifdef TIM7
	if (TIM == TIM7)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
#endif
#ifdef TIM8
	if (TIM == TIM8)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);
#endif
#ifdef TIM9
	if (TIM == TIM9)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
#endif
#ifdef TIM10
	if (TIM == TIM10)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM10);
#endif
#ifdef TIM11
	if (TIM == TIM11)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM11);
#endif
#ifdef TIM12
	if (TIM == TIM12)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM12);
#endif
#ifdef TIM13
	if (TIM == TIM13)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM13);
#endif
#ifdef TIM14
	if (TIM == TIM14)
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
#endif
#ifdef TIM15
	if (TIM == TIM15)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
#endif
#ifdef TIM16
	if (TIM == TIM16)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
#endif
#ifdef TIM17
	if (TIM == TIM17)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
#endif
#ifdef TIM20
	if (TIM == TIM20)
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM20);
#endif
}

static uint8_t kTimPeriphMax = 20;

uint8_t tim_periph_to_num(TIM_TypeDef *TIM) {
	if (TIM == (TIM_TypeDef *)(0x00000000))
		return 0;
#ifdef TIM1
	else if (TIM == TIM1)
		return 1;
#endif
#ifdef TIM2
	else if (TIM == TIM2)
		return 2;
#endif
#ifdef TIM3
	else if (TIM == TIM3)
		return 3;
#endif
#ifdef TIM4
	else if (TIM == TIM4)
		return 4;
#endif
#ifdef TIM5
	else if (TIM == TIM5)
		return 5;
#endif
#ifdef TIM6
	else if (TIM == TIM6)
		return 6;
#endif
#ifdef TIM7
	else if (TIM == TIM7)
		return 7;
#endif
#ifdef TIM8
	else if (TIM == TIM8)
		return 8;
#endif
#ifdef TIM9
	else if (TIM == TIM9)
		return 9;
#endif
#ifdef TIM10
	else if (TIM == TIM10)
		return 10;
#endif
#ifdef TIM11
	else if (TIM == TIM11)
		return 11;
#endif
#ifdef TIM12
	else if (TIM == TIM12)
		return 12;
#endif
#ifdef TIM13
	else if (TIM == TIM13)
		return 13;
#endif
#ifdef TIM14
	else if (TIM == TIM14)
		return 14;
#endif
#ifdef TIM15
	else if (TIM == TIM15)
		return 15;
#endif
#ifdef TIM16
	else if (TIM == TIM16)
		return 16;
#endif
#ifdef TIM17
	else if (TIM == TIM17)
		return 17;
#endif
#ifdef TIM18
	else if (TIM == TIM18)
		return 18;
#endif
#ifdef TIM19
	else if (TIM == TIM19)
		return 19;
#endif
#ifdef TIM20
	else if (TIM == TIM20)
		return 20;
#endif
	else
		return 0;
}

static void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
	RCC_OscInitStruct.PLL.PLLN = 42;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
	PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}

static void SetVectorTable(uint32_t reset_address) {
	SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
}
