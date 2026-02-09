#include "stm32f0xx.h"
#include "scheduler.h"
#include "task.h"
#include <stdio.h>



#define CPU_CLOCK_HZ        8000000UL
#define SYSTICK_1MS_RELOAD  ((CPU_CLOCK_HZ / 1000UL) - 1UL)
static volatile uint32_t count1 = 0;
static volatile uint32_t count2 = 0;
static volatile uint32_t count3 = 0;



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


Task_t task1;
Task_t task2;
Task_t task3;




static void task1_handler(void *parameter){

	while(1){
		count1++;
		Task_Delay(1000);
	}
}

static void task2_handler(void *parameter){

	while(1){
		count2++;
		Task_Delay(2000);
	}
}


static void task3_handler(void *parameter){

	while(1){
		count3++;
		Task_Delay(3000);
	}
}



void SVC_Handler(void) __attribute__((naked));
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


void PendSV_Handler(void) __attribute__((naked));
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

int main(){
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_WWDG_STOP;
	setup_irq_priorities();
	config_systick(); // Not used right now since i'm using yield to request swap context

	//Start tasks
	Task_Create(&task1, task1_handler, NULL);
	Task_Create(&task2, task2_handler, NULL);
	Task_Create(&task3, task3_handler, NULL);

	attach_task(&task1);
	attach_task(&task2);
	attach_task(&task3);

	scheduler_start();
	while(1){
	}
	return 0;
}
