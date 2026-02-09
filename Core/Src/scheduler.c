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

Task_t idle;

static volatile uint32_t tick;
static volatile uint32_t current_index;



static void idle_task(void *parameter)
{
    while (1) {
        __WFI();
    }
}

void SysTick_Handler(void){

	tick++;
	for(int i = 0; i < scheduler.number_of_tasks;i++){
		if(scheduler.tasks_TCB[i]->state == BLOCKED){
			scheduler.tasks_TCB[i]->delay_ticks--;
			if(scheduler.tasks_TCB[i]->delay_ticks == 0){
				scheduler.tasks_TCB[i]->state = READY;
			}
		}
	}
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; //Trigger PendSV exception to context switch
}



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

	//create idle
	Task_Create(&idle,idle_task, NULL);
	scheduler.idle_task = &idle;
	//Set first task
	current_running_task = scheduler.tasks_TCB[0];
	current_running_task->state = READY;
	scheduler.scheduler_index = 0;
	__set_PSP((uint32_t)current_running_task->sp);
	__DSB();
	__set_CONTROL(CONTROL_SPSEL_Msk);
	__enable_irq();
	__ISB();
	__asm volatile (
			" .syntax unified     \n"
			"svc #0 			  \n"

	);
}

void run_scheduler(){

		current_index = scheduler.scheduler_index;
	do{

		scheduler.scheduler_index++;
		scheduler.scheduler_index = (scheduler.scheduler_index > scheduler.number_of_tasks - 1) ? 0 : scheduler.scheduler_index;


		if(scheduler.tasks_TCB[scheduler.scheduler_index]->state == READY){
			current_running_task = scheduler.tasks_TCB[scheduler.scheduler_index];
			return;
		}

	}while(current_index != scheduler.scheduler_index);

	current_running_task = scheduler.idle_task;

}
