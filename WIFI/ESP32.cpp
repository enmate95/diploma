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

	case ESP_CONNECTION_OPENED_CB: {
		ESPConnectionOpenedCallback = DataManager<WifiDevice::ESPConnectionOpenedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_CONNECTION_CLOSED_CB: {
		ESPConnectionClosedCallback = DataManager<WifiDevice::ESPConnectionClosedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_GET_IP_CB: {
		ESPGetIPCallback = DataManager<WifiDevice::ESPGetIPCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_CLIENT_CONNECTED_CB: {
		ESPClientConnectedCallback = DataManager<WifiDevice::ESPClientConnectedCb_t>::convertFromContainer(value);
	}
	break;

	case ESP_TEST_CB: {
		ESPTestCallback = DataManager<WifiDevice::ESPTestCb_t>::convertFromContainer(value);
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
		char* ptr;

		ptr = strstr((char*)raw.data,"ERROR");
		if(ptr != nullptr) {
			HandleStatusERROR();
		}
		ptr = strstr((char*)raw.data,"OK");
		if(ptr != nullptr) {
			statusOk = true;
			HandleStatusOK();
		}

		ptr = strstr((char*)raw.data,"+IPD");
		if(ptr != nullptr) {
			HandleDataReception(ptr);
		}

		if(role == WIFI_CLIENT) {
			ptr = strstr((char*)raw.data,"CLOSED");
			HandleServerDisconnection();
		}
		else if(role == WIFI_SERVER) {
			ptr = strstr((char*)raw.data,",CONNECT");
			if(ptr != nullptr) {
				HandleClientConnection(ptr);
			}
			ptr = strstr((char*)raw.data,",CLOSED");
			if(ptr != nullptr) {
				HandleClientDisconnection(ptr);
			}
		}
	}
}

void WifiDevice::ESP32::HandleDataReception(char* ptr) {
	int length = atoi(ptr + 5);
	while(*ptr != ':') {
		ptr++;
	}
	ptr++;
	if(ESPDataReceivedEventCallback != nullptr)
		ESPDataReceivedEventCallback(ptr,length);
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
}

void WifiDevice::ESP32::HandleServerDisconnection() {
	if(ESPServerDisconnectedCallback != nullptr)
		ESPServerDisconnectedCallback();
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
}

void WifiDevice::ESP32::HandleStatusOK()  {
	if(strstr((char*)raw.data,"WIFI CONNECTED") != nullptr) {
		if(ESPWifiConnectionCompletedCallback != nullptr)
			ESPWifiConnectionCompletedCallback();
	}
	else if(strstr((char*)raw.data,"WIFI DISCONNECT") != nullptr) {
		if(ESPWifiDisconnectionCompletedCallback != nullptr)
			ESPWifiDisconnectionCompletedCallback();
	}
	else if(strstr((char*)raw.data,"CONNECT") != nullptr) {
		if(ESPConnectionOpenedCallback != nullptr)
			ESPConnectionOpenedCallback();
	}
	else if(strstr((char*)raw.data,"CLOSED") != nullptr) {
		if(ESPConnectionClosedCallback != nullptr)
			ESPConnectionClosedCallback();
	}
	else if(strstr((char*)raw.data,"STAIP") != nullptr) {
		char *ptr = strstr((char*)raw.data,"STAIP") + 7;
		params->setIP(ptr);
		if(ESPGetIPCallback != nullptr)
			ESPGetIPCallback(params->getIP());
	}
	else if(strstr((char*)raw.data,">") != nullptr) {
		char *data = toSend->getData();
		serial.send((uint8_t*)data,strlen(data));
	}
	else if((strstr((char*)raw.data,"STATUS:") != nullptr)) {
		char* ptr = strstr((char*)raw.data,"STATUS:") + 7;
		int status = atoi(ptr);

		if(status < 5) {
			if(ESPWifiisConnectedCallback != nullptr)
				ESPWifiisConnectedCallback(true);
		}
		else if(status == 5){
			if(ESPWifiisConnectedCallback != nullptr)
				ESPWifiisConnectedCallback(false);
		}
	}
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
	busy = false;
}

bool WifiDevice::ESP32::isStatusOk() {
	if(statusOk) {
		statusOk = false;
		return true;
	}
	return false;
}

void WifiDevice::ESP32::HandleStatusERROR() {
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
	busy = false;
}

void WifiDevice::ESP32::HandleClientDisconnection(char* ptr) {
	if(ESPClientConnectedCallback != nullptr)
		ESPClientConnectedCallback(atoi(ptr--));
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
}

void WifiDevice::ESP32::HandleClientConnection(char* ptr) {
	if(ESPClientConnectedCallback != nullptr)
		ESPClientConnectedCallback(atoi(ptr--));
	serial.stopDMA();
	serial.clearBuffer();
	serial.startDMA();
	ClearBuff();
	flag = false;
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
	serial.send((uint8_t*)"AT\r\n", 4);
}

void WifiDevice::ESP32::ConnectTCP() {
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

void WifiDevice::ESP32::DisConnectTCP() {
	//AT+CIPCLOSE=0
	serial.send((uint8_t*)"AT+CIPCLOSE\r\n",14);
}

void WifiDevice::ESP32::GetIP() {
	//AT+CIFSR
	serial.send((uint8_t*)"AT+CIFSR\r\n",10);
}

void WifiDevice::ESP32::EnableMultipleConnections() {
	//AT+CIPMUX
	serial.send((uint8_t*)"AT+CIPMUX=1\r\n",13);
}

void WifiDevice::ESP32::DisableMultipleConnections() {
	//AT+CIPMUX
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
	serial.send((uint8_t*)"AT+CIPCLOSE=0\r\n",15);
}

bool WifiDevice::ESP32::send(const Container & data) {
	toSend = &DataManager<ESPData>::convertFromContainer(data);
	busy = true;
	char send[30] = "AT+CIPSEND=";
	char sizestr[10] = "";
	int size = toSend->getLength();
	char client[10] ="";
	sprintf(client,"%d",toSend->getClient());
	sprintf(sizestr,"%d",size);


	switch(role) {
		case WIFI_CLIENT:
			if(protocol == TCP) {
				strcat(send,sizestr);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
			}
			else {
				//TODO:
			}
		break;

		case WIFI_SERVER:
			if(protocol == TCP) {
				strcat(send,client);
				strcat(send,",");
				strcat(send,sizestr);
				strcat(send,"\r\n");
				serial.send((uint8_t*)send,strlen(send));
			}
			else {
				//TODO:
			}
		break;

		case BLE_CENTRAL:
			//TODO:
			break;

		case BLE_PERIPHERAL:
			//TODO:
			break;
	}

	return true;
}

bool WifiDevice::ESP32::sendCmd(int cmd) {
	busy=true;
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

		case ESP_GET_WIFI_STATUS:
			GetWifiConnectionStatus();
		break;

		case ESP_CLOSE_TCP_SERVER:
			CloseTCPServer();
		break;

		case ESP_DISCONNECT_TCP:
			DisConnectTCP();
		break;

		case ESP_DISCONNECT_WIFI:
			DisConnectWifi();
		break;

	}

	return true;
}

