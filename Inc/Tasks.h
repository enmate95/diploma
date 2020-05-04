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
#include "LORA.h"

void InitTasks();
void ProcessTask1(void *param);
void ProcessTask2(void *param);

#endif /* TASKS_H_ */
