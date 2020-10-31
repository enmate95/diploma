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

#if HTTP_MODE
UartDMA serial(&huart1);
Cellular::SIM7000 sim7(serial);
Cellular::SIM7000Params params;
Cellular::SIM7000Data data;

bool GPRSStatus = false;
char GETResult[500] = "";

void GetGPRSstateCallback(bool state);
void GetIPCallback(char* ip);
void GETSessionCallback(char* res,int length);

Cellular::SIM7000IsGPRSConnectedCb_t GPRSStateCb = GetGPRSstateCallback;
Cellular::SIM7000GetIPCb_t GetIPResultCb = GetIPCallback;
Cellular::SIM7000GETResultCb_t GETresultCb = GETSessionCallback;

void GetGPRSstateCallback(bool state) {
	GPRSStatus = state;
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
	bool simSet = false;
	params.setAPN("vitamax.internet.vodafone.net");
	params.setURL("www.sim.com");
	params.setPIN(7188);
	device->setParameter(Cellular::SIM7000_DEVICE_PARAMS, DataManager<Cellular::SIM7000Params>::convertFromData(params));
	device->setCallback(Cellular::SIM7000_GPRS_IS_CONNECTED_CB, DataManager<Cellular::SIM7000IsGPRSConnectedCb_t>::convertFromData(GPRSStateCb));
	device->setCallback(Cellular::SIM7000_HTTP_GET_RECEIVED_CB, DataManager<Cellular::SIM7000GETResultCb_t>::convertFromData(GETresultCb));
	device->setCallback(Cellular::SIM7000_GET_IP_CB, DataManager<Cellular::SIM7000GetIPCb_t>::convertFromData(GetIPResultCb));
	device->start();

	for(;;) {
		if(!device->isBusy()) {
			if(simSet) {
				break;
			}
			else {
				device->sendCmd(Cellular::SIM7000_SET_PIN);
				simSet = true;
			}
		}
		device->process();
		osDelay(100);
	}

	for(;;) {
		if(!device->isBusy()) {
			if(GPRSStatus) {
				break;
			}
			else {
				device->sendCmd(Cellular::SIM7000_GET_GPRS_STATUS);
			}
		}
		device->process();
		osDelay(500);
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
					device->send(DataManager<Cellular::SIM7000Data>::convertFromData(data));
				break;

				case AppState::HTTPClose:
					switch(substate) {
						case SubState::StopHTTP:
							device->sendCmd(Cellular::SIM7000_HTTP_TERMINATE);
							substate = SubState::BearerClose;
						break;

						case SubState::BearerClose:
							device->sendCmd(Cellular::SIM7000_HTTP_CLOSE_BEARER);
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
