/*
 * task.c
 *
 *  Created on: 2 de fev. de 2026
 *      Author: elias
 */


#include "task.h"
#include "scheduler.h"

void Task_Create(Task_t *task, TaskHandle_t task_handler, void *param)
{
    int top = FIXED_STACK_SIZE;

    task->task_stack[--top] = 0x01000000;                 // xPSR
    task->task_stack[--top] = (uint32_t)task_handler;     // PC
    task->task_stack[--top] = 0xFFFFFFFD;        		  // LR
    task->task_stack[--top] = 0;                          // R12
    task->task_stack[--top] = 0;                          // R3
    task->task_stack[--top] = 0;                          // R2
    task->task_stack[--top] = 0;                          // R1
    task->task_stack[--top] = (uint32_t)param;            // R0

    task->task_stack[--top] = 0; // R11
    task->task_stack[--top] = 0; // R10
    task->task_stack[--top] = 0; // R9
    task->task_stack[--top] = 0; // R8
    task->task_stack[--top] = 0; // R7
    task->task_stack[--top] = 0; // R6
    task->task_stack[--top] = 0; // R5
    task->task_stack[--top] = 0; // R4

    task->sp = &task->task_stack[top];
    task->state = READY;
}


void Task_Delay(Ticks_t ticks){
	__disable_irq();
	current_running_task->state = BLOCKED;
	current_running_task->delay_ticks = ticks;
	__enable_irq();
	Yield();
}



