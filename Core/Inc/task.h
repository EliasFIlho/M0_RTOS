/*
 * task.h
 *
 *  Created on: 1 de fev. de 2026
 *      Author: elias
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

#include <stdint.h>

#define FIXED_STACK_SIZE 128

typedef uint32_t StackType;

typedef enum {
	NEW,
	READY,
	BLOCKED,
	RUNNING,
	DONE
}TaskState;


typedef void (* TaskHandle_t)(void *parameter);

typedef struct TCB {
    uint32_t *sp;
    TaskHandle_t task_entry;
    TaskState state;
    uint8_t task_id; //This is populated by the scheduler after the attach
    StackType task_stack[FIXED_STACK_SIZE];
} Task_t;


void Task_Create(Task_t *task, TaskHandle_t task_handler, void *param);

#endif /* INC_TASK_H_ */
