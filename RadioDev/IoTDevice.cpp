#include "DataManager.h"
#include "BLETest.h"
#include "cmsis_os.h"
#include "main.h"

RadioDevice* device;
BLEDevice ble;


extern "C" {
void StartTask(void const * argument) {
		device = &ble;
		device->setParameter(BLEParams::passkey,DataManager<uint32_t>::convertFromData(123456));
		uint32_t key = DataManager<uint32_t>::convertFromContainer(device->getParameter(BLEParams::passkey));
		static_cast<BLEDevice*>(device)->test();


		for(;;) {
//			bool sent = false;
//
//			if(!device->isBusy()) {
//				switch(state) {
//				case appstate::idle:
//					state = appstate::advertising;
//				break;
//
//				case appstate::advertising:
//					if(!sent) {
//						sent = true;
//						device->startAdv();
//					}
//					else {
//						if(connected) {
//							state = appstate::connected;
//						}
//						else {
//							osDelay(100);
//							sent = false;
//						}
//					}
//				break;
//
//				case appstate::connected:
//
//				break;
//
//				default:
//				break;
//
//			}
//			}
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);
			device->process();
			osDelay(500);
			
		}
	}
}


