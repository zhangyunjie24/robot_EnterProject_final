//
// Created by 19182 on 25-1-26.
//

#include "led.h"

extern uint32_t MilliSec_counter;
extern uint32_t one_tenSec_counter;
extern uint32_t Sec_counter;

/**@brief This function makes light breath, T should be set in free rtos.
  * @param  htim TIM handle.
  * @param  OutPutChannel TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
  *
  */

void BreathingLamp_proc(TIM_HandleTypeDef* htim, uint32_t OutPutChannel) {
    static uint32_t compete_counter = 0;
    static uint32_t compare = 0;
    static uint8_t state = 0;

    __HAL_TIM_SET_COMPARE(htim, OutPutChannel, compare);

    switch (state) {
    case 0:
      compare++;
      break;
    case 1:
      compare--;
      break;
    }

    if (compare <= 0 || compare >= 1000) {
      state = !state;
    }

}
