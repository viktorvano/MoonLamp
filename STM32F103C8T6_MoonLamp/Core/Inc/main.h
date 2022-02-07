/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define LAMP_INTERRUPT1_Pin GPIO_PIN_1
#define LAMP_INTERRUPT1_GPIO_Port GPIOA
#define LAMP_INTERRUPT1_EXTI_IRQn EXTI1_IRQn
#define MOTOR_INTERRUPT2_Pin GPIO_PIN_2
#define MOTOR_INTERRUPT2_GPIO_Port GPIOA
#define MOTOR_INTERRUPT2_EXTI_IRQn EXTI2_IRQn
#define MOTOR_Pin GPIO_PIN_4
#define MOTOR_GPIO_Port GPIOA
#define LAMP_Pin GPIO_PIN_5
#define LAMP_GPIO_Port GPIOA
#define ESP_RESET_Pin GPIO_PIN_3
#define ESP_RESET_GPIO_Port GPIOB
#define ESP_ENABLE_Pin GPIO_PIN_4
#define ESP_ENABLE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
