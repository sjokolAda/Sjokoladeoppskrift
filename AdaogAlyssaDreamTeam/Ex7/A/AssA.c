#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/sem.h>
#include <native/timer.h>

#define CHANNEL_A 1
#define CHANNEL_B 2
#define CHANNEL_C 3

#define TASK_PRIO_HIGH 99 /* Highest RT priority */
#define TASK_PRIO_MEDIUM 80
#define TASK_PRIO_LOW 10

#define TASK_MODE T_JOINABLE /* Allow threads to join */
#define TASK_STKSZ 0 /* Stack size (use default one) */
#define cpuid 1 /*cpu number*/


//task definition
RT_TASK TaskA;
RT_TASK TaskB;
RT_TASK TaskC;

RT_SEM semafor;


int set_cpu(int cpu_number) 
{ 
// setting cpu set to the selected cpu
	cpu_set_t cpu;
	CPU_ZERO(&cpu); 
	CPU_SET(cpu_number, &cpu); 
// set cpu set to current thread and return
	return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu); 
} 



//Task_body

void taskA (void* cookie)
{
	rt_sem_p(&semafor,TM_INFINITE);
	printf("Im taskA using semaphore\n");
	rt_sem_v(&semafor);
}

void taskB (void* cookie)
{
	rt_sem_p(&semafor,TM_INFINITE);
	printf("Im taskB using semaphore\n");
	rt_sem_v(&semafor);
}


void taskC (void* cookie)
{
	int err;

	// Create Semaphore
	err = rt_sem_create(&semafor, "sem", 1,S_PRIO);  
	if (err){
		printf("bad semaphore!%d",err);
		return -1;
	}	


	int delay=rt_timer_ns2ticks(100000000); 	//converts nanoseconds to internal clock ticks
	rt_task_sleep(delay);	//sleeps task

	err =  rt_sem_broadcast(&semafor); //broadcast semaphore
	rt_printf("broadcasting semaphore");
	if(err){
		printf("sem broadcast fail");
	}
	rt_task_sleep(delay);	//sleeps task
	rt_sem_delete(&semafor);
}



int main(){
	
	int err;
	mlockall(MCL_CURRENT|MCL_FUTURE); //lock the current and future memory allocations to the main memory



	
	
	//Create and init tasks
	err = rt_task_create(&TaskA,"TaskA",TASK_STKSZ,TASK_PRIO_LOW,TASK_MODE|T_CPU(cpuid));
	if (!err){

		rt_task_start(&TaskA,&taskA,NULL);
	}
	err = rt_task_create(&TaskB,"TaskB",TASK_STKSZ,TASK_PRIO_MEDIUM,TASK_MODE|T_CPU(cpuid));
	if (!err){
		rt_task_start(&TaskB,&taskB,NULL);
	}

	err = rt_task_create(&TaskC,"TaskC",TASK_STKSZ,TASK_PRIO_HIGH,TASK_MODE|T_CPU(cpuid));
	if (!err){
		rt_task_start(&TaskC,&taskC,NULL);
	}
	
	

	//join rt_threads	
	rt_task_join(&TaskA);
	rt_task_join(&TaskB);
	rt_task_join(&TaskC);
	
	int delay=rt_timer_ns2ticks(1000000000); 	//converts nanoseconds to internal clock ticks
	rt_task_sleep(delay);	//sleeps task

	//delete tasks and sempahore
	rt_task_delete(&TaskA);
	rt_task_delete(&TaskB);
	rt_task_delete(&TaskC);	

	printf("Hello from main! Threads are done and deleted\n");
	return 0;
}
