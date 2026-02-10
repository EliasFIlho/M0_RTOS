/*
 * kernel.c
 *
 *  Created on: 9 de fev. de 2026
 *      Author: elias
 */


#include "kernel.h"



void create_mutex(Mutex_t * mux){
	mux->state = FREE;
	mux->task_owner_id = 0xFF; //Above max task
	mux->wait_count = 0;

}
void lock(Mutex_t *  mux){
	__disable_irq();

	if(mux->state == FREE){
		mux->task_owner_id = current_running_task->task_id;
		mux->state = LOCKED;
		__enable_irq();
		return;
	}

	if((mux->state == LOCKED) && (mux->task_owner_id == current_running_task->task_id)){
		__enable_irq();
		return; // Mutex already locked
	}

	current_running_task->state = BLOCKED;
	mux->waiting_queue[mux->wait_count++] = current_running_task;
	__enable_irq();
	Yield();

}
void release(Mutex_t *  mux){
	__disable_irq();

	if((mux->state != LOCKED) || mux->task_owner_id != current_running_task->task_id){
		__enable_irq();
		return;// Only owner task can release
	}

	mux->state = FREE; // Free Mutex

	if(mux->wait_count == 0){
		mux->task_owner_id = 0xFF;
		__enable_irq();
		return;
	}

	mux->task_owner_id = mux->waiting_queue[0]->task_id; // Set the first element of waiting queue as task_owner of mutex
	mux->waiting_queue[0]->state = READY; // Update the BLOCKEDs state to READY

	for(int i = 0;i<mux->wait_count;i++){
		mux->waiting_queue[i] = mux->waiting_queue[i+1]; // Shift the queue
	}
	mux->wait_count--;
	__enable_irq();


}






