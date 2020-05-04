/*
 * Tasks.cpp
 *
 *  Created on: May 4, 2020
 *      Author: mate_pc
 */


#include "Tasks.h"
//#include "LORA.h"

//char AppKey[] = "817E14A131B2D16F6DFFF27439688E6C";
//char AppEUI[] = "70B3D57ED002C6B8";
TaskHandle_t ProcessHandler1 = NULL;
TaskHandle_t ProcessHandler2 = NULL;
//LoRaDevice *Device;

void InitTasks() {
	xTaskCreate(ProcessTask1,"Process1",2*128,NULL,tskIDLE_PRIORITY + 1,&ProcessHandler1);
	xTaskCreate(ProcessTask2,"Process2",2*128,NULL,tskIDLE_PRIORITY + 1,&ProcessHandler2);
	//Device = LoRa;
}

void ProcessTask1(void *param) {
	 const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	for(;;) {

		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		vTaskDelay(xDelay);
	}
}

void ProcessTask2(void *param) {
	 const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;

	for(;;) {

//		Device->Wakeup();
//		Device->Process();
//		Device->GetRadioConfig();
//		Device->Process();
		HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
		vTaskDelay(xDelay);
	}
}
