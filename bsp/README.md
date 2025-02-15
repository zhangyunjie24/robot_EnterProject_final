# *BSP*使用说明
## 一、配置说明
### 1）CubeMX配置
选择HSE为晶振、Debug为Serial Wire

时钟树Input frq设为8、HCLK设为168

选择工具链为IDE、.c/.h分别生成
#### a.呼吸灯
###### TIM配置（用于生成PWM波）
将PB9配置为TIM4_CH4，TIM4 channel4 设置为PWM Generation CH4，预分频168-1、自动重装载值1000-1（1000HZ）
#### b.方波的电压检测、发送、频率检测
###### ADC配置（用于检测电压）
配置ADC1的IN0，IN3；ADC2的IN1； ADC3的IN2

打开各自的DMA，模式设置为circular mode

总体配置：
- *Continuous Conversion = Enable*
- *DMA Continuous Request = Enable*

对于 ADC1，*Number of conversion* 设置为 2

对于所有，*Sampling Time* 都是 56 Cycles

###### USART配置（用于数据传输）
配置usart1为异步模式，波特率为9600

打开rx和tx的dma

###### TIM配置（用于频率检测）
配置TIM6，仅需要设置预分频数为168-1，其他均默认

#### c.FreeRTOS配置
配置两个can、一个light的线程，stack为640、640、384

total heap调整为40 * 1024 bytes，使用heap_4以处理内存碎片

打开允许reentrant

sys使用tim1，避免与freertos冲突

linker settings中，最小堆大小设置为0x2000，最小栈设置为0
x1000
### 2）Core代码设置
#### a. stm32f4xx_it.c设置
变量声明：
```c++
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint32_t MilliSec_counter = 0;
uint32_t one_tenSec_counter = 0;
uint32_t Sec_counter = 0;
/* USER CODE END PV */
```
systick函数的设置：
```c++
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  MilliSec_counter++;
  if (MilliSec_counter % 100 == 0)  one_tenSec_counter++;
  if (MilliSec_counter % 1000 == 0 )  Sec_counter++;
  /* USER CODE END SysTick_IRQn 1 */
}
```
#### b. main.c设置
头文件引用：

变量声明：
```c++
/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t adc_buffer[1];
uint8_t rx_buffer[10];
/* USER CODE END PV */
```

初始化配置：

while循环调用：



## 二、文件说明
### 1）led.c + led.h
    设置LED呼吸灯，显示主控板正常工作。
### 2）interrupt.c + interrupt.h
    - 各路adc读取后生成报文，并通过串口发送。
    - 串口接收报文，确定对应的adc引脚与电压值。
    - 依照电压值，计算此引脚的信号频率。（软件上升沿检测）

