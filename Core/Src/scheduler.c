/*
 * scheduler.c
 *
 *  Created on: 4 de fev. de 2026
 *      Author: elias
 */


#include "scheduler.h"
#include "stm32f0xx.h"

#define CPU_CLOCK_HZ        8000000UL
#define SYSTICK_1MS_RELOAD  ((CPU_CLOCK_HZ / 1000UL) - 1UL)

volatile Scheduler_t scheduler = {};
volatile Task_t * current_running_task;

Task_t idle;

static volatile uint32_t tick;
static volatile uint32_t current_index;

static void config_systick(){
	WRITE_REG(SysTick->CTRL,0);

	WRITE_REG(SysTick->LOAD,SYSTICK_1MS_RELOAD);
	WRITE_REG(SysTick->VAL,0);

	SET_BIT(SysTick->CTRL,SysTick_CTRL_CLKSOURCE_Msk);
	SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
	SET_BIT(SysTick->CTRL,SysTick_CTRL_ENABLE_Msk);
}

static void setup_irq_priorities(){
	NVIC_SetPriority(PendSV_IRQn,3);
	NVIC_SetPriority(SysTick_IRQn,0);
}

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
	scheduler.tasks_TCB[scheduler.number_of_tasks]->task_id = scheduler.number_of_tasks;
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

	setup_irq_priorities();
	config_systick();

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



void SVC_Handler(void){
	__asm volatile (
			" .global current_running_task                    \n"
			" .syntax unified                                 \n"
			"ldr r0, =0xFFFFFFFD 		\n"
			"mov lr, r0					\n"
			" ldr r1, =current_running_task \n"
			" ldr r1, [r1]             \n"
			" ldr r0, [r1]             \n"
			" mov r3, r0               \n"

			" adds r0, #16             \n"
			" ldmia r0!, {r4-r7}       \n"
			" mov r8, r4               \n"
			" mov r9, r5               \n"
			" mov r10, r6              \n"
			" mov r11, r7              \n"
	        " mov r0, r3               \n"
	        " ldmia r0!, {r4-r7}       \n"
			" adds r3, #32             \n"
			" msr psp, r3              \n"
	        "bx lr                    \n"

	);
}



void PendSV_Handler(void)
{

    __asm volatile (
        " .global current_running_task                    \n"
        " .syntax unified                                 \n"
        " mrs r0, psp              \n"
        " subs r0, r0, #32          \n"
        " mov r3, r0               \n"

        " stmia r0!, {r4-r7}       \n"
        " mov r4, r8               \n"
        " mov r5, r9               \n"
        " mov r6, r10              \n"
        " mov r7, r11              \n"
        " stmia r0!, {r4-r7}       \n"
        " ldr r1, =current_running_task \n"
        " ldr r1, [r1]             \n"
        " str r3, [r1]             \n"
        " cpsid i                  \n"
        " bl run_scheduler         \n"
        " cpsie i                  \n"
    	"ldr r0, =0xFFFFFFFD 		\n"
    	"mov lr, r0					\n"
        " ldr r1, =current_running_task \n"
        " ldr r1, [r1]             \n"
        " ldr r0, [r1]             \n"
        " mov r3, r0               \n"

        " adds r0, #16             \n"
        " ldmia r0!, {r4-r7}       \n"
        " mov r8, r4               \n"
        " mov r9, r5               \n"
        " mov r10, r6              \n"
        " mov r11, r7              \n"

        " mov r0, r3               \n"
        " ldmia r0!, {r4-r7}       \n"

        " adds r3, #32             \n"
        " msr psp, r3              \n"

        "bx lr                    \n"
    );
}



