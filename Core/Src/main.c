/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// USB CDC缓冲区大小定义
#define APP_RX_DATA_SIZE  1024

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// 红外传感器句柄
Infrared_HandleTypeDef hinf1;  // 红外传感器1 (左侧) - PA2 (ADC1_IN2)
Infrared_HandleTypeDef hinf2;  // 红外传感器2 (中间) - PA3 (ADC1_IN3) 
Infrared_HandleTypeDef hinf3;  // 红外传感器3 (右侧) - PA0 (ADC2_IN0)

// 超声波传感器句柄
Ultrasonic_HandleTypeDef husonic1;  // 第一个超声波传感器
Ultrasonic_HandleTypeDef husonic2;  // 第二个超声波传感器（示例）

// 定时器句柄（用于超声波测距的精确时序）
TIM_HandleTypeDef htim2;  // 使用TIM2作为超声波测距的定时器

// USB CDC通信相关变量（在usbd_cdc_if.c中定义）

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void MX_TIM2_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief  初始化用于超声波测距的定时器
 * @retval None
 * @note   配置TIM2为1MHz计数频率，用于微秒级时间测量
 */
void MX_TIM2_Init(void)
{
  // 配置定时器基本参数
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;         // 假设系统时钟为72MHz，则预分频后为1MHz (72MHz/72=1MHz)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;    // 最大计数值
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE END 0 */

/* External variables */
extern uint32_t Recv_dlen;
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  // 初始化红外传感器1 (左侧) - PA2 (ADC1_IN2)
  Infrared_Init(&hinf1, &hadc1, ADC_CHANNEL_2);
  
  // 初始化红外传感器2 (中间) - PA3 (ADC1_IN3) 
  Infrared_Init(&hinf2, &hadc1, ADC_CHANNEL_3);
  
  // 初始化红外传感器3 (右侧) - PA0 (ADC2_IN0)
  Infrared_Init(&hinf3, &hadc2, ADC_CHANNEL_0);

  // 初始化定时器（用于超声波测距的精确时序）
  MX_TIM2_Init();
  
  // 初始化第一个超声波传感器（例如使用PB0作为TRIG，PB1作为ECHO）
  Ultrasonic_Init(&husonic1, GPIOB, GPIO_PIN_0, GPIOB, GPIO_PIN_1, &htim2);
  
  // 初始化第二个超声波传感器（例如使用PC0作为TRIG，PC1作为ECHO）
  Ultrasonic_Init(&husonic2, GPIOC, GPIO_PIN_0, GPIOC, GPIO_PIN_1, &htim2);

  // 发送启动信息
  Infrared_SendStartupInfo();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 红外传感器1数据处理（仅读取传感器1的数据）
    Infrared_ProcessData(&hinf1);
    
    // 每500ms发送一次红外传感器1的数据
    if (Infrared_ShouldSendData(&hinf1))
    {
      Infrared_SendData(&hinf1);
      Infrared_UpdateSendTime(&hinf1);
    }
    
    // （保留传感器2和3的代码，但不主动读取其数据）
    // 如需要，可随时启用以下代码来读取传感器2和3的数据
    /*
    // 红外传感器2数据处理
    Infrared_ProcessData(&hinf2);
    
    // 红外传感器3数据处理
    Infrared_ProcessData(&hinf3);
    */
    
    // 超声波传感器测距 - 每隔一段时间触发一次测距
    static uint32_t last_sonic_update = 0;
    if(HAL_GetTick() - last_sonic_update >= 500)  // 每500ms测量一次
    {
      // 触发第一个超声波传感器测距
      Ultrasonic_Trigger(&husonic1);
      
      // 等待测量完成或超时（最多等待100ms）
      uint32_t start_wait = HAL_GetTick();
      while(husonic1.is_measuring && (HAL_GetTick() - start_wait < 100))
      {
        // 等待测量完成
      }
      
      // 发送第一个超声波传感器数据
      Ultrasonic_SendData(&husonic1);
      
      // 触发第二个超声波传感器测距
      Ultrasonic_Trigger(&husonic2);
      
      // 等待测量完成或超时（最多等待100ms）
      start_wait = HAL_GetTick();
      while(husonic2.is_measuring && (HAL_GetTick() - start_wait < 100))
      {
        // 等待测量完成
      }
      
      // 发送第二个超声波传感器数据
      Ultrasonic_SendData(&husonic2);
      
      last_sonic_update = HAL_GetTick();
    }
    
    // USB CDC数据接收和发送处理
    if(Recv_dlen) // 判断是否接收到数据，接收置位处理在usbd_cdc_if.c中的CDC_Receive_FS函数
    {
      CDC_Transmit_FS(UserRxBufferFS, Recv_dlen); // 使用标准CDC发送函数
      Recv_dlen = 0; // 长度清零
    }
    
    HAL_Delay(10);  // 10ms延时
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief  GPIO外部中断处理函数
 * @param  GPIO_Pin: 中断引脚
 * @retval None
 * @note   处理超声波传感器ECHO引脚的边沿变化中断
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // 检查是哪个超声波传感器的ECHO引脚产生了中断
  if(GPIO_Pin == GPIO_PIN_1)  // PB1 或 PC1
  {
    if(GPIOB == husonic1.echo_port && GPIO_Pin == husonic1.echo_pin)
    {
      Ultrasonic_EXTI_Callback(&husonic1);
    }
    else if(GPIOC == husonic2.echo_port && GPIO_Pin == husonic2.echo_pin)
    {
      Ultrasonic_EXTI_Callback(&husonic2);
    }
  }
  else if(GPIO_Pin == GPIO_PIN_0)  // PB0 或 PC0
  {
    if(GPIOB == husonic1.echo_port && GPIO_Pin == husonic1.echo_pin)
    {
      Ultrasonic_EXTI_Callback(&husonic1);
    }
    else if(GPIOC == husonic2.echo_port && GPIO_Pin == husonic2.echo_pin)
    {
      Ultrasonic_EXTI_Callback(&husonic2);
    }
  }
  // 可以根据需要添加更多引脚的处理
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */