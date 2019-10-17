#include "globals.h"
extern volatile uint32_t pingtmr;
extern volatile uint32_t ping_irq_timestamp;
extern volatile uint8_t using_tap_clock;


debounced_digin_t digin[NUM_DEBOUNCED_DIGINS];
TIM_HandleTypeDef debounce_tim;

void init_debouncer(void)
{
    for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
    {
        digin[i].history = 0xFFFF;
        digin[i].state = 0;
        digin[i].edge = 0;
    }
    DEBOUNCE_TIM_RCC_ENABLE();
    
    debounce_tim.Instance = DEBOUNCE_TIMx;
    debounce_tim.Init.Prescaler = 0;
    debounce_tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    debounce_tim.Init.Period = 5000;
    debounce_tim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&debounce_tim);

    HAL_NVIC_SetPriority(DEBOUNCE_IRQn, 1, 0); //was 1
    HAL_NVIC_EnableIRQ(DEBOUNCE_IRQn);
}

void DEBOUNCE_IRQ(void)
{
    uint16_t t;
    uint16_t pin_read;

    if ((__HAL_TIM_GET_FLAG(&debounce_tim, TIM_FLAG_UPDATE) != RESET) &&
        (__HAL_TIM_GET_IT_SOURCE(&debounce_tim, TIM_IT_UPDATE) != RESET))
    {
        __HAL_TIM_CLEAR_IT(&debounce_tim, TIM_IT_UPDATE);

        for (uint8_t i=0; i<NUM_DEBOUNCED_DIGINS; i++)
        {
            if (i==PING_BUTTON)
                pin_read = PINGBUT;
            
            else if (i==CYCLE_BUTTON)
                pin_read = CYCLEBUT;

            else if (i==TRIGGER_JACK)
                pin_read = TRIGJACK_READ;

            else if (i==CYCLE_JACK)
                pin_read = CYCLEJACK_READ;

            else if (i==PING_JACK)
                pin_read = PINGJACK;

            if (pin_read) t=0x0000;
            else t=0x0001;

            digin[i].history=(digin[i].history<<1) | t;

            if (digin[i].history==0xFFFE)
            {
                digin[i].state = 1;
                digin[i].edge = 1;
                if (i==PING_JACK) {
                    ping_irq_timestamp=pingtmr;
                    pingtmr=0;
                    using_tap_clock=0;
                }
            }
            else if (digin[i].history==0x0001)
            {
                digin[i].state = 0;
                digin[i].edge = -1;
            }

        }
    }
}
