/*
 * ESPTests.cpp
 *
 *  Created on: 15 Oct 2020
 *      Author: horny
 */
#include "DataManager.h"
#include "cmsis_os.h"
#include "main.h"
#include "uartDevice.h"
#include "usart.h"
#include "ESP32.h"
#include "Defines.h"

UartDMA ESPSerial(&huart1);
WifiDevice::ESP32 esp(ESPSerial);
WifiDevice::ESPParams parameters;
WifiDevice::ESPData toSend;

#if ESP_TEST
RadioDevice* device;
#if WIFI_CLIENT_MODE

void WifiConnectionCompletedCb();
void WifiDisconnectionCompletedCb();
void TCPConnectionOpenedCb();
void DataReceivedCb(char* data, int size);
void TCPConnectionClosedCb();
void WifiIsConnectedCb(bool status);

WifiDevice::ESPWifiConnectionCompletedCb_t ConnectionComplete = WifiConnectionCompletedCb;
WifiDevice::ESPWifiDisconnectionCompletedCb_t DisconnectionComplete = WifiDisconnectionCompletedCb;
WifiDevice::ESPConnectionOpenedCb_t TCPConnection = TCPConnectionOpenedCb;
WifiDevice::ESPConnectionClosedCb_t TCPClosed = TCPConnectionClosedCb;
WifiDevice::ESPDataReceivedEventCb_t DataReceived = DataReceivedCb;
WifiDevice::ESPWifiisConnectedCb_t isConnected =WifiIsConnectedCb;

char *ptr = nullptr;
int length = 0;
bool connected;
bool TCPconnected = false;
bool received = false;
bool getStatus= true;

void WifiIsConnectedCb(bool status) {
	connected = status;
	getStatus = false;

}

void WifiConnectionCompletedCb() {
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	connected = true;
}

void WifiDisconnectionCompletedCb() {
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
	connected = false;
}

void TCPConnectionOpenedCb() {
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	TCPconnected = true;
}

void TCPConnectionClosedCb() {
	TCPconnected = false;
}

void DataReceivedCb(char* data, int size) {
	received = true;
	ptr = data;
	length = size;
}

extern "C" {

void ESPUartITHandler() {
	static_cast<WifiDevice::ESP32*>(device)->callHandler();
}


enum class AppState {
	Run,
	WifiDisconnected,
	WifiConnected,
	Done,
};

AppState state = AppState::WifiDisconnected;

bool processTimeExpired(int count) {
	static int start;
	static bool isStart = true;
	if(isStart) {
		isStart = false;
		start = count;
	}
	else {
		if(start > 0) {
			start--;
		}
		else {
			isStart = true;
			start = count;
			return true;
		}
	}
	return false;
}

void StartTask(void const * argument) {
	int cntr = 0;
	device = &esp;
	parameters.setSSID("UPC5016370");
	parameters.setPasskey("vn8rtfxUtmFr");
	parameters.setPort(8080);
	parameters.setServerIP("192.168.0.150");
	device->setParameter(WifiDevice::ESP_DEVICE_PARAMS, DataManager<WifiDevice::ESPParams>::convertFromData(parameters));
	device->setCallback(WifiDevice::ESP_WIFI_CONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiConnectionCompletedCb_t>::convertFromData(ConnectionComplete));
	device->setCallback(WifiDevice::ESP_WIFI_DISCONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiDisconnectionCompletedCb_t>::convertFromData(DisconnectionComplete));
	device->setCallback(WifiDevice::ESP_CONNECTION_OPENED_CB,DataManager<WifiDevice::ESPConnectionOpenedCb_t>::convertFromData(TCPConnection));
	device->setCallback(WifiDevice::ESP_DATA_RECEIVED_CB,DataManager<WifiDevice::ESPDataReceivedEventCb_t>::convertFromData(DataReceived));
	device->setCallback(WifiDevice::ESP_CONNECTION_CLOSED_CB,DataManager<WifiDevice::ESPConnectionClosedCb_t>::convertFromData(TCPClosed));
	device->setCallback(WifiDevice::ESP_WIFI_IS_CONNECTED_CB,DataManager<WifiDevice::ESPWifiisConnectedCb_t>::convertFromData(isConnected));

	static_cast<WifiDevice::ESP32*>(device)->start();

	while(!static_cast<WifiDevice::ESP32*>(device)->isStatusOk()) {
		device->sendCmd(WifiDevice::ESP_GET_WIFI_STATUS);
		osDelay(100);
		device->process();
	}

	while(!static_cast<WifiDevice::ESP32*>(device)->isStatusOk()) {
		device->sendCmd(WifiDevice::ESP_DISABLE_MULTIPLE_CONN);
		osDelay(100);
		device->process();
	}

	for(;;) {
		if(!device->isBusy()) {
			switch(state) {
				case AppState::WifiDisconnected:
					if(connected) {
						state = AppState::WifiConnected;
					}
					else {
						device->sendCmd(WifiDevice::ESP_CONNECT_WIFI);
					}
				break;

				case AppState::WifiConnected:
					if(TCPconnected) {
						state = AppState::Run;
					}
					else {
						device->sendCmd(WifiDevice::ESP_CONNECT_TCP);
					}
				break;

				case AppState::Run:
					if(received) {
						cntr++;
						toSend.setData(ptr);
						toSend.setLength(length);
						device->send(DataManager<WifiDevice::ESPData>::convertFromData(toSend));
						osDelay(10);
						received = false;
					}

					if(cntr >= 4) {
						state=AppState::Done;
					}
				break;

				case AppState::Done:
					if(TCPconnected) {
						device->sendCmd(WifiDevice::ESP_DISCONNECT_TCP);
					}
					else {
						HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
					}
				break;
			}
		}
		device->process();
		osDelay(100);
	}
}

}
#else

void WifiConnectionCompletedCb();
void WifiDisconnectionCompletedCb();
void DataReceivedCb(char* data, int size);
void TCPConnectionClosedCb();
void WifiIsConnectedCb(bool status);
void ClientConnectedCb(int num);

WifiDevice::ESPWifiConnectionCompletedCb_t ConnectionComplete = WifiConnectionCompletedCb;
WifiDevice::ESPWifiDisconnectionCompletedCb_t DisconnectionComplete = WifiDisconnectionCompletedCb;
WifiDevice::ESPConnectionClosedCb_t TCPClosed = TCPConnectionClosedCb;
WifiDevice::ESPDataReceivedEventCb_t DataReceived = DataReceivedCb;
WifiDevice::ESPWifiisConnectedCb_t isConnected = WifiIsConnectedCb;
WifiDevice::ESPClientConnectedCb_t ClientConnected = ClientConnectedCb;

char *ptr = nullptr;
int length = 0;
bool connected;
bool received = false;
bool getStatus= true;
int client;
bool clientConnected= false;

enum class AppState {
	Run,
	WifiDisconnected,
	WifiConnected,
	Listen,
	Done,
	GetIp,
};

AppState state = AppState::WifiDisconnected;

void ClientConnectedCb(int num) {
	client = num;
	clientConnected = true;
}

void WifiIsConnectedCb(bool status) {
	connected = status;
	getStatus = false;

}

void WifiConnectionCompletedCb() {
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	connected = true;
}

void WifiDisconnectionCompletedCb() {
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
	connected = false;
}

void TCPConnectionClosedCb() {
	clientConnected = false;
}

void DataReceivedCb(char* data, int size) {
	received = true;
	ptr = data;
	length = size;
}

extern "C" {

void ESPUartITHandler() {
	device->callHandler();
}


void StartTask(void const * argument) {
	int cntr = 0;
	device = &esp;

	WifiDevice::ESPRole_t role = WifiDevice::WIFI_SERVER;

	parameters.setSSID("UPC5016370");
	parameters.setPasskey("vn8rtfxUtmFr");
	parameters.setPort(8080);

	device->setParameter(WifiDevice::ESP_DEVICE_PARAMS, DataManager<WifiDevice::ESPParams>::convertFromData(parameters));
	device->setParameter(WifiDevice::ESP_ROLE, DataManager<WifiDevice::ESPRole_t>::convertFromData(role));

	device->setCallback(WifiDevice::ESP_WIFI_CONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiConnectionCompletedCb_t>::convertFromData(ConnectionComplete));
	device->setCallback(WifiDevice::ESP_WIFI_DISCONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiDisconnectionCompletedCb_t>::convertFromData(DisconnectionComplete));
	device->setCallback(WifiDevice::ESP_WIFI_IS_CONNECTED_CB,DataManager<WifiDevice::ESPWifiisConnectedCb_t>::convertFromData(isConnected));

	device->setCallback(WifiDevice::ESP_CLIENT_CONNECTED_CB,DataManager<WifiDevice::ESPClientConnectedCb_t>::convertFromData(ClientConnected));
	device->setCallback(WifiDevice::ESP_DATA_RECEIVED_CB,DataManager<WifiDevice::ESPDataReceivedEventCb_t>::convertFromData(DataReceived));
	device->setCallback(WifiDevice::ESP_CONNECTION_CLOSED_CB,DataManager<WifiDevice::ESPConnectionClosedCb_t>::convertFromData(TCPClosed));



	device->start();

	while(!static_cast<WifiDevice::ESP32*>(device)->isStatusOk()) {
		device->sendCmd(WifiDevice::ESP_GET_WIFI_STATUS);
		osDelay(100);
		device->process();
	}

	while(!static_cast<WifiDevice::ESP32*>(device)->isStatusOk()) {
		device->sendCmd(WifiDevice::ESP_ENABLE_MULTIPLE_CONN);
		osDelay(100);
		device->process();
	}


	for(;;) {
		if(!device->isBusy()) {
			switch(state) {
				case AppState::WifiDisconnected:
					if(connected) {
						state = AppState::WifiConnected;
					}
					else {
						device->sendCmd(WifiDevice::ESP_CONNECT_WIFI);
					}
				break;

				case AppState::GetIp:
					state=AppState::Listen;
					device->sendCmd(WifiDevice::ESP_OPEN_TCP_SERVER);
				break;

				case AppState::WifiConnected:
					state=AppState::GetIp;
					device->sendCmd(WifiDevice::ESP_GET_IP);
				break;

				case AppState::Listen:
					if(clientConnected) {
						state=AppState::Run;
					}
				break;

				case AppState::Run:
					if(cntr >= 4) {
						state=AppState::Done;
						device->sendCmd(WifiDevice::ESP_CLOSE_TCP_SERVER);
					}
					else {
						toSend.setData("Server");
						toSend.setLength(6);
						device->send(DataManager<WifiDevice::ESPData>::convertFromData(toSend));
						osDelay(10);
						cntr++;
					}
				break;

				case AppState::Done:
						HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				break;
			}
		}
		device->process();
		osDelay(100);
	}
}
}
#endif

#endif



