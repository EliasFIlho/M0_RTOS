/*
 * scheduler.c
 *
 *  Created on: 4 de fev. de 2026
 *      Author: elias
 */


#include "scheduler.h"
#include "stm32f0xx.h"

volatile Scheduler_t scheduler = {};
volatile Task_t * current_running_task;


int8_t attach_task(Task_t * task){
	if(scheduler.number_of_tasks >= MAX_TASKS){
		return -1;
	}

	scheduler.tasks_TCB[scheduler.number_of_tasks] = task;
	scheduler.number_of_tasks++;
	return 0;
}

void Yield(){
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


void scheduler_start(){
	if(scheduler.number_of_tasks == 0){
		return;
	}

	//Set first task
	current_running_task = scheduler.tasks_TCB[0];
	current_running_task->state = RUNNING;
	scheduler.scheduler_index = 0;
	__set_PSP((uint32_t)current_running_task->sp);
	__set_CONTROL(CONTROL_SPSEL_Msk);
	__ISB();
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void run_scheduler(){
	static volatile index = 0;
	scheduler.scheduler_index++;
	scheduler.scheduler_index = (scheduler.scheduler_index > scheduler.number_of_tasks - 1) ? 0 : scheduler.scheduler_index;
	index = scheduler.scheduler_index;
	current_running_task = scheduler.tasks_TCB[scheduler.scheduler_index];

}
