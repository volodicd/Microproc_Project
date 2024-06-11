//
// Created by volodic on 6/9/24.
//

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "hx711.h"
#ifndef PROJECT_FUNCS_H
#define PROJECT_FUNCS_H
double meas_dist();
void menu(UART_HandleTypeDef *huart);
void send_data_huart(UART_HandleTypeDef *huart, RTC_HandleTypeDef * hrtc, HX711 * hx_obj, TIM_HandleTypeDef * htim, uint8_t time_count);
#endif //PROJECT_FUNCS_H
