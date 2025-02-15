//
// Created by 19182 on 25-2-1.
//

#ifndef CAN_EX_H
#define CAN_EX_H

#include "can.h"



void CAN1_FilterTypeDef_Init(void);
void CAN2_FilterTypeDef_Init(void);

void CAN1_TxHeaderType_Init(uint32_t id_type,uint32_t basic_id,uint32_t ex_id);
void CAN2_TxHeaderType_Init(uint32_t id_type,uint32_t basic_id,uint32_t ex_id);

#endif //CAN_EX_H
