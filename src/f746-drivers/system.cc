#include "drivers/system.hh"
#include "stm32xx.h"

//stub for adapting to g431 C code
extern "C" void init_timekeeper() {
}
extern "C" void init_debouncer() {
}

extern "C" void system_init() {
	mdrivlib::System::SetVectorTable(0x08000000);
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
		.APB2CLKDivider = RCC_HCLK_DIV2,
	};
	RCC_PeriphCLKInitTypeDef noperiphclk{0};
	mdrivlib::SystemClocks::init_clocks(RCC_OscInitStruct, RCC_ClkInitStruct, noperiphclk, 1000);

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
