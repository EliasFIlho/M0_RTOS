/*
 * scheduler.h
 *
 *  Created on: 4 de fev. de 2026
 *      Author: elias
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "task.h"

#define MAX_TASKS 8


typedef struct {
	Task_t * tasks_TCB[MAX_TASKS];
	uint8_t number_of_tasks;
	uint8_t scheduler_index;
}Scheduler_t;

int8_t attach_task(Task_t * task);
void run_scheduler();
void scheduler_start();
void Yield();

#endif /* INC_SCHEDULER_H_ */
