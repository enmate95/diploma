/*
 * ESP32.cpp
 *
 *  Created on: 11 Oct 2020
 *      Author: horny
 */

#include "ESP32.h"
#include "DataManager.h"

void WifiDevice::ESP32::init(const Container & data) {
	DataManager<WifiDevice::ESPMode_t> dm;
	mode = dm.convertFromContainer(data);
}

void WifiDevice::ESP32::setParameter(int param, const Container & value) {
	switch(param) {
		case ESP_DEVICE_PARAMS: {
			DataManager<WifiDevice::ESPParams> dm;
			params = dm.convertFromContainer(value);
		}
		break;

		case ESP_MODE: {
			DataManager<WifiDevice::ESPMode_t> dm;
			mode = dm.convertFromContainer(value);
		}
		break;

		case ESP_ROLE: {
			DataManager<WifiDevice::ESPRole_t> dm;
			role = dm.convertFromContainer(value);
		}
		break;
	}
}

Container WifiDevice::ESP32::getParameter(int param) {
	switch(param) {
		case ESP_DEVICE_PARAMS: {
			DataManager<WifiDevice::ESPParams> dm;
			return dm.convertFromData(params);
		}
		break;

		case ESP_MODE: {
			DataManager<WifiDevice::ESPMode_t> dm;
			return dm.convertFromData(mode);
		}
		break;

		case ESP_ROLE: {
			DataManager<WifiDevice::ESPRole_t> dm;
			return dm.convertFromData(role);
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
		DataManager<WifiDevice::ESPWifiConnectionCompletedCb_t> dm;
		ESPWifiConnectionCompletedCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_WIFI_DISCONNECTION_COMPLETE_CB: {
		DataManager<WifiDevice::ESPWifiDisconnectionCompletedCb_t> dm;
		ESPWifiDisconnectionCompletedCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_DATA_RECEIVED_CB: {
		DataManager<WifiDevice::ESPDataReceivedEventCb_t> dm;
		ESPDataReceivedEventCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_CONNECTION_OPENED_CB: {
		DataManager<WifiDevice::ESPConnectionOpenedCb_t> dm;
		ESPConnectionOpenedCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_CONNECTION_CLOSED_CB: {
		DataManager<WifiDevice::ESPConnectionClosedCb_t> dm;
		ESPConnectionClosedCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_GET_IP_CB: {
		DataManager<WifiDevice::ESPGetIPCb_t> dm;
		ESPGetIPCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_CLIENT_CONNECTED_CB: {
		DataManager<WifiDevice::ESPClientConnectedCb_t> dm;
		ESPClientConnectedCallback = dm.convertFromContainer(value);
	}
	break;

	case ESP_TEST_CB: {
		DataManager<WifiDevice::ESPTestCb_t> dm;
		ESPTestCallback = dm.convertFromContainer(value);
	}
	break;

	}
}

void WifiDevice::ESP32::callHandler() {
	if(serial.uartHandleFLags()) {
		if(serial.setLength()) {
			flag = true;
		}
	}
}

void WifiDevice::ESP32::process() {
	if(flag) {
		flag = false;
		serial.copyBuffer(&raw);

		if(strstr((char*)raw.data,"WIFI CONNECTED") != nullptr) {
			busy = false;
			if(ESPWifiConnectionCompletedCallback != nullptr)
				ESPWifiConnectionCompletedCallback();
		}
		else if(strstr((char*)raw.data,"WIFI DISCONNECTED") != nullptr) {
			busy = false;
			if(ESPWifiDisconnectionCompletedCallback != nullptr)
				ESPWifiDisconnectionCompletedCallback();
		}
		else if(strstr((char*)raw.data,",CONNECT") != nullptr) {
			char *ptr = strstr((char*)raw.data,",CONNECT");
			busy = false;
			if(ESPClientConnectedCallback != nullptr)
				ESPClientConnectedCallback(atoi(ptr--));
		}
		else if(strstr((char*)raw.data,"CONNECT") != nullptr) {
			busy = false;
			if(ESPConnectionOpenedCallback != nullptr)
				ESPConnectionOpenedCallback();
		}
		else if(strstr((char*)raw.data,",CLOSED") != nullptr) {
			busy = false;
			if(ESPConnectionClosedCallback != nullptr)
				ESPConnectionClosedCallback();
		}
		else if(strstr((char*)raw.data,"STAIP") != nullptr) {
			char *ptr = strstr((char*)raw.data,"STAIP");
			ptr += 7;
			params.setIP(ptr);
			busy = false;
			if(ESPGetIPCallback != nullptr)
				ESPGetIPCallback(params.getIP());
		}
		else if(strstr((char*)raw.data,",>") != nullptr && test) {
			char *data = toSend.getData();
			serial.send((uint8_t*)data,strlen(data));
		}
		else if(strstr((char*)raw.data,",SEND OK") != nullptr && test) {
			busy = false;
		}
		else if(strstr((char*)raw.data,",OK") != nullptr && test) {
			busy = false;
			test = false;
			if(ESPTestCallback != nullptr)
				ESPTestCallback();
		}
	}
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

void WifiDevice::ESP32::Test() {
	test = true;
	serial.send((uint8_t*)"AT\r\n", 4);
}

void WifiDevice::ESP32::ConnectTCP() {
	//AT+CIPSTART="TCP","IP",port
	char* ServerIP = params.getIP();
	int port = params.getPort();
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
	char* passkey = params.getPasskey();
	char* ssid = params.getSSID();
	strcat(toSend,ssid);
	strcat(toSend,"\",\"");
	strcat(toSend,passkey);
	strcat(toSend,"\"\r\n");
	serial.send((uint8_t*)toSend,strlen(toSend));
}

void WifiDevice::ESP32::GetIP() {
	//AT+CIFSR
	serial.send((uint8_t*)"AT+CIFSR\r\n",10);
}

void WifiDevice::ESP32::EnableMultipleConnections() {
	//AT+CIPMUX
	serial.send((uint8_t*)"AT+CIPMUX=1\r\n",12);
}

void WifiDevice::ESP32::DisableMultipleConnections() {
	//AT+CIPMUX
	serial.send((uint8_t*)"AT+CIPMUX=0\r\n",12);
}

void WifiDevice::ESP32::OpenTCPServer() {
	char toSend[50] = "AT+CIPSERVER=1,";
	char num[10] = "";
	int port = params.getPort();
	sprintf(num,"%d\r\n",port);
	strcat(toSend,num);
	serial.send((uint8_t*)toSend,strlen(toSend));
}

void WifiDevice::ESP32::CloseTCPServer() {
	//AT+CIPCLOSE=0
	serial.send((uint8_t*)"AT+CIPCLOSE=0\r\n",15);
}


bool WifiDevice::ESP32::send(const Container & data) {
	DataManager<ESPData> dm;
	toSend = dm.convertFromContainer(data);
	char send[30] = "AT+CIPSEND=";
	char sizestr[10] = "";
	int size = strlen(toSend.getData());
	char client[10] ="";

	sprintf(client,"%d",toSend.getClient());
	sprintf(sizestr,"%d",size);

	switch(role) {
	case WIFI_CLIENT:
			strcat(send,sizestr);
			strcat(send,"\r\n");
			serial.send((uint8_t*)send,strlen(send));
		break;

	case WIFI_SERVER:
		strcat(send,client);
		strcat(send,",");
		strcat(send,sizestr);
		strcat(send,"\r\n");
		serial.send((uint8_t*)send,strlen(send));
		break;
	}

	return true;
}

bool WifiDevice::ESP32::sendCmd(int cmd) {
	switch(cmd) {
		case ESP_TEST:
			Test();
		break;

		case ESP_SET_MODE:
			SetMode();
		break;

		case ESP_CONNECT_TCP:
			ConnectTCP();
		break;

		case ESP_CONNECT_WIFI:
			ConnectWifi();
		break;

		case ESP_GET_IP:
			GetIP();
		break;

		case ESP_DISABLE_MULTIPLE_CONN:
			DisableMultipleConnections();
		break;

		case ESP_ENABLE_MULTIPLE_CONN:
			EnableMultipleConnections();
		break;

		case ESP_OPEN_TCP_SERVER:
			OpenTCPServer();
		break;

		case ESP_CLOSE_TCP_SERVER:
			CloseTCPServer();
		break;
	}

	return true;
}

