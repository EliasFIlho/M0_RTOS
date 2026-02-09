/*
 * task.h
 *
 *  Created on: 1 de fev. de 2026
 *      Author: elias
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

#include <stdint.h>
#include "stm32f0xx.h"

#define FIXED_STACK_SIZE 128

typedef uint32_t StackType;
typedef uint32_t Ticks_t;
typedef void (* TaskHandle_t)(void *parameter);

typedef enum {
	READY,
	BLOCKED
}TaskState;




typedef struct TCB {
    uint32_t *sp;
    TaskHandle_t task_entry;
    TaskState state;
    Ticks_t delay_ticks;
    uint8_t task_id; //This is populated by the scheduler after the attach
    StackType task_stack[FIXED_STACK_SIZE];
} Task_t;


void Task_Create(Task_t *task, TaskHandle_t task_handler, void *param);
void Task_Delay(Ticks_t ticks);


#endif /* INC_TASK_H_ */
