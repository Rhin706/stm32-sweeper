# TCRT5000红外反射传感器驱动库

## 概述

TCRT5000驱动库是一个专为STM32F103C8T6微控制器设计的红外反射传感器驱动程序。该库通过模拟量接口(AO)读取传感器数据，并通过串口将数据传输到PC端进行监控。

## 功能特性

- **模拟量读取**：通过PA1引脚读取TCRT5000传感器的模拟输出
- **数据映射**：将ADC原始值(0-4095)映射到0-1000范围
- **距离检测**：根据反射强度判断物体距离
- **表面特性识别**：区分浅色和深色表面
- **串口通信**：通过USART1发送数据到PC端
- **模块化设计**：提供清晰的API接口

## 硬件连接

| TCRT5000传感器 | STM32F103C8T6 |
|----------------|---------------|
| VCC            | 3.3V 或 5V    |
| GND            | GND           |
| AO (模拟输出)   | PA1           |
| DO (数字输出)   | 不连接        |

串口连接：
- STM32 PA9 (TX) → USB转串口模块 RX
- 波特率：115200

## API接口

### 初始化函数
```c
HAL_StatusTypeDef TCRT5000_Init(void);
```
初始化TCRT5000传感器，启动ADC。

### 数据读取函数
```c
uint16_t TCRT5000_ReadAnalog(void);
```
读取原始ADC值 (0-4095)。

### 数据处理函数
```c
void TCRT5000_ProcessData(void);
```
处理传感器数据并映射到0-1000范围。

### 数据发送函数
```c
void TCRT5000_SendData(void);
```
通过串口发送传感器数据。

### 状态获取函数
```c
TCRT5000_Status_t TCRT5000_GetStatus(void);
```
获取检测状态。

```c
uint16_t TCRT5000_GetMappedValue(void);
```
获取映射后的值 (0-1000)。

## 使用方法

### 1. 初始化
在main函数的初始化部分调用：
```c
TCRT5000_Init();
```

### 2. 主循环
在主循环中定期处理数据：
```c
while (1)
{
  // 处理传感器数据
  TCRT5000_ProcessData();
  
  // 每500ms发送一次数据
  if (TCRT5000_ShouldSendData())
  {
    TCRT5000_SendData();
    TCRT5000_UpdateSendTime();
  }
  
  HAL_Delay(10);
}
```

## 输出格式

串口输出示例：
```
TCRT5000驱动库启动成功
检测距离范围: 1-25mm
数据范围: 0-1000
========================
原始ADC值: 1234, 映射值:  856
状态: 检测到近距离物体（浅色表面）
```

## 参数配置

可在tcrt5000.h中调整以下参数：
- `TCRT5000_ADC_MIN`: 最小ADC值（强反射）
- `TCRT5000_ADC_MAX`: 最大ADC值（弱反射）
- `TCRT5000_THRESHOLD_NEAR`: 近距离阈值
- `TCRT5000_THRESHOLD_MID`: 中距离阈值
- `TCRT5000_THRESHOLD_FAR`: 远距离阈值
- `TCRT5000_SEND_INTERVAL`: 发送间隔（ms）

## 注意事项

1. 确保PA1引脚配置为模拟输入模式
2. 传感器检测距离一般在1-25mm范围内
3. 浅色表面反射更强，深色表面反射较弱
4. 需要适当的供电电压（3.3V或5V）