//
// Created by 19182 on 25-2-2.
//

#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include "can.h"
typedef int s16;

#define FILTER_BUF_LEN		5
/*电机的参数结构体*/
typedef struct{
    int16_t	 	speed_rpm;
    int16_t  	real_current;
    int16_t  	given_current;
    uint8_t  	hall;
    uint16_t 	angle;				//abs angle range:[0,8191]
    uint16_t 	last_angle;	//abs angle range:[0,8191]
    uint16_t	offset_angle;
    int32_t		round_cnt;
    int32_t		total_angle;
    uint8_t		buf_idx;
    uint16_t	angle_buf[FILTER_BUF_LEN];
    uint16_t	fited_angle;
    uint32_t	msg_cnt;
}moto_measure_t;


/* Extern  ------------------------------------------------------------------*/
extern moto_measure_t  moto_chassis[];
extern moto_measure_t moto_info;
extern uint8_t CAN1_Data[8];
extern uint8_t CAN2_Data[8];

uint8_t get_moto_measure(moto_measure_t *ptr, uint8_t CAN_Data[8], CAN_RxHeaderTypeDef rx_hdr);
void set_moto_current(uint8_t CAN_Data[],/*CAN_HandleTypeDef* hcan, s16 SID,*/ s16 iq1, s16 iq2, s16 iq3, s16 iq4);
void get_moto_offset(moto_measure_t *ptr, uint8_t CAN_Data[]);
void get_total_angle(moto_measure_t *p);

#endif //MOTOR_H
