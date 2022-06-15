#ifdef STM32G431xx
#include "stm32g4xx.h"
#elif defined(STM32F746xx)
#include "stm32f7xx.h"
#else
#error "No STM32xx defined. Please add the build flag -DSTM32G431xx or -DSTM32F746xx"
#endif
