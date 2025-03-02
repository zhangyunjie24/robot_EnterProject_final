/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "../../bsp/led.h"
#include "tim.h"
#include "usart.h"
#include "../../bsp/can_ex.h"
#include "../../bsp/motor.h"
#include "../../bsp/pid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t CAN1_Data[8] = {0};
uint8_t CAN2_Data[8] = {0};

float set_speed[4];
float set_angle[4];

enum __mode_t{
  speed_control = 0,
  angle_control
}control_mode;

extern CAN_FilterTypeDef can1_filter;
extern CAN_FilterTypeDef can2_filter;

extern CAN_TxHeaderTypeDef tx1_hdr;
extern CAN_RxHeaderTypeDef rx1_hdr;

extern CAN_TxHeaderTypeDef tx2_hdr;
extern CAN_RxHeaderTypeDef rx2_hdr;

extern uint32_t* CAN1_MailBox;
extern uint32_t* CAN2_MailBox;
/* USER CODE END Variables */
/* Definitions for can1_Task */
osThreadId_t can1_TaskHandle;
const osThreadAttr_t can1_Task_attributes = {
  .name = "can1_Task",
  .stack_size = 640 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uart6_Task */
osThreadId_t uart6_TaskHandle;
const osThreadAttr_t uart6_Task_attributes = {
  .name = "uart6_Task",
  .stack_size = 640 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for light_Task */
osThreadId_t light_TaskHandle;
const osThreadAttr_t light_Task_attributes = {
  .name = "light_Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Start_can1_Task(void *argument);
void Start_uart6_Task(void *argument);
void Start_light_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of can1_Task */
  can1_TaskHandle = osThreadNew(Start_can1_Task, NULL, &can1_Task_attributes);

  /* creation of uart6_Task */
  uart6_TaskHandle = osThreadNew(Start_uart6_Task, NULL, &uart6_Task_attributes);

  /* creation of light_Task */
  light_TaskHandle = osThreadNew(Start_light_Task, NULL, &light_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Start_can1_Task */
/**
  * @brief  Function implementing the can1_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_can1_Task */
void Start_can1_Task(void *argument)
{
  /* USER CODE BEGIN Start_can1_Task */

  pid_t pid_speed[4];		   //电机速度PID环（can be an array if it has multiple motor）,保留数组格式，实际只用一个
  pid_t pid_angle[4];
  float set_speed_temp;			   //加减速时的临时设定速度
  int16_t delta;					   //设定速度与实际速度的差值
  int16_t max_speed_change = 1000;   //电机单次最大变化速度，加减速用
  // 500经测试差不多是最大加速区间，即从零打到最大速度不异常的最大值


  for (char i = 0; i < 4; i++)
  {
    PID_struct_init(&pid_speed[i], POSITION_PID, 16384, 16384, 1.5f, 0.1f, 0.0f); //4 motos angular rate closeloop.
    pid_speed[i].deadband = 50;
  }
  for (char i = 0; i < 4; i++) {
    PID_struct_init(&pid_angle[i], POSITION_PID, 16384, 16384, 1.5f, 0.1f, 0.0f);
    pid_angle[i].deadband = 50;
  }

  /* Infinite loop */
  for(;;)
  {


    while (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) == 0);
    // 判断FIFO0是否有东西进来
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx1_hdr, CAN1_Data);

    switch (control_mode) {

      case speed_control: {
        /****************speed control***********************/
        uint8_t num = rx1_hdr.StdId - 0x201;
        switch(rx1_hdr.StdId){
          case 0x201:
          case 0x202:
          case 0x203:
          case 0x204:
            if (moto_chassis[num].msg_cnt <= 50) {  //make little change, prohibit re-offset
              moto_chassis[num].msg_cnt++;
              get_moto_offset(&moto_chassis[num], CAN1_Data);
            }
            else get_moto_measure(&moto_chassis[num], CAN1_Data, rx1_hdr);
          break;
          default:break;
        }


        //PID计算输出，写5 多算了一个，避免最后一个电机不计算，这里是一个未知bug
        for (char i = 0; i < 5; i++)
        {
          //PID计算

          // 无加减速
          //pid_calc(&pid_speed[i], (float)moto_chassis[i].speed_rpm, set_speed[i]);

          //加减速
          delta = (int16_t)set_speed[i] - moto_chassis[i].speed_rpm;
          if (delta > max_speed_change)
            set_speed_temp = (float)(moto_chassis[i].speed_rpm + max_speed_change);
          else if (delta < -max_speed_change)
            set_speed_temp = (float)(moto_chassis[i].speed_rpm - max_speed_change);
          else
            set_speed_temp = set_speed[i];
          pid_calc(&pid_speed[i], (float)moto_chassis[i].speed_rpm, set_speed_temp);
        }

      }
      break;


      case angle_control: {
        /************************angle control****************************/
        uint8_t num = rx1_hdr.StdId - 0x201;
        switch(rx1_hdr.StdId){
          case 0x201:
          case 0x202:
          case 0x203:
          case 0x204:
            if (moto_chassis[num].msg_cnt <= 50) {  //make little change, forbidden re-offset
              moto_chassis[num].msg_cnt++;
              get_moto_offset(&moto_chassis[num], CAN1_Data);
            }
            else get_total_angle(&moto_chassis[num]);
          break;
          default:break;
        }



        for (char i = 0; i < 5; i++) {
          pid_calc(&pid_angle[i], moto_chassis[i].total_angle,set_angle[i]);
        }


      }
      break;
      default:break;
    }


    /******************sent message************************/
    set_moto_current(CAN1_Data, pid_speed[0].pos_out,
             pid_speed[1].pos_out,
             pid_speed[2].pos_out,
             pid_speed[3].pos_out);




    // 获取FIFO0的内容，存入Data之中，帧格式存入rec_msg_hdr之中

    HAL_CAN_AddTxMessage(&hcan1,&tx1_hdr,CAN1_Data,CAN1_MailBox);

  }
  /* USER CODE END Start_can1_Task */
}

/* USER CODE BEGIN Header_Start_uart6_Task */
/**
* @brief Function implementing the uart6_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_uart6_Task */
void Start_uart6_Task(void *argument)
{
  /* USER CODE BEGIN Start_uart6_Task */
  uint8_t Data[4];
  /* Infinite loop */
  for(;;)
  {
    /*报文设计：
     *        头：
     *        0xaa
     *        数据：
     *        bite1：选择speed control或 angle control mode
     *        bite2、3: 指定电机
     *        bite4-16：电机角度/速度输出
     *        校验：
     *        校验和
     */
    HAL_UART_Receive(&huart6,Data,sizeof(Data),HAL_MAX_DELAY);
    if (Data[0] == 0xaa && (Data[0] + Data[1] + Data[2]) & 0xff == Data[3]) {
      control_mode = Data[1] & 0x80 ? speed_control : angle_control;
      static enum __mode_t last_mode = 2;
      if (last_mode != control_mode) {  //切换模式时使得moto_chassis重新初始化
        memset(moto_chassis, 0, sizeof(moto_chassis));
        last_mode = control_mode;
      }
      uint8_t motor_num = Data[1] & 0x60;
      switch(control_mode) {
        case speed_control: {
          set_speed[motor_num] = (Data[1] & 0x10 << 11) + (Data[1] & 0xf << 8)+ Data[2];  //第4位表示正负 总速度范围+-4095
        }
        break;
        case angle_control: {
          set_angle[motor_num] = (Data[1] & 0x1f << 8)+ Data[2];  //0-8191(360)
        }
        break;
      }
    }
    if (Data[0] == 0xaa && (Data[0] + Data[1] + Data[2]) & 0xff != Data[3]) {
      HAL_UART_Transmit(&huart6,"ERROR",5,HAL_MAX_DELAY);
    }
  }
  /* USER CODE END Start_uart6_Task */
}

/* USER CODE BEGIN Header_Start_light_Task */
/**
* @brief Function implementing the light_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_light_Task */
void Start_light_Task(void *argument)
{
  /* USER CODE BEGIN Start_light_Task */
  /* Infinite loop */
  for(;;)
  {
    BreathingLamp_proc(&htim4, TIM_CHANNEL_4);
    osDelay(1);
  }
  /* USER CODE END Start_light_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

