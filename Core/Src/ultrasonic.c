/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ultrasonic.c
  * @brief   MD0017超声波测距模块驱动库实现
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
#include "ultrasonic.h"
#include <stdio.h>
#include <string.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

/* USER CODE BEGIN 1 */

// 全局变量用于存储超声波传感器实例指针（在中断中使用）
static Ultrasonic_HandleTypeDef* p_ultrasonic_echo_falling_edge = NULL;
static uint32_t echo_start_time = 0;
static uint32_t echo_end_time = 0;
static uint8_t echo_edge_count = 0;  // 用于跟踪边沿状态

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
                                  TIM_HandleTypeDef* tim_handle)
{
  // 检查参数有效性
  if(hsonic == NULL || trig_port == NULL || echo_port == NULL || tim_handle == NULL)
  {
    return HAL_ERROR;
  }

  // 初始化结构体成员
  hsonic->trig_port = trig_port;
  hsonic->trig_pin = trig_pin;
  hsonic->echo_port = echo_port;
  hsonic->echo_pin = echo_pin;
  hsonic->tim_handle = tim_handle;
  hsonic->last_echo_time_us = 0;
  hsonic->distance_cm = 0.0f;
  hsonic->is_measuring = 0;

  // 启动定时器
  if(HAL_TIM_Base_Init(tim_handle) != HAL_OK)
  {
    return HAL_ERROR;
  }
  
  if(HAL_TIM_Base_Start(tim_handle) != HAL_OK)
  {
    return HAL_ERROR;
  }

  // 配置TRIG引脚为输出模式
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = trig_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(trig_port, &GPIO_InitStruct);

  // 配置ECHO引脚为输入模式，上拉
  GPIO_InitStruct.Pin = echo_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;  // 配置为上升沿和下降沿中断
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(echo_port, &GPIO_InitStruct);

  // 初始状态下TRIG保持低电平
  HAL_GPIO_WritePin(trig_port, trig_pin, GPIO_PIN_RESET);

  // 使能GPIO中断
  if(echo_pin == GPIO_PIN_0) {
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  } else if(echo_pin == GPIO_PIN_1) {
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  } else if(echo_pin == GPIO_PIN_2) {
    HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  } else if(echo_pin == GPIO_PIN_3) {
    HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  } else if(echo_pin == GPIO_PIN_4) {
    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  } else if(echo_pin >= GPIO_PIN_5 && echo_pin <= GPIO_PIN_9) {
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  } else if(echo_pin >= GPIO_PIN_10 && echo_pin <= GPIO_PIN_15) {
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  }

  return HAL_OK;
}

/**
 * @brief  触发超声波测距
 * @param  hsonic: 超声波传感器句柄指针
 * @retval HAL状态
 * @note   发送触发脉冲，启动测距
 */
HAL_StatusTypeDef Ultrasonic_Trigger(Ultrasonic_HandleTypeDef* hsonic)
{
  if(hsonic == NULL)
  {
    return HAL_ERROR;
  }

  // 设置TRIG引脚为高电平，持续至少10微秒
  HAL_GPIO_WritePin(hsonic->trig_port, hsonic->trig_pin, GPIO_PIN_SET);

  // 使用微秒级延时，确保至少10微秒的触发脉冲
  // 这里使用一个简单的循环延时，实际应用中建议使用定时器
  volatile uint32_t delay_us = 15; // 15微秒触发脉冲
  volatile uint32_t counter = 0;
  // 假设系统时钟为72MHz，大约每个循环3条指令，则需要约360次循环
  // 实际应用中应根据系统时钟频率校准此延时
  for(counter = 0; counter < delay_us * 12; counter++) {
    // 空循环实现微秒级延时
    // 该数值需要根据实际系统时钟频率进行调整
  }

  // 拉低TRIG引脚，完成触发
  HAL_GPIO_WritePin(hsonic->trig_port, hsonic->trig_pin, GPIO_PIN_RESET);

  // 设置为正在测量状态
  hsonic->is_measuring = 1;
  echo_edge_count = 0;
  p_ultrasonic_echo_falling_edge = hsonic;

  return HAL_OK;
}

/**
 * @brief  获取测量的距离
 * @param  hsonic: 超声波传感器句柄指针
 * @retval 距离值（厘米）
 * @note   返回上次测量的距离值
 */
float Ultrasonic_GetDistance(Ultrasonic_HandleTypeDef* hsonic)
{
  if(hsonic == NULL)
  {
    return 0.0f;
  }

  return hsonic->distance_cm;
}

/**
 * @brief  更新距离测量结果
 * @param  hsonic: 超声波传感器句柄指针
 * @param  echo_duration_us: ECHO高电平持续时间（微秒）
 * @retval None
 * @note   根据ECHO信号持续时间计算距离
 */
void Ultrasonic_UpdateDistance(Ultrasonic_HandleTypeDef* hsonic, uint32_t echo_duration_us)
{
  if(hsonic == NULL)
  {
    return;
  }

  // 记录回响时间
  hsonic->last_echo_time_us = echo_duration_us;

  // 根据公式计算距离：距离(cm) = 时间(μs) / 58
  // 使用整数运算避免浮点运算库的链接
  hsonic->distance_cm = (float)echo_duration_us / 58.0f;
}

/**
 * @brief  超声波传感器通过串口发送数据
 * @param  hsonic: 超声波传感器句柄指针
 * @retval None
 * @note   将超声波传感器测量的距离通过串口发送到电脑
 */
void Ultrasonic_SendData(Ultrasonic_HandleTypeDef* hsonic)
{
  if(hsonic == NULL)
  {
    return;
  }

  char uart_buffer[64];

  // 格式化输出距离信息（使用整数方式避免浮点数格式化）
  int distance_int = (int)hsonic->distance_cm;
  sprintf(uart_buffer, "US:%d cm\r\n", distance_int);

  // 通过USART1发送数据，超时时间1000ms
  HAL_UART_Transmit(&huart1, (uint8_t*)uart_buffer, strlen(uart_buffer), 1000);
}

/**
 * @brief  超声波传感器外部中断回调函数
 * @param  hsonic: 超声波传感器句柄指针
 * @retval None
 * @note   处理ECHO引脚的边沿变化中断
 */
void Ultrasonic_EXTI_Callback(Ultrasonic_HandleTypeDef* hsonic)
{
  if(hsonic == NULL)
    return;

  // 检查ECHO引脚的状态
  if(HAL_GPIO_ReadPin(hsonic->echo_port, hsonic->echo_pin) == GPIO_PIN_SET)
  {
    // 上升沿：开始计时
    echo_start_time = __HAL_TIM_GET_COUNTER(hsonic->tim_handle);
    echo_edge_count = 1;
  }
  else if(HAL_GPIO_ReadPin(hsonic->echo_port, hsonic->echo_pin) == GPIO_PIN_RESET && echo_edge_count == 1)
  {
    // 下降沿：结束计时
    echo_end_time = __HAL_TIM_GET_COUNTER(hsonic->tim_handle);
    echo_edge_count = 0;

    // 计算ECHO高电平持续时间（假设定时器时钟为1MHz，即每计数代表1微秒）
    uint32_t duration_us = 0;
    if(echo_end_time >= echo_start_time)
    {
      duration_us = echo_end_time - echo_start_time;
    }
    else
    {
      // 处理计数器溢出情况
      duration_us = (0xFFFFFFFF - echo_start_time) + echo_end_time + 1;
    }

    // 更新距离值
    Ultrasonic_UpdateDistance(hsonic, duration_us);

    // 测量完成
    hsonic->is_measuring = 0;
  }
}

/**
 * @brief  超声波传感器发送启动信息
 * @retval None
 * @note   发送超声波传感器驱动库启动信息
 */
void Ultrasonic_SendStartupInfo(void)
{
  char startup_info[] = "US sensor ready\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)startup_info, strlen(startup_info), 1000);
}

/* USER CODE END 1 */