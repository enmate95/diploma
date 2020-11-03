/*
 * ESP32.cpp
 *
 *  Created on: 11 Oct 2020
 *      Author: horny
 */

#include "ESP32.h"
#include "DataManager.h"

void WifiDevice::ESP32::init(const Container & data) {
	mode = DataManager<WifiDevice::ESPMode_t>::convertFromContainer(data);
}

void WifiDevice::ESP32::setParameter(int param, const Container & value) {
	switch(param) {
		case ESP_DEVICE_PARAMS: {
			params = &DataManager<WifiDevice::ESPParams>::convertFromContainer(value);
		}
		break;

		case ESP_ROLE: {
			role =DataManager<WifiDevice::ESPRole_t>::convertFromContainer(value);
		}
		break;
	}
}

Container WifiDevice::ESP32::getParameter(int param) {
	switch(param) {
		case ESP_DEVICE_PARAMS: {
			return DataManager<WifiDevice::ESPParams>::convertFromData(*params);
		}
		break;

		case ESP_ROLE: {
			return DataManager<WifiDevice::ESPRole_t>::convertFromData(role);
		}
		break;

		default:
			Container con;
			return con;
		break;
	}
}

void WifiDevice::ESP32::setCallback(int type, const Container & value) {
	switch(type) {
	case ESP_WIFI_CONNECTION_COMPLETE_CB: {
		ESPWifiConnectionCompletedCallback = DataManager<WifiDevice::ESPWifiConnectionCompletedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_WIFI_DISCONNECTION_COMPLETE_CB: {
		ESPWifiDisconnectionCompletedCallback = DataManager<WifiDevice::ESPWifiDisconnectionCompletedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_DATA_RECEIVED_CB: {
		ESPDataReceivedEventCallback = DataManager<WifiDevice::ESPDataReceivedEventCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_CONNECTED_TO_TCP_SERVER_CB: {
		ESPConnectedToTCPServerCallback = DataManager<WifiDevice::ESPConnectedToTCPServerCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_DISCONNECTED_FROM_TCP_SERVER_CB: {
		ESPDisconnectedFromTCPServerCallback = DataManager<WifiDevice::ESPDisconnectedFromTCPServerCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_GET_IP_CB: {
		ESPGetIPCallback = DataManager<WifiDevice::ESPGetIPCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_TCP_CLIENT_CONNECTED_CB: {
		ESPTCPClientConnectedCallback = DataManager<WifiDevice::ESPTCPClientConnectedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_TCP_CLIENT_DISCONNECTED_CB: {
		ESPTCPClientDisonnectedCallback = DataManager<WifiDevice::ESPTCPClientDisonnectedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_STATUS_OK_CB: {
		ESPIsStatusOKCallback = DataManager<WifiDevice::ESPIsStatusOKCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_WIFI_IS_CONNECTED_CB: {
		ESPWifiisConnectedCallback = DataManager<WifiDevice::ESPWifiisConnectedCb_t>::convertFromContainer(value);
	}
	break;
	}
}

void WifiDevice::ESP32::start() {
	serial.startDMA();
}

void WifiDevice::ESP32::stop() {
	serial.stopDMA();
}

void WifiDevice::ESP32::callHandler() {
	if(serial.uartHandleFLags()) {
		if(serial.setLength()) {
			flag = true;
		}
	}
}

void WifiDevice::ESP32::ClearBuff() {
	for(uint32_t i = 0; i < raw.length;i++)
		raw.data[i] = 0;
	raw.length = 0;
}

void WifiDevice::ESP32::process() {
	if(flag) {
		serial.copyBuffer(&raw);
		switch(protocol) {
		case TCP:
			if(role == TCP_CLIENT) {
				HandleTCPClient();
			}
			else {
				HandleTCPServer();
			}
		break;

		case UDP:
			HandleUDP();
		break;
		}
	}
}

void WifiDevice::ESP32::HandleTCPServer() {
	bool sent = false;
	char* ptr;
	switch(state) {
	case ESPCommandState::IDLE:
		ptr = strstr((char*)raw.data,"+IPD");
		if(ptr != nullptr) {
			int length = atoi(ptr + 7);
			int client = atoi(ptr + 5);
			while(*ptr != ':') {
				ptr++;
			}
			ptr++;
			if(ESPDataReceivedEventCallback != nullptr)
				ESPDataReceivedEventCallback(ptr,length,client);
			ProcessDone();
			state = ESPCommandState::IDLE;
		}

		ptr = strstr((char*)raw.data,",CLOSED");
		if(ptr != nullptr) {
			if(ESPTCPClientDisonnectedCallback != nullptr)
				ESPTCPClientDisonnectedCallback(ptr--);
			ProcessDone();
			state = ESPCommandState::IDLE;
		}

		ptr = strstr((char*)raw.data,",CONNECT");
		if(ptr != nullptr) {
			if(ESPTCPClientConnectedCallback != nullptr)
				ESPTCPClientConnectedCallback(ptr--);
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;

	case ESPCommandState::SEND:
		if(strstr((char*)raw.data,"SEND OK") != nullptr){
			ProcessDone();
			state = ESPCommandState::IDLE;
			sent = false;
		}
		else if((strstr((char*)raw.data,">") != nullptr) && (!sent)) {
			serial.send((uint8_t*)toSend->getData(),toSend->getLength());
			sent = true;
		}
	break;

	case ESPCommandState::CONNECT_WIFI:
		if(strstr((char*)raw.data,"OK") != nullptr) {
			if(ESPWifiConnectionCompletedCallback != nullptr)
				ESPWifiConnectionCompletedCallback();
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;

	case ESPCommandState::DISCONNECT_WIFI:
		if(strstr((char*)raw.data,"WIFI DISCONNECT") != nullptr) {
			if(ESPWifiDisconnectionCompletedCallback != nullptr)
				ESPWifiDisconnectionCompletedCallback();
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;

	case ESPCommandState::OPEN_TCP_SERVER:
		if(strstr((char*)raw.data,"OK") != nullptr) {
			if(ESPTCPServerOpenedCallback != nullptr)
				ESPTCPServerOpenedCallback();
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;

	case ESPCommandState::CLOSE_TCP_SERVER:
		if(strstr((char*)raw.data,"OK") != nullptr) {
			if(ESPTCPServerClosedCallback != nullptr)
				ESPTCPServerClosedCallback();
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;


	case ESPCommandState::GET_IP:
		ptr = strstr((char*)raw.data,"STAIP");
		if(ptr != nullptr) {
			ptr += 7;
			char* start = ptr;
			char ip[20];
			int length = 0;
			while(IsNumber(*ptr) || (*ptr == '.')) {
				ptr++;
				length++;
			}
			memcpy(ip,start,length);
			params->setIP(ip);
			if(ESPGetIPCallback != nullptr)
				ESPGetIPCallback(params->getIP());
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;


	case ESPCommandState::GET_WIFI_STATUS:
		ptr = strstr((char*)raw.data,"STATUS:");
		if(ptr != nullptr) {
			ptr += 7;
			int status = atoi(ptr);

			if(status < 5) {
				if(ESPWifiisConnectedCallback != nullptr)
					ESPWifiisConnectedCallback(true);
			}
			else if(status == 5){
				if(ESPWifiisConnectedCallback != nullptr)
					ESPWifiisConnectedCallback(false);
			}
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;

	default:
		if(strstr((char*)raw.data,"OK") != nullptr) {
			if(ESPIsStatusOKCallback != nullptr)
				ESPIsStatusOKCallback(true);
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
		else if(strstr((char*)raw.data,"ERROR") != nullptr){
			if(ESPIsStatusOKCallback != nullptr)
				ESPIsStatusOKCallback(false);
			ProcessDone();
			state = ESPCommandState::IDLE;
		}
	break;
}
}

void WifiDevice::ESP32::HandleUDP() {

}

void WifiDevice::ESP32::HandleTCPClient() {
	char* ptr;
	bool sent = false;

	switch(state) {
		case ESPCommandState::IDLE:
			ptr = strstr((char*)raw.data,"+IPD");
			if(ptr != nullptr) {
				int length = atoi(ptr + 5);
				while(*ptr != ':') {
					ptr++;
				}
				ptr++;
				if(ESPDataReceivedEventCallback != nullptr)
					ESPDataReceivedEventCallback(ptr,length,0);
				ProcessDone();
				state = ESPCommandState::IDLE;
			}

			if(strstr((char*)raw.data,"CLOSED") != nullptr) {
				if(ESPDisconnectedFromTCPServerCallback != nullptr)
					ESPDisconnectedFromTCPServerCallback();
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;

		case ESPCommandState::SEND:
			if(strstr((char*)raw.data,"SEND OK") != nullptr){
				ProcessDone();
				state = ESPCommandState::IDLE;
				sent = false;
			}
			else if((strstr((char*)raw.data,">") != nullptr) && (!sent)) {
				serial.send((uint8_t*)toSend->getData(),toSend->getLength());
				sent = true;
			}
		break;

		case ESPCommandState::CONNECT_WIFI:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				if(ESPWifiConnectionCompletedCallback != nullptr)
					ESPWifiConnectionCompletedCallback();
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;

		case ESPCommandState::DISCONNECT_WIFI:
			if(strstr((char*)raw.data,"WIFI DISCONNECT") != nullptr) {
				if(ESPWifiDisconnectionCompletedCallback != nullptr)
					ESPWifiDisconnectionCompletedCallback();
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;

		case ESPCommandState::CONNECT_TO_TCP_SERVER:
			if(strstr((char*)raw.data,"CONNECT") != nullptr) {
				if(ESPConnectedToTCPServerCallback != nullptr)
					ESPConnectedToTCPServerCallback();
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;

		case ESPCommandState::DISCONNECT_FROM_TCP_SERVER:
			if(strstr((char*)raw.data,"CLOSED") != nullptr) {
				if(ESPDisconnectedFromTCPServerCallback != nullptr)
					ESPDisconnectedFromTCPServerCallback();
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;


		case ESPCommandState::GET_IP:
			ptr = strstr((char*)raw.data,"STAIP");
			if(ptr != nullptr) {
				ptr += 7;
				char* start = ptr;
				char ip[20];
				int length = 0;
				while(IsNumber(*ptr) || (*ptr == '.')) {
					ptr++;
					length++;
				}
				memcpy(ip,start,length);
				params->setIP(ip);
				if(ESPGetIPCallback != nullptr)
					ESPGetIPCallback(params->getIP());
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;


		case ESPCommandState::GET_WIFI_STATUS:
			ptr = strstr((char*)raw.data,"STATUS:");
			if(ptr != nullptr) {
				ptr += 7;
				int status = atoi(ptr);

				if(status < 5) {
					if(ESPWifiisConnectedCallback != nullptr)
						ESPWifiisConnectedCallback(true);
				}
				else if(status == 5){
					if(ESPWifiisConnectedCallback != nullptr)
						ESPWifiisConnectedCallback(false);
				}
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;

		default:
			if(strstr((char*)raw.data,"OK") != nullptr) {
				if(ESPIsStatusOKCallback != nullptr)
					ESPIsStatusOKCallback(true);
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
			else if(strstr((char*)raw.data,"ERROR") != nullptr){
				if(ESPIsStatusOKCallback != nullptr)
					ESPIsStatusOKCallback(false);
				ProcessDone();
				state = ESPCommandState::IDLE;
			}
		break;
	}
}

bool WifiDevice::ESP32::IsNumber(char c) {
	const char lookup[] = "0123456789";
	for(int i = 0; i < strlen(lookup); i++) {
		if(c == lookup[i])
			return true;
	}
	return false;
}

void WifiDevice::ESP32::ProcessDone() {
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
	busy = false;
}

void WifiDevice::ESP32::SetMode() {
	char toSend[20] = "AT+CWMODE=";
	char num[10] = "";
	switch(mode) {
		case AccesPoint:
				sprintf(num,"%d\r\n",2);
				strcat(toSend,num);
			break;

		case Station:
				sprintf(num,"%d\r\n",1);
				strcat(toSend,num);
			break;

		case ApAndStation:
				sprintf(num,"%d\r\n",3);
				strcat(toSend,num);
			break;
	}

	serial.send((uint8_t*)toSend, strlen(toSend));
}

void WifiDevice::ESP32::DisableEcho() {
	serial.send((uint8_t*)"ATE0\r\n", 6);
}

void WifiDevice::ESP32::ConnectToTCPServer() {
	//AT+CIPSTART="TCP","IP",port
	char* ServerIP = params->getServerIP();
	int port = params->getPort();
	char toSend[50] = "AT+CIPSTART=\"TCP\",\"";
	char num[10] = "";
	strcat(toSend,ServerIP);
	strcat(toSend,"\",");
	sprintf(num,"%d\r\n",port);
	strcat(toSend,num);
	serial.send((uint8_t*)toSend,strlen(toSend));
}

void WifiDevice::ESP32::ConnectWifi() {
	//AT+CWJAP="SSID","password"
	char toSend[50] = "AT+CWJAP=\"";
	char* passkey = params->getPasskey();
	char* ssid = params->getSSID();
	strcat(toSend,ssid);
	strcat(toSend,"\",\"");
	strcat(toSend,passkey);
	strcat(toSend,"\"\r\n");
	serial.send((uint8_t*)toSend,strlen(toSend));
}

void WifiDevice::ESP32::GetWifiConnectionStatus() {
	//AT+CWQAP
	serial.send((uint8_t*)"AT+CIPSTATUS\r\n",14);
}

void WifiDevice::ESP32::DisConnectWifi() {
	//AT+CWQAP
	serial.send((uint8_t*)"AT+CWQAP\r\n",10);
}

void WifiDevice::ESP32::DisConnectFromTCPServer() {
	//AT+CIPCLOSE
	serial.send((uint8_t*)"AT+CIPCLOSE\r\n",15);
}

void WifiDevice::ESP32::DisConnectFromTCPClient() {
	//AT+CIPCLOSE=client
	char msg[50] ="AT+CIPCLOSE=";
	char client[5];
	sprintf(client,"%d\r\n",toSend->getClient());
	strcat(msg,client);
	serial.send((uint8_t*)msg,strlen(msg));
}

void WifiDevice::ESP32::GetIP() {
	//AT+CIFSR
	serial.send((uint8_t*)"AT+CIFSR\r\n",10);
}

void WifiDevice::ESP32::EnableMultipleConnections() {
	//AT+CIPMUX=1
	serial.send((uint8_t*)"AT+CIPMUX=1\r\n",13);
}

void WifiDevice::ESP32::DisableMultipleConnections() {
	//AT+CIPMUX=0
	serial.send((uint8_t*)"AT+CIPMUX=0\r\n",13);
}

void WifiDevice::ESP32::OpenTCPServer() {
	char toSend[50] = "AT+CIPSERVER=1,";
	char num[10] = "";
	int port = params->getPort();
	sprintf(num,"%d\r\n",port);
	strcat(toSend,num);
	serial.send((uint8_t*)toSend,strlen(toSend));
}

void WifiDevice::ESP32::CloseTCPServer() {
	//AT+CIPCLOSE=0
	serial.send((uint8_t*)"AT+CIPSERVER=0\r\n",16);
}

bool WifiDevice::ESP32::send(const Container & data) {
	toSend = &DataManager<ESPData>::convertFromContainer(data);
	busy = true;
	char send[30] = "AT+CIPSEND=";
	char size[10] = "";
	char client[10] ="";
	sprintf(client,"%d",toSend->getClient());
	sprintf(size,"%d",toSend->getLength());
	state = ESPCommandState::SEND;

	if(protocol == TCP) {
		if(role == TCP_CLIENT) {
				strcat(send,size);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
		}
		else {
				strcat(send,client);
				strcat(send,",");
				strcat(send,size);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
		}
	}
	else {

	}

	return true;
}

bool WifiDevice::ESP32::sendCmd(int cmd) {
	busy=true;
	switch(cmd) {
		case ESP_DISABLE_ECHO:
			state = ESPCommandState::DISABLE_ECHO;
			DisableEcho();
		break;

		case ESP_SET_MODE:
			state = ESPCommandState::SET_MODE;
			SetMode();
		break;

		case ESP_CONNECT_WIFI:
			state = ESPCommandState::CONNECT_WIFI;
			ConnectWifi();
		break;

		case ESP_DISCONNECT_WIFI:
			state = ESPCommandState::DISCONNECT_WIFI;
			DisConnectWifi();
		break;

		case ESP_GET_IP:
			state = ESPCommandState::GET_IP;
			GetIP();
		break;

		case ESP_DISABLE_MULTIPLE_CONN:
			state = ESPCommandState::DISABLE_MULTIPLE_CONN;
			DisableMultipleConnections();
		break;

		case ESP_ENABLE_MULTIPLE_CONN:
			state = ESPCommandState::ENABLE_MULTIPLE_CONN;
			EnableMultipleConnections();
		break;

		case ESP_OPEN_TCP_SERVER:
			state = ESPCommandState::OPEN_TCP_SERVER;
			OpenTCPServer();
		break;

		case ESP_CLOSE_TCP_SERVER:
			state = ESPCommandState::CLOSE_TCP_SERVER;
			CloseTCPServer();
		break;

		case ESP_GET_WIFI_STATUS:
			state = ESPCommandState::GET_WIFI_STATUS;
			GetWifiConnectionStatus();
		break;

		case ESP_CONNECT_TO_TCP_SERVER:
			state = ESPCommandState::CONNECT_TO_TCP_SERVER;
			ConnectToTCPServer();
		break;

		case ESP_DISCONNECT_FROM_TCP_SERVER:
			state = ESPCommandState::DISCONNECT_FROM_TCP_SERVER;
			DisConnectFromTCPServer();
		break;

		case ESP_DISCONNECT_FROM_TCP_CLIENT:
			state = ESPCommandState::DISCONNECT_FROM_TCP_CLIENT;
			DisConnectFromTCPClient();
		break;
	}

	return true;
}

