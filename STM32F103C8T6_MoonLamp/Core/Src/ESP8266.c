/*
 * ESP8266.c
 *
 *  Created on: Nov 17, 2021
 *      Author: vikto
 */


#include "ESP8266.h"

uint32_t seconds = 0;
uint8_t buffer[2000];
uint16_t buffer_index = 0, timeout = 0, messageHandlerFlag = 0, no_activity_counter = 0;
uint8_t lamp_status = 0, motor_status = 0;
uint8_t lamp_int_flag = 0, motor_int_flag = 0;
ESP_State esp_state = ESP_Disconnected;
uint8_t update_flag = 0;

void ESP_RESET()
{
	HAL_GPIO_WritePin(ESP_ENABLE_GPIO_Port, ESP_ENABLE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(ESP_ENABLE_GPIO_Port, ESP_ENABLE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_SET);
}

void ESP_Server_Init()
{
	ESP_RESET();
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWDHCP=1,1\r\n", strlen("AT+CWDHCP=1,1\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPMUX=1\r\n", strlen("AT+CIPMUX=1\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSERVER=1,80\r\n", strlen("AT+CIPSERVER=1,80\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	//Change your WiFi SSID credentials below
	HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
}

void ESP_Clear_Buffer()
{
	memset(buffer, 0, 2000);
	buffer_index = 0;
}

uint8_t string_compare(char array1[], char array2[], uint16_t length)
{
	 uint16_t comVAR=0, i;
	 for(i=0;i<length;i++)
	   	{
	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	   	}
	 if (comVAR==length)
		 	return 1;
	 else 	return 0;
}

int string_contains(char bufferArray[], char searchedString[], uint16_t length)
{
	uint8_t result=0;
	for(uint16_t i=0; i<length; i++)
	{
		result = string_compare(&bufferArray[i], &searchedString[0], strlen(searchedString));
		if(result == 1)
			return i;
	}
	return -1;
}

void messageHandler()
{
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	int position = 0;
	if((position = string_contains((char*)buffer, ",CONNECT\r\n", buffer_index)) != -1)
	{
		ESP_Clear_Buffer();
	}else if((position = string_contains((char*)buffer, "GET", buffer_index)) != -1)//send Web page HTML code
	{
		sendData();
	}else if((position = string_contains((char*)buffer, "LAMP:OFF,MOTOR:OFF", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:OFF,ROTATION:OFF", buffer_index)) != -1)
	{
		lamp_status = 0;
		motor_status = 0;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if((position = string_contains((char*)buffer, "LAMP:OFF,MOTOR:ON", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:OFF,ROTATION:ON", buffer_index)) != -1)
	{
		lamp_status = 0;
		motor_status = 1;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if((position = string_contains((char*)buffer, "LAMP:ON,MOTOR:OFF", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:ON,ROTATION:OFF", buffer_index)) != -1)
	{
		lamp_status = 1;
		motor_status = 0;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if((position = string_contains((char*)buffer, "LAMP:ON,MOTOR:ON", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:ON,ROTATION:ON", buffer_index)) != -1)
	{
		lamp_status = 1;
		motor_status = 1;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if((position = string_contains((char*)buffer, "LAMP:ON", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:ON", buffer_index)) != -1)
	{
		lamp_status = 1;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
	}else if((position = string_contains((char*)buffer, "LAMP:OFF", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "LIGHT:OFF", buffer_index)) != -1)
	{
		lamp_status = 0;
		update_flag = 1;
		HAL_GPIO_WritePin(LAMP_GPIO_Port, LAMP_Pin, lamp_status);
	}else if((position = string_contains((char*)buffer, "MOTOR:ON", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "ROTATION:ON", buffer_index)) != -1)
	{
		motor_status = 1;
		update_flag = 1;
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if((position = string_contains((char*)buffer, "MOTOR:OFF", buffer_index)) != -1
		  || (position = string_contains((char*)buffer, "ROTATION:OFF", buffer_index)) != -1)
	{
		motor_status = 0;
		update_flag = 1;
		HAL_GPIO_WritePin(MOTOR_GPIO_Port, MOTOR_Pin, motor_status);
	}else if(string_contains((char*)buffer, "CWJAP", buffer_index) != -1
			&& (string_contains((char*)buffer, "FAIL", buffer_index) != -1
			|| string_contains((char*)buffer, "No AP", buffer_index) != -1
			|| string_contains((char*)buffer, "WIFI DISCONNECT", buffer_index) != -1)
			&& !(string_contains((char*)buffer, "WIFI CONNECTED", buffer_index) != -1))
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
		esp_state = ESP_Disconnected;
	}else if(string_contains((char*)buffer, "WIFI CONNECTED", buffer_index) != -1)
	{
		esp_state = ESP_Connected;
		update_flag = 1;
	}else if(string_contains((char*)buffer, "boot mode:", buffer_index) != -1)
	{
		esp_state = ESP_Disconnected;
		ESP_Server_Init();
	}else if(string_contains((char*)buffer, "CWJAP", buffer_index) != -1//"AT+CWJAP?\r\n"
			&& (string_contains((char*)buffer, "OK", buffer_index) != -1)
			&& (string_contains((char*)buffer, "No AP", buffer_index) == -1))// does not contain "No AP"
	{
		esp_state = ESP_Connected;
		update_flag = 1;
	}
	ESP_Clear_Buffer();
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void sendData()//sends data compatible with a browser
{
	char outputString[300], cipsend[50], response[600];
	memset(outputString, 0, 300);
	memset(cipsend, 0, 50);
	memset(response, 0, 600);

	if(lamp_status == 0 && motor_status == 0)
		sprintf(outputString, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><title>Moon Lamp</title><meta http-equiv=\"refresh\" content=\"5\"></head><body><h1>Lamp: OFF</h1><h1>Motor: OFF</h1></body></html>");
	else if(lamp_status == 0 && motor_status == 1)
		sprintf(outputString, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><title>Moon Lamp</title><meta http-equiv=\"refresh\" content=\"5\"></head><body><h1>Lamp: OFF</h1><h1>Motor: ON</h1></body></html>");
	else if(lamp_status == 1 && motor_status == 0)
		sprintf(outputString, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><title>Moon Lamp</title><meta http-equiv=\"refresh\" content=\"5\"></head><body><h1>Lamp: ON</h1><h1>Motor: OFF</h1></body></html>");
	else
		sprintf(outputString, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><title>Moon Lamp</title><meta http-equiv=\"refresh\" content=\"5\"></head><body><h1>Lamp: ON</h1><h1>Motor: ON</h1></body></html>");
	sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\nContent-Type: text/html\r\n\r\n%s", strlen(outputString), outputString);
	sprintf(cipsend, "AT+CIPSEND=0,%i\r\n", strlen(response));

	HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 100);
	HAL_Delay(50);
	HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 100);
	HAL_Delay(150);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	uint8_t fail_count = 0;
	const uint8_t fail_limit = 5;
	while(1)
	{
		ESP_Close_Connection();
		while(messageHandlerFlag == 0);
		if(!(string_contains((char*)buffer, "CIPCLOSE=0\r\r\n", buffer_index) != -1))
		{
			fail_count++;
			if(fail_count > fail_limit)
				NVIC_SystemReset();

			ESP_Clear_Buffer();
			messageHandlerFlag = 0;
		}else
		{
			ESP_Clear_Buffer();
			messageHandlerFlag = 0;
			fail_count = 0;
			IWDG->KR = 0xAAAA;//Reset WatchDog
			__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
			break;//next step if succeeded
		}
	}
}

void postMessageAsString(char message[])
{
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_UART_Transmit(&huart1, (uint8_t*)ESP_Post, strlen(ESP_Post), 100);
	HAL_Delay(80);
	char cipsend[30];

	//send Message
	memset(cipsend, 0, 30);
	sprintf(cipsend, "AT+CIPSEND=0,%i\r\n", strlen(message)+2);
	HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 100);
	HAL_Delay(80);
	uint8_t length[2] = {0, strlen(message)};
	HAL_UART_Transmit(&huart1, length, 2, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 100);
	HAL_Delay(80);

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPCLOSE=0\r\n", strlen("AT+CIPCLOSE=0\r\n"), 100);
	HAL_Delay(80);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void send_MQTT_packet(uint8_t message[], uint16_t len)
{
	char cipsend[200];

	//send Message
	memset(cipsend, 0, 200);
	sprintf(cipsend, "AT+CIPSEND=0,%i\r\n", len);
	HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 100);
	HAL_Delay(80);
	HAL_UART_Transmit(&huart1, (uint8_t*)message, len, 100);
	HAL_Delay(80);
}

void ESP_Open_Connection()
{
	HAL_UART_Transmit(&huart1, (uint8_t*)ESP_Post, strlen(ESP_Post), 100);
    HAL_Delay(80);
}

void ESP_Close_Connection()
{
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPCLOSE=0\r\n", strlen("AT+CIPCLOSE=0\r\n"), 100);
}
