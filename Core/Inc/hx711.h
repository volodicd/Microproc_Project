//
// Created by volodic on 6/9/24.
//
#include "stm32f4xx_hal.h"
#ifndef PROJECT_HX711_H
#define PROJECT_HX711_H


typedef struct {
    GPIO_TypeDef* dout_port;
    uint16_t dout_pin;
    GPIO_TypeDef* sck_port;
    uint16_t sck_pin;
    int32_t zeroOffset;
    double scaleFactor;
} HX711;

void HX711_Init(HX711* hx711, GPIO_TypeDef* dout_port, uint16_t dout_pin, GPIO_TypeDef* sck_port, uint16_t sck_pin);
int32_t HX711_Read(HX711* hx711);
void HX711_Tare(HX711* hx711, int times);
double HX711_GetWeight(HX711* hx711, int times);
#endif //PROJECT_HX711_H
