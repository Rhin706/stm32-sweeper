/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tcrt5000.h"
#include "ultrasonic.h"
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
#define AIN1_Pin GPIO_PIN_13
#define AIN1_GPIO_Port GPIOC
#define AIN2_Pin GPIO_PIN_14
#define AIN2_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_15
#define BIN1_GPIO_Port GPIOC
#define red_3_Pin GPIO_PIN_0
#define red_3_GPIO_Port GPIOA
#define huidu_Pin GPIO_PIN_1
#define huidu_GPIO_Port GPIOA
#define red_1_Pin GPIO_PIN_2
#define red_1_GPIO_Port GPIOA
#define red_2_Pin GPIO_PIN_3
#define red_2_GPIO_Port GPIOA
#define E1A_Pin GPIO_PIN_6
#define E1A_GPIO_Port GPIOA
#define E1B_Pin GPIO_PIN_7
#define E1B_GPIO_Port GPIOA
#define Trig1_Pin GPIO_PIN_0
#define Trig1_GPIO_Port GPIOB
#define Echo1_Pin GPIO_PIN_1
#define Echo1_GPIO_Port GPIOB
#define Trig2_Pin GPIO_PIN_2
#define Trig2_GPIO_Port GPIOB
#define OledSCL_Pin GPIO_PIN_10
#define OledSCL_GPIO_Port GPIOB
#define OledSDA_Pin GPIO_PIN_11
#define OledSDA_GPIO_Port GPIOB
#define PWMA_Pin GPIO_PIN_8
#define PWMA_GPIO_Port GPIOA
#define HC_TX_Pin GPIO_PIN_9
#define HC_TX_GPIO_Port GPIOA
#define HC_RX_Pin GPIO_PIN_10
#define HC_RX_GPIO_Port GPIOA
#define BIN2_Pin GPIO_PIN_4
#define BIN2_GPIO_Port GPIOB
#define Echo2_Pin GPIO_PIN_5
#define Echo2_GPIO_Port GPIOB
#define E2A_Pin GPIO_PIN_6
#define E2A_GPIO_Port GPIOB
#define E2B_Pin GPIO_PIN_7
#define E2B_GPIO_Port GPIOB
#define PWMB_Pin GPIO_PIN_8
#define PWMB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
