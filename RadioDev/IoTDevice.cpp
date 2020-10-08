#include "DataManager.h"
#include "LORA.h"
#include "cmsis_os.h"
#include "main.h"
#include "uartDevice.h"
#include "usart.h"
RadioDevice* device;

UartDMA LoRaSerial(&huart2);
SLIP slip;
CRC16 crc;
LoRaHCI::HCI hci(LoRaSerial,slip,crc);
LoRa::LoRaDevice lora(hci);

void PingHandler() {
	HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
}

extern "C" {
void uartITHandler() {
	static_cast<LoRa::LoRaDevice*>(device)->callHandler();
}
}


enum class AppState {
	Run,
};

AppState state = AppState::Run;

extern "C" {
void StartTask(void const * argument) {
		LoRa::LORAPingCb_t cb = PingHandler;
		DataManager<LoRa::LORAPingCb_t> dm;
		device = &lora;
		static_cast<LoRa::LoRaDevice*>(device)->start();
		device->setCallback(LoRa::LORA_PING_CB, dm.convertFromData(cb));


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


