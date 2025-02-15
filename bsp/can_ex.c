//
// Created by 19182 on 25-2-1.
//

#include "can_ex.h"

CAN_FilterTypeDef can1_filter;
CAN_FilterTypeDef can2_filter;

CAN_TxHeaderTypeDef tx1_hdr;
CAN_RxHeaderTypeDef rx1_hdr;

CAN_TxHeaderTypeDef tx2_hdr;
CAN_RxHeaderTypeDef rx2_hdr;

uint32_t* CAN1_MailBox;
uint32_t* CAN2_MailBox;

void CAN1_FilterTypeDef_Init(void) {
    can1_filter.FilterIdHigh = 0X0000; //32位ID
    can1_filter.FilterIdLow = 0X0000;
    can1_filter.FilterMaskIdHigh = 0X0000; //32位MASK
    can1_filter.FilterMaskIdLow = 0X0000;
    can1_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器0关联到FIFO0
    can1_filter.FilterBank = 0;
    can1_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can1_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can1_filter.FilterActivation = ENABLE; //激活滤波器0

    if (HAL_CAN_ConfigFilter(&hcan1, &can1_filter) != HAL_OK) {
        Error_Handler();
    }
}
void CAN2_FilterTypeDef_Init(void) {
    can2_filter.FilterIdHigh = 0X0000; //32位ID
    can2_filter.FilterIdLow = 0X0000;
    can2_filter.FilterMaskIdHigh = 0X0000; //32位MASK
    can2_filter.FilterMaskIdLow = 0X0000;
    can2_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器0关联到FIFO0
    can2_filter.FilterBank = 0;
    can2_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can2_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can2_filter.FilterActivation = ENABLE; //激活滤波器0

    if (HAL_CAN_ConfigFilter(&hcan2, &can2_filter) != HAL_OK) {
        Error_Handler();
    }

}


/**
 * @brief init send_msg_hdr
 * @param id_type:CAN type:
 *                - CAN_ID_STD
 *                - CAN_ID_EXT
 * @param basic_id:evaluable when std
 * @param ex_id:evaluable when ext
 */
void CAN1_TxHeaderType_Init(uint32_t id_type,uint32_t basic_id,uint32_t ex_id)
{
    tx1_hdr.StdId = basic_id;
    tx1_hdr.ExtId = ex_id;
    tx1_hdr.IDE = id_type;
    tx1_hdr.RTR = CAN_RTR_DATA;
    tx1_hdr.DLC = 8;
    tx1_hdr.TransmitGlobalTime = DISABLE;
}
void CAN2_TxHeaderType_Init(uint32_t id_type,uint32_t basic_id,uint32_t ex_id) {
    tx2_hdr.StdId = basic_id;
    tx2_hdr.ExtId = ex_id;
    tx2_hdr.IDE = id_type;
    tx2_hdr.RTR = CAN_RTR_DATA;
    tx2_hdr.DLC = 8;
    tx2_hdr.TransmitGlobalTime = DISABLE;
}



