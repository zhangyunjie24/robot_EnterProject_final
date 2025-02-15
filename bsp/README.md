# *BSP*ʹ��˵��
## һ������˵��
### 1��CubeMX����
ѡ��HSEΪ����DebugΪSerial Wire

ʱ����Input frq��Ϊ8��HCLK��Ϊ168

ѡ�񹤾���ΪIDE��.c/.h�ֱ�����
#### a.������
###### TIM���ã���������PWM����
��PB9����ΪTIM4_CH4��TIM4 channel4 ����ΪPWM Generation CH4��Ԥ��Ƶ168-1���Զ���װ��ֵ1000-1��1000HZ��
#### b.�����ĵ�ѹ��⡢���͡�Ƶ�ʼ��
###### ADC���ã����ڼ���ѹ��
����ADC1��IN0��IN3��ADC2��IN1�� ADC3��IN2

�򿪸��Ե�DMA��ģʽ����Ϊcircular mode

�������ã�
- *Continuous Conversion = Enable*
- *DMA Continuous Request = Enable*

���� ADC1��*Number of conversion* ����Ϊ 2

�������У�*Sampling Time* ���� 56 Cycles

###### USART���ã��������ݴ��䣩
����usart1Ϊ�첽ģʽ��������Ϊ9600

��rx��tx��dma

###### TIM���ã�����Ƶ�ʼ�⣩
����TIM6������Ҫ����Ԥ��Ƶ��Ϊ168-1��������Ĭ��

#### c.FreeRTOS����
��������can��һ��light���̣߳�stackΪ640��640��384

total heap����Ϊ40 * 1024 bytes��ʹ��heap_4�Դ����ڴ���Ƭ

������reentrant

sysʹ��tim1��������freertos��ͻ

linker settings�У���С�Ѵ�С����Ϊ0x2000����Сջ����Ϊ0
x1000
### 2��Core��������
#### a. stm32f4xx_it.c����
����������
```c++
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint32_t MilliSec_counter = 0;
uint32_t one_tenSec_counter = 0;
uint32_t Sec_counter = 0;
/* USER CODE END PV */
```
systick���������ã�
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
#### b. main.c����
ͷ�ļ����ã�

����������
```c++
/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t adc_buffer[1];
uint8_t rx_buffer[10];
/* USER CODE END PV */
```

��ʼ�����ã�

whileѭ�����ã�



## �����ļ�˵��
### 1��led.c + led.h
    ����LED�����ƣ���ʾ���ذ�����������
### 2��interrupt.c + interrupt.h
    - ��·adc��ȡ�����ɱ��ģ���ͨ�����ڷ��͡�
    - ���ڽ��ձ��ģ�ȷ����Ӧ��adc�������ѹֵ��
    - ���յ�ѹֵ����������ŵ��ź�Ƶ�ʡ�����������ؼ�⣩

