#include "drivers/system.hh"
#include "../system.hh"
#include "drivers/rcc.hh"
#include "flash_layout.hh"
#include "settings.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32xx.h"

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
				.PLLM = 16,
				.PLLN = 200,
				.PLLP = RCC_PLLP_DIV2,
				.PLLQ = 2,
				.PLLR = 2,
			},
	};

	RCC_ClkInitTypeDef RCC_ClkInitStruct = {
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV2,
		.APB2CLKDivider = RCC_HCLK_DIV1,
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

	__HAL_DBGMCU_FREEZE_TIM7();
}

void reset_buses() {
	mdrivlib::RCC_Reset::APB1::write(0xFFFFFFFF);
	mdrivlib::RCC_Reset::APB1::write(0x0);

	mdrivlib::RCC_Reset::APB2::write(0xFFFFFFFF);
	mdrivlib::RCC_Reset::APB2::write(0x0);

	mdrivlib::RCC_Reset::AHB1::write(0xFFFFFFFF);
	mdrivlib::RCC_Reset::AHB1::write(0x0);

	mdrivlib::RCC_Reset::AHB2::write(0xFFFFFFFF);
	mdrivlib::RCC_Reset::AHB2::write(0x0);

	mdrivlib::RCC_Reset::AHB3::write(0xFFFFFFFF);
	mdrivlib::RCC_Reset::AHB3::write(0x0);
}

//LL_RCC_DeInit + systick disable
void reset_RCC() {
	// Disable SysTick IRQ and SysTick Timer
	SysTick->CTRL = SysTick->CTRL & ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);

	/* Set HSION bit */
	LL_RCC_HSI_Enable();

	/* Wait for HSI READY bit */
	while (LL_RCC_HSI_IsReady() != 1U) {
	}

	/* Reset CFGR register */
	LL_RCC_WriteReg(CFGR, 0x00000000U);

	/* Read CR register */
	uint32_t vl_mask = LL_RCC_ReadReg(CR);

	/* Reset HSEON, HSEBYP, PLLON, CSSON bits */
	CLEAR_BIT(vl_mask, (RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_PLLON | RCC_CR_CSSON));

#if defined(RCC_PLLSAI_SUPPORT)
	/* Reset PLLSAION bit */
	CLEAR_BIT(vl_mask, RCC_CR_PLLSAION);
#endif /* RCC_PLLSAI_SUPPORT */

#if defined(RCC_PLLI2S_SUPPORT)
	/* Reset PLLI2SON bit */
	CLEAR_BIT(vl_mask, RCC_CR_PLLI2SON);
#endif /* RCC_PLLI2S_SUPPORT */

	/* Write new value in CR register */
	LL_RCC_WriteReg(CR, vl_mask);

	/* Set HSITRIM bits to the reset value*/
	LL_RCC_HSI_SetCalibTrimming(0x10U);

	/* Wait for PLL READY bit to be reset */
	while (LL_RCC_PLL_IsReady() != 0U) {
	}

	/* Reset PLLCFGR register */
	LL_RCC_WriteReg(PLLCFGR, RCC_PLLCFGR_RST_VALUE);

#if defined(RCC_PLLI2S_SUPPORT)
	/* Reset PLLI2SCFGR register */
	LL_RCC_WriteReg(PLLI2SCFGR, RCC_PLLI2SCFGR_RST_VALUE);
#endif /* RCC_PLLI2S_SUPPORT */

#if defined(RCC_PLLSAI_SUPPORT)
	/* Reset PLLSAICFGR register */
	LL_RCC_WriteReg(PLLSAICFGR, RCC_PLLSAICFGR_RST_VALUE);
#endif /* RCC_PLLSAI_SUPPORT */

	/* Disable all interrupts */
	CLEAR_BIT(RCC->CIR, RCC_CIR_LSIRDYIE | RCC_CIR_LSERDYIE | RCC_CIR_HSIRDYIE | RCC_CIR_HSERDYIE | RCC_CIR_PLLRDYIE);

#if defined(RCC_CIR_PLLI2SRDYIE)
	CLEAR_BIT(RCC->CIR, RCC_CIR_PLLI2SRDYIE);
#endif /* RCC_CIR_PLLI2SRDYIE */

#if defined(RCC_CIR_PLLSAIRDYIE)
	CLEAR_BIT(RCC->CIR, RCC_CIR_PLLSAIRDYIE);
#endif /* RCC_CIR_PLLSAIRDYIE */

	/* Clear all interrupt flags */
	SET_BIT(RCC->CIR,
			RCC_CIR_LSIRDYC | RCC_CIR_LSERDYC | RCC_CIR_HSIRDYC | RCC_CIR_HSERDYC | RCC_CIR_PLLRDYC | RCC_CIR_CSSC);

#if defined(RCC_CIR_PLLI2SRDYC)
	SET_BIT(RCC->CIR, RCC_CIR_PLLI2SRDYC);
#endif /* RCC_CIR_PLLI2SRDYC */

#if defined(RCC_CIR_PLLSAIRDYC)
	SET_BIT(RCC->CIR, RCC_CIR_PLLSAIRDYC);
#endif /* RCC_CIR_PLLSAIRDYC */

	/* Clear LSION bit */
	CLEAR_BIT(RCC->CSR, RCC_CSR_LSION);

	/* Reset all CSR flags */
	SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}
