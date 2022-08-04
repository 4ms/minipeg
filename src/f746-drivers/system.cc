#include "drivers/system.hh"
#include "flash_layout.hh"
#include "settings.h"
#include "stm32xx.h"

//stub for adapting to g431 C code
void init_timekeeper() {
}

void system_init() {
	mdrivlib::System::SetVectorTable(AppFlashAddr);
	HAL_Init();

	RCC_OscInitTypeDef RCC_OscInitStruct = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState = RCC_HSE_ON,
		.PLL =
			{
				.PLLState = RCC_PLL_ON,
				.PLLSource = RCC_PLLSOURCE_HSE,
				.PLLM = 8,
				.PLLN = 216,
				.PLLP = RCC_PLLP_DIV2,
				.PLLQ = 2,
			},
	};

	RCC_ClkInitTypeDef RCC_ClkInitStruct = {
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV4,
		.APB2CLKDivider = RCC_HCLK_DIV4, //Max is DIV2, but ADC reads too fast and bleeds with 1k impedance
	};
	RCC_PeriphCLKInitTypeDef noperiphclk{0};
	mdrivlib::SystemClocks::init_clocks(RCC_OscInitStruct, RCC_ClkInitStruct, noperiphclk, TICKS_PER_MS * 1000);

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	HAL_MPU_Disable();

	SCB_InvalidateDCache();
	SCB_DisableDCache();
	SCB_EnableICache();

	__HAL_DBGMCU_FREEZE_TIM7();
}

//TODO: check this is right! From F730
void reset_buses() {
	RCC->APB1RSTR = 0xFFFFFFFFU;
	RCC->APB1RSTR = 0x00U;

	RCC->APB2RSTR = 0xFFFFFFFFU;
	RCC->APB2RSTR = 0x00U;

	RCC->AHB1RSTR = 0xFFFFFFFFU;
	RCC->AHB1RSTR = 0x00U;

	RCC->AHB2RSTR = 0xFFFFFFFFU;
	RCC->AHB2RSTR = 0x00U;

	RCC->AHB3RSTR = 0xFFFFFFFFU;
	RCC->AHB3RSTR = 0x00U;
}

//TODO: check this is right! From F730
void reset_RCC() {
	SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk |
					   SysTick_CTRL_ENABLE_Msk); /* Disable SysTick IRQ and SysTick Timer */

	/* Set HSION bit to the reset value */
	SET_BIT(RCC->CR, RCC_CR_HSION);

	/* Wait till HSI is ready */
	while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET) {
		;
	}

	/* Set HSITRIM[4:0] bits to the reset value */
	SET_BIT(RCC->CR, RCC_CR_HSITRIM_4);

	/* Reset CFGR register */
	CLEAR_REG(RCC->CFGR);

	/* Wait till clock switch is ready */
	while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {
		;
	}

	/* Clear HSEON, HSEBYP and CSSON bits */
	CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

	/* Wait till HSE is disabled */
	while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {
		;
	}

	/* Clear PLLON bit */
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

	/* Wait till PLL is disabled */
	while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {
		;
	}

	/* Reset PLLI2SON bit */
	CLEAR_BIT(RCC->CR, RCC_CR_PLLI2SON);

	/* Wait till PLLI2S is disabled */
	while (READ_BIT(RCC->CR, RCC_CR_PLLI2SRDY) != RESET) {
		;
	}

	/* Reset PLLSAI bit */
	CLEAR_BIT(RCC->CR, RCC_CR_PLLSAION);

	/* Wait till PLLSAI is disabled */
	while (READ_BIT(RCC->CR, RCC_CR_PLLSAIRDY) != RESET) {
		;
	}

	/* Once PLL, PLLI2S and PLLSAI are OFF, reset PLLCFGR register to default value */
	RCC->PLLCFGR = RCC_PLLCFGR_PLLM_4 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLQ_2 | 0x20000000U;

	/* Reset PLLI2SCFGR register to default value */
	RCC->PLLI2SCFGR =
		RCC_PLLI2SCFGR_PLLI2SN_6 | RCC_PLLI2SCFGR_PLLI2SN_7 | RCC_PLLI2SCFGR_PLLI2SQ_2 | RCC_PLLI2SCFGR_PLLI2SR_1;

	/* Reset PLLSAICFGR register to default value */
	RCC->PLLSAICFGR = RCC_PLLSAICFGR_PLLSAIN_6 | RCC_PLLSAICFGR_PLLSAIN_7 | RCC_PLLSAICFGR_PLLSAIQ_2 | 0x20000000U;

	/* Disable all interrupts */
	CLEAR_BIT(RCC->CIR,
			  RCC_CIR_LSIRDYIE | RCC_CIR_LSERDYIE | RCC_CIR_HSIRDYIE | RCC_CIR_HSERDYIE | RCC_CIR_PLLRDYIE |
				  RCC_CIR_PLLI2SRDYIE | RCC_CIR_PLLSAIRDYIE);

	/* Clear all interrupt flags */
	SET_BIT(RCC->CIR,
			RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC |
				RCC_CIR_PLLI2SRDYC | RCC_CIR_PLLSAIRDYC | RCC_CIR_CSSC);

	/* Clear LSION bit */
	CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

	/* Reset all CSR flags */
	SET_BIT(RCC->CSR, RCC_CSR_RMVF);

	/* Update the SystemCoreClock global variable */
	SystemCoreClock = HSI_VALUE;
}
