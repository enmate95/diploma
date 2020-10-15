#include "DataManager.h"
#include "LORA.h"
#include "cmsis_os.h"
#include "main.h"
#include "uartDevice.h"
#include "usart.h"
#include "Defines.h"

#if LORA_TEST

RadioDevice* device;

UartDMA LoRaSerial(&huart2);
SLIP slip;
CRC16 crc;
LoRaHCI::HCI hci(LoRaSerial,slip,crc);
LoRa::LoRaDevice lora(hci);

enum class AppState {
	Run,
};

AppState state = AppState::Run;


void PingHandler();

void PingHandler() {
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
}

extern "C" {
void LoRaUartITHandler() {
	device->callHandler();
}

void StartTask(void const * argument) {
	LoRa::LORAPingCb_t cb = PingHandler;
	device = &lora;

	device->setCallback(LoRa::LORA_PING_CB, DataManager<LoRa::LORAPingCb_t>::convertFromData(cb));
	device->start();

	for(;;) {
		if(!device->isBusy()) {
			switch(state) {

			case AppState::Run:
					device->sendCmd(LoRa::WAKEUP);
					osDelay(5);
					device->sendCmd(LoRa::PING);
				break;
			}
		}
		device->process();
		osDelay(500);
	}
}

}

#endif



