#include "DataManager.h"
#include "LORA.h"
#include "cmsis_os.h"
#include "main.h"
#include "uartDevice.h"
#include "usart.h"
#include "ESP32.h"

RadioDevice* device;

UartDMA LoRaSerial(&huart2);
SLIP slip;
CRC16 crc;
LoRaHCI::HCI hci(LoRaSerial,slip,crc);
LoRa::LoRaDevice lora(hci);

UartDMA ESPSerial(&huart1);
WifiDevice::ESP32 esp(ESPSerial);
WifiDevice::ESPParams parameters;
WifiDevice::ESPData toSend;

void TestHandler();
void PingHandler();

void WifiConnectionCompletedCb();
void WifiDisconnectionCompletedCb();
void TCPConnectionOpenedCb();
void DataReceivedCb(char* data, int size);

WifiDevice::ESPWifiConnectionCompletedCb_t ConnectionComplete = WifiConnectionCompletedCb;
WifiDevice::ESPWifiDisconnectionCompletedCb_t DisconnectionComplete = WifiDisconnectionCompletedCb;
WifiDevice::ESPConnectionOpenedCb_t TCPConnection = TCPConnectionOpenedCb;
WifiDevice::ESPDataReceivedEventCb_t DataReceived = DataReceivedCb;

char *ptr = nullptr;
int length = 0;
bool connected = false;
bool TCPconnected = false;
bool received = false;


void PingHandler() {
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
}

void TestHandler() {
	//HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
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

void DataReceivedCb(char* data, int size) {
	received = true;
	ptr = data;
	length = size;
}

extern "C" {
void LoRaUartITHandler() {
	static_cast<LoRa::LoRaDevice*>(device)->callHandler();
}

void ESPUartITHandler() {
	static_cast<WifiDevice::ESP32*>(device)->callHandler();
}
}


enum class AppState {
	Run,
	WifiDisconnected,
	WifiConnected,
};

AppState state = AppState::WifiDisconnected;

extern "C" {
//void StartTask(void const * argument) {
//	LoRa::LORAPingCb_t cb = PingHandler;
//	DataManager<LoRa::LORAPingCb_t> dm;
//	device = &lora;
//	static_cast<LoRa::LoRaDevice*>(device)->start();
//	device->setCallback(LoRa::LORA_PING_CB, dm.convertFromData(cb));
//
//	for(;;) {
//		if(!device->isBusy()) {
//			switch(state) {
//
//			case AppState::Run:
//					device->sendCmd(LoRa::WAKEUP);
//					osDelay(5);
//					device->sendCmd(LoRa::PING);
//				break;
//			}
//		}
//		device->process();
//		osDelay(500);
//	}
//}
void StartTask(void const * argument) {
	bool sent = false;
	int cntr = 0;
	device = &esp;
	parameters.setSSID("UPC5016370");
	parameters.setPasskey("vn8rtfxUtmFr");
	parameters.setPort(8080);
	parameters.setIP("192.168.0.150");
	device->setParameter(WifiDevice::ESP_DEVICE_PARAMS, DataManager<WifiDevice::ESPParams>::convertFromData(parameters));
	device->setCallback(WifiDevice::ESP_WIFI_CONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiConnectionCompletedCb_t>::convertFromData(ConnectionComplete));
	device->setCallback(WifiDevice::ESP_WIFI_DISCONNECTION_COMPLETE_CB,DataManager<WifiDevice::ESPWifiDisconnectionCompletedCb_t>::convertFromData(DisconnectionComplete));
	device->setCallback(WifiDevice::ESP_CONNECTION_OPENED_CB,DataManager<WifiDevice::ESPConnectionOpenedCb_t>::convertFromData(TCPConnection));
	device->setCallback(WifiDevice::ESP_DATA_RECEIVED_CB,DataManager<WifiDevice::ESPDataReceivedEventCb_t>::convertFromData(DataReceived));

	static_cast<WifiDevice::ESP32*>(device)->start();
	device->sendCmd(WifiDevice::ESP_DISCONNECT_WIFI);

	for(;;) {
		if(!device->isBusy()) {
			switch(state) {
				case AppState::WifiDisconnected:
					if(connected) {
						state = AppState::WifiConnected;
						sent = false;
					}
					else {
						if(!sent) {
							sent = true;
							device->sendCmd(WifiDevice::ESP_CONNECT_WIFI);
						}
						else{
							if(cntr == 100) {
								sent = false;
								cntr=0;
							}
							cntr++;
						}
					}
				break;

				case AppState::WifiConnected:
					if(TCPconnected) {
						state = AppState::Run;
						sent = false;
					}
					else {
						if(!sent) {
							sent = true;
							device->sendCmd(WifiDevice::ESP_CONNECT_TCP);
						}
						else{
							if(cntr == 100) {
								sent = false;
								cntr=0;
							}
							cntr++;
						}
					}
				break;

				case AppState::Run:
					if(received) {
						received = 0;
						toSend.setData(ptr);
						toSend.setLength(length);
						device->send(DataManager<WifiDevice::ESPData>::convertFromData(toSend));
					}
				break;
			}
		}
		device->process();
		osDelay(50);
	}
}
}


