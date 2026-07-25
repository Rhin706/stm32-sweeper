/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ultrasonic.h
  * @brief   MD0017超声波测距模块驱动库头文件
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
#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "gpio.h"
#include "usart.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/**
 * @brief 超声波传感器结构体定义
 * @note  包含传感器的GPIO和定时器配置
 */
typedef struct {
  GPIO_TypeDef* trig_port;      /*!< 触发引脚端口 */
  uint16_t trig_pin;            /*!< 触发引脚号 */
  GPIO_TypeDef* echo_port;      /*!< 回响引脚端口 */
  uint16_t echo_pin;            /*!< 回响引脚号 */
  TIM_HandleTypeDef* tim_handle; /*!< 定时器句柄 */
  uint32_t last_echo_time_us;   /*!< 上次回响时间（微秒） */
  float distance_cm;            /*!< 测量距离（厘米） */
  uint8_t is_measuring;         /*!< 是否正在测量标志 */
} Ultrasonic_HandleTypeDef;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

#define ULTRASONIC_TRIGGER_PULSE_WIDTH_US   15     /*!< 触发脉冲宽度（微秒） */
#define ULTRASONIC_SOUND_SPEED_CM_US        0.034f /*!< 声速（厘米/微秒） */
#define ULTRASONIC_DISTANCE_FACTOR          58.0f  /*!< 距离计算因子（简化公式） */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/**
 * @brief  超声波传感器初始化函数
 * @param  hsonic: 超声波传感器句柄指针
 * @param  trig_port: TRIG引脚端口
 * @param  trig_pin: TRIG引脚号
 * @param  echo_port: ECHO引脚端口
 * @param  echo_pin: ECHO引脚号
 * @param  tim_handle: 定时器句柄指针
 * @retval HAL状态
 * @note   初始化超声波传感器，配置GPIO和中断
 */
HAL_StatusTypeDef Ultrasonic_Init(Ultrasonic_HandleTypeDef* hsonic,
                                  GPIO_TypeDef* trig_port, uint16_t trig_pin,
                                  GPIO_TypeDef* echo_port, uint16_t echo_pin,
                                  TIM_HandleTypeDef* tim_handle);

/**
 * @brief  触发超声波测距
 * @param  hsonic: 超声波传感器句柄指针
 * @retval HAL状态
 * @note   发送触发脉冲，启动测距
 */
HAL_StatusTypeDef Ultrasonic_Trigger(Ultrasonic_HandleTypeDef* hsonic);

/**
 * @brief  获取测量的距离
 * @param  hsonic: 超声波传感器句柄指针
 * @retval 距离值（厘米）
 * @note   返回上次测量的距离值
 */
float Ultrasonic_GetDistance(Ultrasonic_HandleTypeDef* hsonic);

/**
 * @brief  更新距离测量结果
 * @param  hsonic: 超声波传感器句柄指针
 * @param  echo_duration_us: ECHO高电平持续时间（微秒）
 * @retval None
 * @note   根据ECHO信号持续时间计算距离
 */
void Ultrasonic_UpdateDistance(Ultrasonic_HandleTypeDef* hsonic, uint32_t echo_duration_us);

/**
 * @brief  超声波传感器通过串口发送数据
 * @param  hsonic: 超声波传感器句柄指针
 * @retval None
 * @note   将超声波传感器测量的距离通过串口发送到电脑
 */
void Ultrasonic_SendData(Ultrasonic_HandleTypeDef* hsonic);

/**
 * @brief  超声波传感器外部中断回调函数
 * @param  hsonic: 超声波传感器句柄指针
 * @retval None
 * @note   处理ECHO引脚的边沿变化中断
 */
void Ultrasonic_EXTI_Callback(Ultrasonic_HandleTypeDef* hsonic);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __ULTRASONIC_H */