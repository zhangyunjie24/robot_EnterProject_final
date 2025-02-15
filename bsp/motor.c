//
// Created by 19182 on 25-2-2.
//

#include "motor.h"

moto_measure_t moto_chassis[4] = {0};//2 chassis moto
moto_measure_t moto_info;



/*******************************************************************************************
  * @Func			void get_moto_measure(moto_measure_t *ptr, CAN_HandleTypeDef* hcan)
  * @Brief    接收云台电机,3510电机通过CAN发过来的信息
  * @Param
  * @Retval		None
  * @Date     2015/11/24
 *******************************************************************************************/
uint8_t get_moto_measure(moto_measure_t *ptr, uint8_t CAN_Data[], CAN_RxHeaderTypeDef rx_hdr)
{
    //	u32  sum=0;
    //	u8	 i = FILTER_BUF_LEN;

    /*BUG!!! dont use this para code*/
    //	ptr->angle_buf[ptr->buf_idx] = (uint16_t)(hcan->pRxMsg->Data[0]<<8 | hcan->pRxMsg->Data[1]) ;
    //	ptr->buf_idx = ptr->buf_idx++ > FILTER_BUF_LEN ? 0 : ptr->buf_idx;
    //	while(i){
    //		sum += ptr->angle_buf[--i];
    //	}
    //	ptr->fited_angle = sum / FILTER_BUF_LEN;
    ptr->last_angle = ptr->angle;
    ptr->angle = (uint16_t)(CAN_Data[0]<<8 | CAN_Data[1]) ;
    ptr->real_current  = (int16_t)(CAN_Data[2]<<8 | CAN_Data[3]);
    ptr->speed_rpm = ptr->real_current;	//这里是因为两种电调对应位不一样的信息
    ptr->given_current = (int16_t)(CAN_Data[4]<<8 | CAN_Data[5])/-5;
    ptr->hall = CAN_Data[6];
    if(ptr->angle - ptr->last_angle > 4096)
        ptr->round_cnt --;
    else if (ptr->angle - ptr->last_angle < -4096)
        ptr->round_cnt ++;
    ptr->total_angle = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
    return rx_hdr.DLC;
}

/*this function should be called after system+can init */
void get_moto_offset(moto_measure_t *ptr, uint8_t CAN_Data[])
{
    ptr->angle = (uint16_t)(CAN_Data[0]<<8 | CAN_Data[1]) ;
    ptr->offset_angle = ptr->angle;
}

#define ABS(x)	( (x>0) ? (x) : (-x) )

/**
*@bref 电机上电角度=0， 之后用这个函数更新3510电机的相对开机后（为0）的相对角度。
    */
void get_total_angle(moto_measure_t *p){

    int res1, res2, delta;
    if(p->angle < p->last_angle){			//可能的情况
        res1 = p->angle + 8192 - p->last_angle;	//正转，delta=+
        res2 = p->angle - p->last_angle;				//反转	delta=-
    }else{	//angle > last
        res1 = p->angle - 8192 - p->last_angle ;//反转	delta -
        res2 = p->angle - p->last_angle;				//正转	delta +
    }
    //不管正反转，肯定是转的角度小的那个是真的
    if(ABS(res1)<ABS(res2))
        delta = res1;
    else
        delta = res2;

    p->total_angle += delta;
    p->last_angle = p->angle;
}

//have to transmit can_data by yourself
void set_moto_current(uint8_t CAN_Data[], /*CAN_HandleTypeDef* hcan, s16 SID,*/ s16 iq1, s16 iq2, s16 iq3, s16 iq4) {
    // hcan->pTxMsg->StdId = SID;
    // hcan->pTxMsg->IDE = CAN_ID_STD;
    // hcan->pTxMsg->RTR = CAN_RTR_DATA;
    // hcan->pTxMsg->DLC = 0x08;
    CAN_Data[0] = iq1 >> 8;
    CAN_Data[1] = iq1;
    CAN_Data[2] = iq2 >> 8;
    CAN_Data[3] = iq2;
    CAN_Data[4] = iq3 >> 8;
    CAN_Data[5] = iq3;
    CAN_Data[6] = iq4 >> 8;
    CAN_Data[7] = iq4;

}