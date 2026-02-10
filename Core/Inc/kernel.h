/*
 * kernel.h
 *
 * Herer i'll concetrate all the needed functions to use my RTOS
 * and the utilities like queue, mutex, semaphores structures + functions
 *
 *  Created on: 9 de fev. de 2026
 *      Author: elias
 */

#ifndef INC_KERNEL_H_
#define INC_KERNEL_H_

#include "task.h"
#include "scheduler.h"





typedef enum{
	LOCKED,
	FREE
}Mux_state;

typedef struct{
	Mux_state state;
	volatile Task_t * waiting_queue[MAX_TASKS];
	uint8_t wait_count;
	uint8_t task_owner_id;
}Mutex_t;


void create_mutex(Mutex_t *  mux);
void lock(Mutex_t *  mux);
void release(Mutex_t *  mux);

#endif /* INC_KERNEL_H_ */
