/*
 * Tasks.cpp
 *
 *  Created on: May 4, 2020
 *      Author: mate_pc
 */

#include "main.h"
#include "cmsis_os.h"
#include "Tasks.h"


TaskHandle_t ProcessHandler = NULL;


void InitTasks() {
	xTaskCreate(ProcessTask,"Process",4*128,NULL,tskIDLE_PRIORITY + 1,&ProcessHandler);
}

void ProcessTask(void *param) {
	 const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	for(;;) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		vTaskDelay(xDelay);
	}
}
