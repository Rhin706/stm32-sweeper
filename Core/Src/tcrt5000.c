/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tcrt5000.c
  * @brief   红外传感器驱动库实现
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

/* Includes ------------------------------------------------------------------*/
#include "tcrt5000.h"
#include <stdio.h>
#include <string.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

// 全局变量定义
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern UART_HandleTypeDef huart1;

/* USER CODE END EV */

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

/* USER CODE BEGIN 1 */

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
                               uint32_t adc_channel)
{
  if(hinf == NULL || adc_handle == NULL)
  {
    return HAL_ERROR;
  }
  
  // 初始化结构体成员
  hinf->adc_handle = adc_handle;
  hinf->adc_channel = adc_channel;
  hinf->raw_value = 0;
  hinf->mapped_value = 0;
  hinf->status = INFRARED_NO_OBJECT;
  hinf->last_send_time = 0;
  
  // 启动ADC
  if (HAL_ADC_Start(adc_handle) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  return HAL_OK;
}

/**
 * @brief  红外传感器读取模拟量值
 * @param  hinf: 红外传感器句柄指针
 * @retval 原始ADC值 (0-4095)
 * @note   读取指定红外传感器的模拟输出值
 */
uint16_t Infrared_ReadAnalog(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return 0;
  }
  
  // 选择正确的ADC通道
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = hinf->adc_channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  
  if (HAL_ADC_ConfigChannel(hinf->adc_handle, &sConfig) != HAL_OK)
  {
    return 0;
  }
  
  // 读取ADC值
  if (HAL_ADC_PollForConversion(hinf->adc_handle, HAL_MAX_DELAY) == HAL_OK)
  {
    return (uint16_t)HAL_ADC_GetValue(hinf->adc_handle);
  }
  
  return 0;
}

/**
 * @brief  红外传感器数据处理函数
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   处理红外传感器的原始ADC值，将其映射到0-1000范围
 */
void Infrared_ProcessData(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return;
  }
  
  // 读取原始ADC值
  hinf->raw_value = Infrared_ReadAnalog(hinf);
  
  // 将ADC值映射到0-1000范围（使用定点数运算）
  if (hinf->raw_value <= INFRARED_ADC_MIN)
  {
    hinf->mapped_value = INFRARED_MAPPED_MAX;  // 强反射
  }
  else if (hinf->raw_value >= INFRARED_ADC_MAX)
  {
    hinf->mapped_value = 0;  // 弱反射
  }
  else
  {
    // 定点数线性映射：将[INFRARED_ADC_MIN, INFRARED_ADC_MAX]映射到[INFRARED_MAPPED_MAX, 0]
    uint32_t temp = (uint32_t)(INFRARED_ADC_MAX - hinf->raw_value) * INFRARED_MAPPED_MAX;
    hinf->mapped_value = (uint16_t)(temp / (INFRARED_ADC_MAX - INFRARED_ADC_MIN));
  }
  
  // 根据映射值判断检测状态
  if (hinf->mapped_value > INFRARED_THRESHOLD_NEAR)
  {
    hinf->status = INFRARED_CLOSE_LIGHT;  // 近距离浅色物体
  }
  else if (hinf->mapped_value > INFRARED_THRESHOLD_MID)
  {
    hinf->status = INFRARED_MID_RANGE;    // 中距离物体
  }
  else if (hinf->mapped_value > INFRARED_THRESHOLD_FAR)
  {
    hinf->status = INFRARED_FAR_DARK;     // 远距离深色物体
  }
  else
  {
    hinf->status = INFRARED_NO_OBJECT;    // 未检测到物体
  }
}

/**
 * @brief  红外传感器通过串口发送数据
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   将红外传感器数据通过USART1发送到电脑
 */
void Infrared_SendData(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return;
  }
  
  char uart_buffer[128];
  
  // 格式化输出原始ADC值和映射值（简短格式）
  sprintf(uart_buffer, "IR:%4d M:%4d\r\n", 
          hinf->raw_value, hinf->mapped_value);
  
  // 通过USART1发送数据，超时时间1000ms
  HAL_UART_Transmit(&huart1, (uint8_t*)uart_buffer, strlen(uart_buffer), 1000);
}

/**
 * @brief  红外传感器发送启动信息
 * @retval None
 * @note   发送红外传感器驱动库启动信息
 */
void Infrared_SendStartupInfo(void)
{
  char startup_info[] = "IR sensor ready\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)startup_info, strlen(startup_info), 1000);
}

/**
 * @brief  红外传感器检查是否需要发送数据
 * @param  hinf: 红外传感器句柄指针
 * @retval 需要发送数据返回1，否则返回0
 * @note   检查是否达到发送间隔
 */
uint8_t Infrared_ShouldSendData(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return 0;
  }
  
  return (HAL_GetTick() - hinf->last_send_time >= INFRARED_SEND_INTERVAL) ? 1 : 0;
}

/**
 * @brief  红外传感器更新发送时间
 * @param  hinf: 红外传感器句柄指针
 * @retval None
 * @note   更新上次发送时间
 */
void Infrared_UpdateSendTime(Infrared_HandleTypeDef* hinf)
{
  if(hinf != NULL)
  {
    hinf->last_send_time = HAL_GetTick();
  }
}

/**
 * @brief  获取映射后的值
 * @param  hinf: 红外传感器句柄指针
 * @retval 映射后的值 (0-1000)
 * @note   返回经过处理的映射值
 */
uint16_t Infrared_GetMappedValue(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return 0;
  }
  
  return hinf->mapped_value;
}

/**
 * @brief  获取检测状态
 * @param  hinf: 红外传感器句柄指针
 * @retval 检测状态
 * @note   返回根据映射值判断的检测状态
 */
Infrared_Status_t Infrared_GetStatus(Infrared_HandleTypeDef* hinf)
{
  if(hinf == NULL)
  {
    return INFRARED_NO_OBJECT;
  }
  
  return hinf->status;
}

/* USER CODE END 1 */