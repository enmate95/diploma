/*
 * tasks.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: mate_pc
 */
#include <Threads.h>

uint8_t rx_buffer[100];
uint8_t tx_buffer[100];

void initApp() {
	TaskHandle_t xProcessHandle = NULL;
	TaskHandle_t xRXHandle = NULL;
	TaskHandle_t xTXHandle = NULL;
	xTaskCreate((TaskFunction_t)ProcessTask,"Process",256*4,NULL,tskIDLE_PRIORITY+1,&xProcessHandle);
	xTaskCreate((TaskFunction_t)RXTask,"RX",256*4,NULL,tskIDLE_PRIORITY+1,&xRXHandle);
	xTaskCreate((TaskFunction_t)TXTask,"TX",256*4,NULL,tskIDLE_PRIORITY+1,&xTXHandle);
}

void ProcessTask(void const * argument) {

	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

	for(;;) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		vTaskDelay(xDelay);
	}
}

void RXTask(void const * argument) {

	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

	for(;;) {

		vTaskDelay(xDelay);
	}
}

void TXTask(void const * argument) {

	const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

	for(;;) {

		vTaskDelay(xDelay);
	}
}
