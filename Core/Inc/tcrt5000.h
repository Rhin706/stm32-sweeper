/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tcrt5000.h
  * @brief   TCRT5000红外反射传感器驱动库头文件
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
#ifndef __TCRT5000_H
#define __TCRT5000_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/**
 * @brief 红外传感器检测状态枚举
 */
typedef enum {
  INFRARED_NO_OBJECT = 0,      // 未检测到物体
  INFRARED_FAR_DARK = 1,       // 远距离深色物体
  INFRARED_MID_RANGE = 2,      // 中距离物体
  INFRARED_CLOSE_LIGHT = 3     // 近距离浅色物体
} Infrared_Status_t;

/**
 * @brief 红外传感器结构体定义
 */
typedef struct {
  ADC_HandleTypeDef* adc_handle;  /*!< ADC句柄 */
  uint32_t adc_channel;          /*!< ADC通道 */
  uint16_t raw_value;            /*!< 原始ADC值 */
  uint16_t mapped_value;         /*!< 映射值 */
  Infrared_Status_t status;      /*!< 检测状态 */
  uint32_t last_send_time;       /*!< 上次发送时间 */
} Infrared_HandleTypeDef;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

// 红外传感器参数定义
#define INFRARED_ADC_MIN         0       // 最小ADC值
#define INFRARED_ADC_MAX         4095    // 最大ADC值
#define INFRARED_MAPPED_MAX      1000    // 映射最大值
#define INFRARED_THRESHOLD_NEAR  800     // 近距离阈值
#define INFRARED_THRESHOLD_MID   500     // 中距离阈值
#define INFRARED_THRESHOLD_FAR   200     // 远距离阈值
#define INFRARED_SEND_INTERVAL   500     // 发送间隔（ms）

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/**
 * @brief  红外传感器初始化函数
 * @param  hinf: 红外传感器句柄指针
 * @param  adc_handle: ADC句柄指针
 * @param  adc_channel: ADC通道
 * @retval HAL状态
 * @note   初始化指定的红外传感器
 */
HAL_StatusTypeDef Infrared_Init(Infrared_HandleTypeDef* hinf, 
                               ADC_HandleTypeDef* adc_handle, 
                               uint32_t adc_channel);

/**
 * @brief  红外传感器读取模拟量值
 * @param  hinf: 红外传感器句柄指针
 * @retval 原始ADC值 (0-4095)
 * @note   读取指定红外传感器的模拟输出值
 */
uint16_t Infrared_ReadAnalog(Infrared_HandleTypeDef* hinf);

/**
 * @brief  红外传感器数据处理函数
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   处理红外传感器的原始ADC值，将其映射到0-1000范围
 */
void Infrared_ProcessData(Infrared_HandleTypeDef* hinf);

/**
 * @brief  红外传感器通过串口发送数据
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   将红外传感器数据通过USART1发送到电脑
 */
void Infrared_SendData(Infrared_HandleTypeDef* hinf);

/**
 * @brief  红外传感器发送启动信息
 * @retval None
 * @note   发送红外传感器驱动库启动信息
 */
void Infrared_SendStartupInfo(void);

/**
 * @brief  红外传感器检查是否需要发送数据
 * @param  hinf: 红外传感器句柄指针
 * @retval 需要发送数据返回1，否则返回0
 * @note   检查是否达到发送间隔
 */
uint8_t Infrared_ShouldSendData(Infrared_HandleTypeDef* hinf);

/**
 * @brief  红外传感器更新发送时间
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   更新上次发送时间
 */
void Infrared_UpdateSendTime(Infrared_HandleTypeDef* hinf);

/**
 * @brief  获取映射后的值
 * @param  hinf: 红外传感器句柄指针
 * @retval 映射后的值 (0-1000)
 * @note   返回经过处理的映射值
 */
uint16_t Infrared_GetMappedValue(Infrared_HandleTypeDef* hinf);

/**
 * @brief  获取检测状态
 * @param  hinf: 红外传感器句柄指针
 * @retval 检测状态
 * @note   返回根据映射值判断的检测状态
 */
Infrared_Status_t Infrared_GetStatus(Infrared_HandleTypeDef* hinf);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __TCRT5000_H */