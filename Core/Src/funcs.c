//
// Created by volodic on 6/9/24.
//

#include "funcs.h"
#include "hx711.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "malloc.h"
#include "flash.h"


typedef struct Time{
    uint8_t sec;
    uint8_t min_S;
    uint8_t hour;
} Time;

typedef struct SensorsMes{
    double dist;
    int weight;
    Time meas_time;
} SensorsMes;

SensorsMes main_arr[100];



size_t string_length(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

static uint8_t counter = 0;

void save_measurement_to_flash(SensorsMes * meas_data) {
    static uint32_t flash_address = FLASH_USER_START_ADDR;
    Flash_Write(flash_address, (uint8_t*)meas_data, sizeof(*meas_data));
    flash_address += sizeof(*meas_data);
}

void read_all_measurements_from_flash(SensorsMes *data_array, size_t max_records) {
    size_t record_size = sizeof(SensorsMes);
    for (size_t i = 0; i < max_records; i++) {
        uint32_t address = FLASH_USER_START_ADDR + i * record_size;
        Flash_Read(address, (uint8_t*)&main_arr[i], record_size);
    }
}

uint32_t get_flash_end_addr() {
    return FLASH_USER_START_ADDR + (counter * sizeof(SensorsMes)) - 1;
}

void clean_flash(void) {
    Flash_Erase(FLASH_USER_START_ADDR, get_flash_end_addr() - FLASH_USER_START_ADDR + 1);
}

uint8_t string_check(const char *str_chk, const char * str_org){
    if(string_length(str_org) != string_length( str_chk)) return 0;
    size_t length = 0;
    while (str_org[length]!= '\0'){
        if(str_org[length] != str_chk[length]) return 0;
    }
    return 1;
}

uint8_t time_count = 0;
void send_data_huart(UART_HandleTypeDef *huart, RTC_HandleTypeDef * hrtc, HX711 * hx_obj, TIM_HandleTypeDef * htim, uint8_t temp_time_count){
    if(htim -> Instance == TIM3) {
        if (time_count < temp_time_count) {
            time_count += 1;
        }
        else {
            char msg[50];
            double dist = meas_dist();
            RTC_TimeTypeDef time;
            RTC_DateTypeDef date;
            HAL_RTC_GetTime(hrtc, &time, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(hrtc, &date, RTC_FORMAT_BIN);
            sprintf(msg, "%02d:%02d:%02d Distance: %.2f cm Weight: %f\r\n", time.Hours, time.Minutes, time.Seconds,
                    dist, HX711_GetWeight(hx_obj, 10));
            HAL_UART_Transmit(huart, (uint8_t *) msg, string_length(msg), HAL_MAX_DELAY);
            SensorsMes * meas = malloc(sizeof (SensorsMes));
            meas -> dist = dist;
            meas -> weight = (int) HX711_GetWeight(hx_obj, 10);
            meas -> meas_time.sec = time.Seconds, meas -> meas_time.min_S = time.Minutes, meas -> meas_time.hour = time.Hours;
            main_arr[counter].weight = (int) HX711_GetWeight(hx_obj, 10);
            main_arr[counter].dist = dist;
            main_arr[counter].meas_time.sec = time.Seconds;
            main_arr[counter].meas_time.min_S = time.Minutes;
            main_arr[counter].meas_time.hour = time.Hours;
            save_measurement_to_flash(meas);
            counter += 1;
            time_count = 0;
        }
    }

}
void tim_spec_del(uint16_t us){
    TIM2-> CNT = 0;
    while(TIM2 -> CNT < us);
}

uint16_t read_ultrasonic(){
    uint32_t local_time = 0;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    tim_spec_del(2);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_SET);
    tim_spec_del(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

    while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET);
    while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET){
        local_time++;
        tim_spec_del(1);
    }
    return local_time;
}

double meas_dist(){
    HAL_Delay(1000);
    return (read_ultrasonic() * 0.034/2);
}

SensorsMes * find_max(){
    SensorsMes * temp = malloc(sizeof (SensorsMes));
    temp -> dist = 0;
    temp -> weight = 0;
    for(int i = 0; i < counter; i++){
        if(main_arr[i].dist > temp -> dist){
            temp -> dist = main_arr[i].dist;
        }
        if(main_arr[i].weight > temp -> weight){
            temp-> weight = main_arr[i].weight;
        }
    }
    return temp;
}

SensorsMes * find_min(){
    SensorsMes * temp = malloc(sizeof (SensorsMes));
    temp -> dist = main_arr[0].dist;
    temp -> weight = main_arr[0].weight;
    for(int i = 0; i < counter; i++){
        if(main_arr[i].dist < temp -> dist){
            temp -> dist = main_arr[i].dist;
        }
        if(main_arr[i].weight < temp -> weight) {
            temp->weight = main_arr[i].weight;
        }
    }
    return temp;
}

void menu(UART_HandleTypeDef *huart){
    uint8_t buffer[40];
    HAL_UART_Receive_IT(huart, (uint8_t*)buffer, 1);
    if(buffer[0] == 's'){
       //read_all_measurements_from_flash(main_arr, counter);
        SensorsMes * max_data = find_max();
        char msg[10];
        sprintf(msg, "Maximum distance: %.2f cm Weight: %d\r\n",max_data->dist, max_data->weight);
        HAL_UART_Transmit(huart, (uint8_t *) msg, string_length(msg), HAL_MAX_DELAY);
        free(max_data);
    }
    else if(buffer[0] == 'm'){
        //read_all_measurements_from_flash(main_arr, counter);
        SensorsMes * min_data = find_min();
        char msg[10];
        sprintf(msg, "Minimum distance: %.2f cm Weight: %d\r\n", min_data->dist, min_data->weight);
        HAL_UART_Transmit(huart, (uint8_t *) msg, string_length(msg), HAL_MAX_DELAY);
        free(min_data);
    }
    else if(buffer[0] == 'c'){
        clean_flash();
        for(int i = 0; i< counter; i++){
        free(main_arr + i);
        }
    }
}