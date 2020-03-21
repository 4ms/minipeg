#pragma once
#include <stdint.h>

void system_init(void);
void Error_Handler(void);
void enable_gpio_rcc(GPIO_TypeDef *port);
void enable_tim_rcc(TIM_TypeDef *TIM);
void enable_adc_rcc(ADC_TypeDef *ADCx);
void enable_dma_rcc(DMA_TypeDef *DMAx);
uint8_t tim_periph_to_num(TIM_TypeDef *TIM);
