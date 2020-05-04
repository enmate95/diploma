/*
 * Tasks.h
 *
 *  Created on: May 4, 2020
 *      Author: mate_pc
 */

#ifndef TASKS_H_
#define TASKS_H_
#include "main.h"
#include "cmsis_os.h"

void InitTasks(UART_HandleTypeDef *Handler);
void ProcessTask(void *param);


#endif /* TASKS_H_ */
