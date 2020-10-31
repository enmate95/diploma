/*
 * CellurarTest.cpp
 *
 *  Created on: 18 Oct 2020
 *      Author: horny
 */

#include "DataManager.h"
#include "cmsis_os.h"
#include "main.h"
#include "uartDevice.h"
#include "usart.h"
#include "SIM7000.h"
#include "Defines.h"

#if SIM7000_TEST
RadioDevice* device;
UartDMA serial(&huart1);
Cellular::SIM7000 sim7(serial);
Cellular::SIM7000Params params;
Cellular::SIM7000Data data;

#if HTTP_MODE

bool GPRSStatus = false;
bool IsPinNeeded = false;
char GETResult[500] = "";

void GetGPRSstateCallback(bool state);
void GetIPCallback(char* ip);
void GETSessionCallback(char* res,int length);
void IsPINCallback(bool isPin);

Cellular::SIM7000IsGPRSConnectedCb_t GPRSStateCb = GetGPRSstateCallback;
Cellular::SIM7000GetIPCb_t GetIPResultCb = GetIPCallback;
Cellular::SIM7000GETResultCb_t GETresultCb = GETSessionCallback;
Cellular::SIM7000IsPINCb_t IsPINCb = IsPINCallback;

void GetGPRSstateCallback(bool state) {
	GPRSStatus = state;
}

void IsPINCallback(bool isPin) {
	IsPinNeeded = isPin;
}

void GetIPCallback(char* ip) {

}

void GETSessionCallback(char* res,int length) {
	memcpy(GETResult,res,length);
}

extern "C" {
	void CellularITHandler() {
		device->callHandler();
	}
}

enum class AppState {
	Run,
	HTTPInit,
	HTTPClose,
	Done,
};

enum class SubState {
	BearerConfig,
	BearerOpen,
	StartHTTP,
	SetCID,
	SetURL,
	StopHTTP,
	BearerClose,
};

AppState state = AppState::HTTPInit;
SubState substate = SubState::BearerConfig;

extern "C" {
void StartTask() {
	device = &sim7;
	bool get = true;
	params.setAPN("vitamax.internet.vodafone.net");
	params.setURL("www.sim.com");
	params.setPIN(7188);
	device->setParameter(Cellular::SIM7000_DEVICE_PARAMS, DataManager<Cellular::SIM7000Params>::convertFromData(params));
	device->setCallback(Cellular::SIM7000_GPRS_IS_CONNECTED_CB, DataManager<Cellular::SIM7000IsGPRSConnectedCb_t>::convertFromData(GPRSStateCb));
	device->setCallback(Cellular::SIM7000_HTTP_GET_RECEIVED_CB, DataManager<Cellular::SIM7000GETResultCb_t>::convertFromData(GETresultCb));
	device->setCallback(Cellular::SIM7000_GET_IP_CB, DataManager<Cellular::SIM7000GetIPCb_t>::convertFromData(GetIPResultCb));
	device->setCallback(Cellular::SIM7000_IS_PIN_CB, DataManager<Cellular::SIM7000IsPINCb_t>::convertFromData(IsPINCb));
	device->start();


	for(;;) {
		device->sendCmd(Cellular::SIM7000_DISABLE_ECHO);
		osDelay(500);
		device->process();
		if(!device->isBusy())
			break;
	}

	device->sendCmd(Cellular::SIM7000_IS_PIN_NEEDED);
	while(device->isBusy()) {
		device->process();
		osDelay(100);
	}

	if(IsPinNeeded) {
		device->sendCmd(Cellular::SIM7000_SET_PIN);
		while(device->isBusy()) {
			device->process();
			osDelay(100);
		}
	}

	device->sendCmd(Cellular::SIM7000_SET_RX_DATA_FORMAT);
	while(device->isBusy()) {
		device->process();
		osDelay(100);
	}

	for(;;) {
		device->sendCmd(Cellular::SIM7000_GET_GPRS_STATUS);
		osDelay(500);
		device->process();
		if(GPRSStatus)
			break;
	}

	for(;;) {
		if(!device->isBusy()) {
			switch(state) {
				case AppState::HTTPInit:
					switch(substate) {
						case SubState::BearerConfig:
							device->sendCmd(Cellular::SIM7000_HTTP_CONFIG_BEARER);
							substate = SubState::BearerOpen;
						break;

						case SubState::BearerOpen:
							device->sendCmd(Cellular::SIM7000_HTTP_OPEN_BEARER);
							substate = SubState::StartHTTP;
						break;

						case SubState::StartHTTP:
							device->sendCmd(Cellular::SIM7000_HTTP_INIT);
							substate = SubState::SetCID;
						break;

						case SubState::SetCID:
							device->sendCmd(Cellular::SIM7000_HTTP_SET_CID);
							substate = SubState::SetURL;
						break;

						case SubState::SetURL:
							device->sendCmd(Cellular::SIM7000_HTTP_SET_URL);
							substate = SubState::StopHTTP;
							state = AppState::Run;
						break;

						default:
							break;
					}
				break;

				case AppState::Run:
					if(get) {
						get = false;
						device->send(DataManager<Cellular::SIM7000Data>::convertFromData(data));
					}
					else {
						Cellular::SIM7000Protocol_t newProt = Cellular::HTTP_POST;
						data.setData("asd");
						data.setLength(3);
						device->setParameter(Cellular::SIM7000_PROTOCOL, DataManager<Cellular::SIM7000Protocol_t>::convertFromData(newProt));
						device->send(DataManager<Cellular::SIM7000Data>::convertFromData(data));
						state = AppState::HTTPClose;
					}
				break;

				case AppState::Done:
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				break;

				case AppState::HTTPClose:
					switch(substate) {
						case SubState::StopHTTP:
							device->sendCmd(Cellular::SIM7000_HTTP_TERMINATE);
							substate = SubState::BearerClose;
						break;

						case SubState::BearerClose:
							device->sendCmd(Cellular::SIM7000_HTTP_CLOSE_BEARER);
							state = AppState::Done;
						break;

						default:
							break;
					}
				break;
			}
		}
		device->process();
		osDelay(200);
	}
}
}
#endif

#if TCP_CLIENT_MODE
bool GPRSStatus = false;
bool IsPinNeeded = false;
bool connected = false;
char received[500] = "";
char send[] = "GET https://www.google.com HTTP/1.0";

Cellular::SIM7000Role_t role = Cellular::TCP;
Cellular::SIM7000Protocol_t protocol = Cellular::TCP_CLIENT;

void GetGPRSstateCallback(bool state);
void DataReceived(char* data,int length);
void Connected();
void Disconnected();
void IsPINCallback(bool isPin);

Cellular::SIM7000TCP_UDP_DataReceivedEventCb_t DataReceivedCallback = DataReceived;
Cellular::SIM7000ConnectedToTCPServerCb_t ConnectedToTCPServerCallback = Connected;
Cellular::SIM7000DisconnectedFromTCPServerCb_t DisconnectedFromTCPServerCallback = Disconnected;
Cellular::SIM7000IsGPRSConnectedCb_t GPRSStateCb = GetGPRSstateCallback;
Cellular::SIM7000IsPINCb_t IsPINCb = IsPINCallback;

void GetGPRSstateCallback(bool state) {
	GPRSStatus = state;
}

void IsPINCallback(bool isPin) {
	IsPinNeeded = isPin;
}

void DataReceived(char* data,int length) {
	if(length < sizeof(received)) {
		memcpy(received,data,length);
	}
}

void Connected() {
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	connected = true;
}

void Disconnected() {
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	connected = false;
}

extern "C" {
	void CellularITHandler() {
		device->callHandler();
	}
}

enum class AppState {
	InitTCP,
	Run,
	ClosTCP,
	Done,
};

enum class SubState {
	StartTCPTask,
	ConnectNetwork,
	ConnectTCPServer,
	DisconnectServer,
	StopTCPTask,
};

AppState state = AppState::InitTCP;
SubState substate = SubState::StartTCPTask;

extern "C" {
void StartTask() {
	device = &sim7;
	params.setServerIP("docs.postman-echo.com");
	params.setAPN("vitamax.internet.vodafone.net");
	params.setPort(80);
	params.setPIN(7188);
	device->setParameter(Cellular::SIM7000_DEVICE_PARAMS, DataManager<Cellular::SIM7000Params>::convertFromData(params));

	device->setParameter(Cellular::SIM7000_ROLE, DataManager<Cellular::SIM7000Role_t>::convertFromData(role));
	device->setParameter(Cellular::SIM7000_PROTOCOL, DataManager<Cellular::SIM7000Protocol_t>::convertFromData(protocol));

	device->setCallback(Cellular::SIM7000_GPRS_IS_CONNECTED_CB, DataManager<Cellular::SIM7000IsGPRSConnectedCb_t>::convertFromData(GPRSStateCb));
	device->setCallback(Cellular::SIM7000_IS_PIN_CB, DataManager<Cellular::SIM7000IsPINCb_t>::convertFromData(IsPINCb));
	device->setCallback(Cellular::SIM7000_TCP_CONNECT_TO_SERVER, DataManager<Cellular::SIM7000ConnectedToTCPServerCb_t>::convertFromData(ConnectedToTCPServerCallback));
	device->setCallback(Cellular::SIM7000_TCP_DISCONNECTED_FROM_SERVER_CB, DataManager<Cellular::SIM7000DisconnectedFromTCPServerCb_t>::convertFromData(DisconnectedFromTCPServerCallback));
	device->setCallback(Cellular::SIM7000_TCP_UDP_DATA_RECEIVED_CB, DataManager<Cellular::SIM7000TCP_UDP_DataReceivedEventCb_t>::convertFromData(DataReceivedCallback));
	device->start();


	for(;;) {
		device->sendCmd(Cellular::SIM7000_DISABLE_ECHO);
		osDelay(500);
		device->process();
		if(!device->isBusy())
			break;
	}

	device->sendCmd(Cellular::SIM7000_IS_PIN_NEEDED);
	while(device->isBusy()) {
		device->process();
		osDelay(100);
	}

	if(IsPinNeeded) {
		device->sendCmd(Cellular::SIM7000_SET_PIN);
		while(device->isBusy()) {
			device->process();
			osDelay(100);
		}
	}

	device->sendCmd(Cellular::SIM7000_SET_RX_DATA_FORMAT);
	while(device->isBusy()) {
		device->process();
		osDelay(100);
	}

	for(;;) {
		device->sendCmd(Cellular::SIM7000_GET_GPRS_STATUS);
		osDelay(500);
		device->process();
		if(GPRSStatus)
			break;
	}

	for(;;) {
		if(!device->isBusy()) {
			switch(state) {
				case AppState::InitTCP:
					switch(substate) {
						case SubState::StartTCPTask:
							device->sendCmd(Cellular::SIM7000_TCP_UDP_START_TASK);
							substate = SubState::ConnectNetwork;
						break;

						case SubState::ConnectNetwork:
							device->sendCmd(Cellular::SIM7000_TCP_UDP_CONNECT_NETWORK);
							substate = SubState::ConnectTCPServer;
						break;

						case SubState::ConnectTCPServer:
							if(connected) {
								state = AppState::Run;
							}
							else {
								device->sendCmd(Cellular::SIM7000_TCP_CONNECT_TO_SERVER);
							}
						break;

						default:
							break;
					}
				break;

				case AppState::Run:
					data.setData(send);
					data.setLength(strlen(send));
					device->send(DataManager<Cellular::SIM7000Data>::convertFromData(data));
				break;

				case AppState::Done:
					HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				break;

				case AppState::ClosTCP:
					switch(substate) {
						case SubState::DisconnectServer:
							device->sendCmd(Cellular::SIM7000_TCP_DISCONNECTED_FROM_SERVER_CB);
							substate = SubState::StopTCPTask;
						break;

						case SubState::StopTCPTask:
							device->sendCmd(Cellular::SIM7000_TCP_UDP_DISCONNECT_NETWORK);
							state = AppState::Done;
						break;

						default:
							break;
					}
				break;
			}
		}
		device->process();
		osDelay(200);
	}
}
}
#endif

#endif
