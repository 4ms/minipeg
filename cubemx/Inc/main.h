/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_pwr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CYCLE_BUT_Pin GPIO_PIN_13
#define CYCLE_BUT_GPIO_Port GPIOC
#define PING_BUT_Pin GPIO_PIN_14
#define PING_BUT_GPIO_Port GPIOC
#define PING_JACK_Pin GPIO_PIN_15
#define PING_JACK_GPIO_Port GPIOC
#define PING_JACK_EXTI_IRQn EXTI15_10_IRQn
#define CYCLE_JACK_Pin GPIO_PIN_0
#define CYCLE_JACK_GPIO_Port GPIOA
#define CYCLE_JACK_EXTI_IRQn EXTI0_IRQn
#define T2C2_LED_EOF_Pin GPIO_PIN_1
#define T2C2_LED_EOF_GPIO_Port GPIOA
#define T2C4_LOCKBUT_G_Pin GPIO_PIN_3
#define T2C4_LOCKBUT_G_GPIO_Port GPIOA
#define ENV_Pin GPIO_PIN_4
#define ENV_GPIO_Port GPIOA
#define ENV_5V_Pin GPIO_PIN_5
#define ENV_5V_GPIO_Port GPIOA
#define A2C3_POT_OFFSET_Pin GPIO_PIN_6
#define A2C3_POT_OFFSET_GPIO_Port GPIOA
#define A2C4_POT_SCALE_Pin GPIO_PIN_7
#define A2C4_POT_SCALE_GPIO_Port GPIOA
#define A1C15_CV_SHAPE_Pin GPIO_PIN_0
#define A1C15_CV_SHAPE_GPIO_Port GPIOB
#define LOCK_BUT_Pin GPIO_PIN_1
#define LOCK_BUT_GPIO_Port GPIOB
#define A2C12_POT_SHAPE_Pin GPIO_PIN_2
#define A2C12_POT_SHAPE_GPIO_Port GPIOB
#define T2C3_LOCKBUT_B_Pin GPIO_PIN_10
#define T2C3_LOCKBUT_B_GPIO_Port GPIOB
#define A2C14_POT_DIVMULT_Pin GPIO_PIN_11
#define A2C14_POT_DIVMULT_GPIO_Port GPIOB
#define A1C11_CV_DIVMULT_Pin GPIO_PIN_12
#define A1C11_CV_DIVMULT_GPIO_Port GPIOB
#define TRIG_JACK_Pin GPIO_PIN_13
#define TRIG_JACK_GPIO_Port GPIOB
#define TRIG_JACK_EXTI_IRQn EXTI15_10_IRQn
#define T15C1_ENV_B_Pin GPIO_PIN_14
#define T15C1_ENV_B_GPIO_Port GPIOB
#define T15C2_ENV_G_Pin GPIO_PIN_15
#define T15C2_ENV_G_GPIO_Port GPIOB
#define T1C1_ENV_R_Pin GPIO_PIN_8
#define T1C1_ENV_R_GPIO_Port GPIOA
#define T1C2_CYCLEBUT_B_Pin GPIO_PIN_9
#define T1C2_CYCLEBUT_B_GPIO_Port GPIOA
#define T1C3_CYCLEBUT_G_Pin GPIO_PIN_10
#define T1C3_CYCLEBUT_G_GPIO_Port GPIOA
#define T1C4_CYCLEBUT_R_Pin GPIO_PIN_11
#define T1C4_CYCLEBUT_R_GPIO_Port GPIOA
#define EOF_Pin GPIO_PIN_12
#define EOF_GPIO_Port GPIOA
#define T2C1_ENV5V_B_Pin GPIO_PIN_15
#define T2C1_ENV5V_B_GPIO_Port GPIOA
#define T3C1_ENV5V_G_Pin GPIO_PIN_4
#define T3C1_ENV5V_G_GPIO_Port GPIOB
#define T3C2_ENV5V_R_Pin GPIO_PIN_5
#define T3C2_ENV5V_R_GPIO_Port GPIOB
#define T4C1_PINGBUT_B_Pin GPIO_PIN_6
#define T4C1_PINGBUT_B_GPIO_Port GPIOB
#define T3C4_PINGBUT_G_Pin GPIO_PIN_7
#define T3C4_PINGBUT_G_GPIO_Port GPIOB
#define T4C3_LOCKBUT_R_Pin GPIO_PIN_8
#define T4C3_LOCKBUT_R_GPIO_Port GPIOB
#define T4C4_PINGBUT_R_Pin GPIO_PIN_9
#define T4C4_PINGBUT_R_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
