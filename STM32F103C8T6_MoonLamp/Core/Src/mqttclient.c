#include <mqttclient.h>
#include "ESP8266.h"
#include "MQTTPacket.h"

uint32_t mqtt_msgId = 0;

int mqtt_buflen = MQTT_PacketBuffSize;
uint8_t mqtt_PacketBuf[MQTT_PacketBuffSize];

int mqtt_transport_open(uint8_t *host, int port) {
	return 1;
}
int mqtt_transport_close(int sock) {
	return 0;
}

void mqtt_PublishRoutine()
{
	uint8_t fail_count = 0;
	uint8_t connack_rc = 0;
	const uint8_t fail_limit = 10;

	mqtt_buflen = sizeof(mqtt_PacketBuf);

	MQTTPacket_connectData ConnectData = MQTTPacket_connectData_initializer;
	ConnectData.clientID.cstring = MQTT_CLIENTID;
	ConnectData.username.cstring = MQTT_USERNAME;
	ConnectData.password.cstring = MQTT_PASSWORD;
	ConnectData.keepAliveInterval = MQTT_KeepAliveInterval;
	ConnectData.MQTTVersion = 4;
	ConnectData.cleansession = 1;

	// Use connack_rc as command length here
	connack_rc = MQTTSerialize_connect(mqtt_PacketBuf, mqtt_buflen,
			&ConnectData); // build connect packet


	ESP_Clear_Buffer();
	messageHandlerFlag = 0;
	while(1)
	{
		ESP_Open_Connection();
		while(messageHandlerFlag == 0);
		if(string_contains((char*)buffer, "busy", buffer_index) != -1
		|| string_contains((char*)buffer, "ERROR", buffer_index) != -1)
		{
			fail_count++;
			if(fail_count > fail_limit)
				return;//NVIC_SystemReset();

			ESP_Clear_Buffer();
			messageHandlerFlag = 0;
		}else
		{
			ESP_Clear_Buffer();
			messageHandlerFlag = 0;
			fail_count = 0;
			IWDG->KR = 0xAAAA;//Reset WatchDog
			break;//next step if succeded
		}
	}

	while(1)
	{
		send_MQTT_packet(&mqtt_PacketBuf[0], connack_rc);
		while(messageHandlerFlag == 0);
		if(!(string_contains((char*)buffer, "SEND OK\r\n", buffer_index) != -1))
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
			break;//next step if succeeded
		}
	}

	//Publish Start

	char mqtt_buffer[100];
	char status[20];
	memset(status, 0, 20);
	if(motor_status == 1)
	{
		strcat(status, "Rotating");
	}else
	{
		strcat(status, "Stopped");
	}
	uint8_t len = mqtt_PostMessage(mqtt_buffer, 100, "moon/motor", status);
	while(1)
	{
		send_MQTT_packet((uint8_t*)&mqtt_buffer[0], len);
		while(messageHandlerFlag == 0);
		if(!(string_contains((char*)buffer, "SEND OK\r\n", buffer_index) != -1))
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
			break;//next step if succeeded
		}
	}

	memset(status, 0, 20);
	if(lamp_status == 1)
	{
		strcat(status, "On");
	}else
	{
		strcat(status, "Off");
	}
	len = mqtt_PostMessage(mqtt_buffer, 100, "moon/light", status);
	while(1)
	{
		send_MQTT_packet((uint8_t*)&mqtt_buffer[0], len);
		while(messageHandlerFlag == 0);
		if(!(string_contains((char*)buffer, "SEND OK\r\n", buffer_index) != -1))
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
			break;//next step if succeeded
		}
	}

	//Publish End

	while(1)
	{
		mqtt_DisconnectServer();
		while(messageHandlerFlag == 0);
		if(!(string_contains((char*)buffer, "OK\r\n", buffer_index) != -1))
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
			break;//next step if succeeded
		}
	}

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
			break;//next step if succeeded
		}
	}
}

void mqtt_DisconnectServer()
{

	uint8_t connack_rc = 0;

	mqtt_buflen = sizeof(mqtt_PacketBuf);

	// Use connack_rc as command length here
	connack_rc = MQTTSerialize_disconnect(mqtt_PacketBuf, mqtt_buflen); // build connect packet
	send_MQTT_packet(&mqtt_PacketBuf[0], connack_rc);
}

int16_t mqtt_PostMessage(char mqtt_buffer[], uint8_t bufferLength, char topicName[], char value[])
{
	uint8_t length = strlen(topicName) + strlen(value) + 4;

	if(length > bufferLength)
		return -1;

	memset(mqtt_buffer, 0, bufferLength);
	mqtt_buffer[0] = 49;
	mqtt_buffer[1] = strlen(topicName) + strlen(value) + 2;
	mqtt_buffer[2] = 0;
	mqtt_buffer[3] = strlen(topicName);
	strcpy(&mqtt_buffer[4], topicName);
	strcat(&mqtt_buffer[4], value);
	return length;
}
