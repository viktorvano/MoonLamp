/*
 * ESP8266.h
 *
 *  Created on: Nov 17, 2021
 *      Author: vikto
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WiFi_Credentials	"AT+CWJAP=\"WiFiSSID\",\"WiFiPASSWORD\"\r\n"

extern UART_HandleTypeDef huart1;

extern uint32_t seconds;
extern uint8_t buffer[2000];
extern uint16_t buffer_index, timeout, messageHandlerFlag, no_activity_counter;
extern uint8_t lamp_status, motor_status;
extern uint8_t lamp_int_flag, motor_int_flag;

void ESP_RESET();
void ESP_Server_Init();
void ESP_Clear_Buffer();
uint8_t string_compare(char array1[], char array2[], uint16_t length);
int string_contains(char bufferArray[], char searchedString[], uint16_t length);
void messageHandler();
void sendData();

#endif /* INC_ESP8266_H_ */
