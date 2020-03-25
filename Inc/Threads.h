/*
 * tasks.h
 *
 *  Created on: Mar 25, 2020
 *      Author: mate_pc
 */

#ifndef TASKS_H_
#define TASKS_H_

#include "main.h"			//because of port definitions
#include "cmsis_os.h"		//because of RTOS

void ProcessTask(void const * argument);
void RXTask(void const *argument);
void TXTask(void const *argument);
void initApp();

#endif /* TASKS_H_ */
