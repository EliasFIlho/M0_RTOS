
#include "kernel.h"
#include <stdio.h>






static volatile uint32_t count1 = 0;
static volatile uint32_t count2 = 0;
static volatile uint32_t count3 = 0;
static volatile uint32_t shared_counter = 0;

Mutex_t my_mux;




Task_t task1;
Task_t task2;
Task_t task3;




static void task1_handler(void *parameter){

	while(1){
		count1++;
		lock(&my_mux);
		for(int i = 0; i < 5000;i++){
			shared_counter += i;
		}
		release(&my_mux);
		Task_Delay(5000);
	}
}

static void task2_handler(void *parameter){

	while(1){
		count2++;
		lock(&my_mux);
		for(int i = 0; i < 5000;i++){
			shared_counter += i;
		}
		release(&my_mux);
		Task_Delay(5000);
	}
}


static void task3_handler(void *parameter){

	while(1){
		count3++;
		lock(&my_mux);
		for(int i = 0; i < 5000;i++){
			shared_counter += i;
		}
		release(&my_mux);
		Task_Delay(5000);
	}
}



int main(){

	//Start tasks
	Task_Create(&task1, task1_handler, NULL);
	Task_Create(&task2, task2_handler, NULL);
	Task_Create(&task3, task3_handler, NULL);

	attach_task(&task1);
	attach_task(&task2);
	attach_task(&task3);

	create_mutex(&my_mux);

	scheduler_start();
	while(1){
	}
	return 0;
}
