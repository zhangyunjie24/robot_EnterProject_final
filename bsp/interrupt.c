//
// Created by 19182 on 25-1-27.
//

#include "interrupt.h"
#include "tim.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

extern uint32_t adc_buffer_1[2];
extern uint32_t adc_buffer_2[1];
extern uint32_t adc_buffer_3[1];
extern uint8_t rx_buffer[10];

volatile uint8_t uart_busy = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    uint8_t text[10];
    if (hadc->Instance == ADC1) {
        //another method: Directly store the data in the adc_buffer into an array of uint8_t and transfer it
        //wait till not busy
        while(uart_busy){}
        uart_busy = 1;
        sprintf(text, "1%04o\r\n", adc_buffer_1[0]);
        HAL_UART_Transmit_DMA(&huart1, (uint8_t *)text, strlen(text));

        while(uart_busy){}
        uart_busy = 1;
        sprintf(text, "2%04o\r\n", adc_buffer_1[1]);
        HAL_UART_Transmit_DMA(&huart1, (uint8_t *)text, strlen(text));
    }
    if (hadc->Instance == ADC2) {
        while(uart_busy){}
        uart_busy = 1;
        sprintf(text, "3%04o\r\n", adc_buffer_2[0]);
        HAL_UART_Transmit_DMA(&huart1, (uint8_t *)text, strlen(text));
    }
    if (hadc->Instance == ADC3) {
        while(uart_busy){}
        uart_busy = 1;
        sprintf(text, "4%04o\r\n", adc_buffer_3[0]);
        HAL_UART_Transmit_DMA(&huart1, (uint8_t *)text, strlen(text));
    }
}

//read uart value & achieve software IC rising edge capture.
uint32_t TTL_frq[4];
__weak void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
    UNUSED(Size);   //use strlen to replace size.
    if (huart->Instance == USART1) {
        if (strlen(rx_buffer) != 7) return; //ignore error transmits

        //read text
        uint16_t adc_value = (rx_buffer[1] - '0') << 9 | (rx_buffer[2] - '0') << 6 | (rx_buffer[3] - '0') << 3 | (rx_buffer[4] - '0') << 0;

        static uint8_t previous_voltage[4] = {0};   //0 or 1

        uint8_t i = rx_buffer[0] - '1';

        //TTL standard voltage
        uint8_t current_voltage;    //0 or 1
        if (adc_value > 1638) { //higher than 2.0V
            current_voltage = 1;
        }
        else if (adc_value < 655) { //lower than 0.8V
            current_voltage = 0;
        }
        else return;  //ignore when voltage in middle

        //rising edge detect

        uint16_t temp = 0;
        if (previous_voltage[i] == 0 && current_voltage == 1) {
            temp = __HAL_TIM_GetCounter(&htim6);
            HAL_TIM_Base_Stop(&htim6);
            __HAL_TIM_SetCounter(&htim6, 0);
            HAL_TIM_Base_Start(&htim6);
        }

        if (temp != 0) {
            TTL_frq[i] = 1000000 / temp;

            UNUSED(TTL_frq[i]);  //TTL_frq[i] is unused, can be used in other logics
        }

        previous_voltage[i] = current_voltage;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        uart_busy = 0;
    }
}