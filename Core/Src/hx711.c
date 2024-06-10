//
// Created by volodic on 6/9/24.
//

#include "hx711.h"

void HX711_Init(HX711* hx711, GPIO_TypeDef* dout_port, uint16_t dout_pin, GPIO_TypeDef* sck_port, uint16_t sck_pin) {
    hx711->dout_port = dout_port;
    hx711->dout_pin = dout_pin;
    hx711->sck_port = sck_port;
    hx711->sck_pin = sck_pin;
    hx711->zeroOffset = 0;
    hx711->scaleFactor = 1.0;
    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
}

int32_t HX711_Read(HX711* hx711) {
    while (HAL_GPIO_ReadPin(hx711->dout_port, hx711->dout_pin) == GPIO_PIN_SET);

    int32_t count = 0;
    for (int i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
        count = count << 1;
        HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);
        if (HAL_GPIO_ReadPin(hx711->dout_port, hx711->dout_pin) == GPIO_PIN_SET) {
            count++;
        }
    }

    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_SET);
    count = count ^ 0x800000;
    HAL_GPIO_WritePin(hx711->sck_port, hx711->sck_pin, GPIO_PIN_RESET);

    return count;
}

void HX711_Tare(HX711* hx711, int times) {
    int64_t sum = 0;
    for (int i = 0; i < times; i++) {
        sum += HX711_Read(hx711);
    }
    hx711->zeroOffset = sum / times;
}

double HX711_GetWeight(HX711* hx711, int times) {
    int64_t sum = 0;
    for (int i = 0; i < times; i++) {
        sum += HX711_Read(hx711);
    }
    int32_t value = sum / times;
    return (value - hx711->zeroOffset) / hx711->scaleFactor;
}