#define _GNU_SOURCE
#include "schedule.h"

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int policy;

int main(int argc, char const *argv[])
{
    // determine policy type
	heap_t *task_heap;
    char type[8];
    scanf("%s", type);
    if(type[0]=='F'){ // FIFO
        policy = FIFO;
		task_heap = heap_create(fifo_pri);
    }else if(type[0]=='R'){ // R
        policy = RR;
		task_heap = heap_create(rr_pri);
    }else if(type[0]=='S'){ // SJF
        policy = SJF;
		task_heap = heap_create(sjf_pri);
    }else if(type[0]=='P'){ // PSJF
        policy = PSJF;
		task_heap = heap_create(psjf_pri);
    }else{
        printf("The input policy is not correct.\n");
		exit(1);
    }

    // get number of processes
    int n;
    scanf("%d", &n);
    // get each line
    struct waiting_task task[MAX_WAITING_NUM];
    struct process P[MAX_WAITING_NUM];
	memset(P, 0, sizeof(P));

    for(int i=0;i<n;i++){
        scanf("%s%d%d", P[i].name, &P[i].ready_time, &P[i].left_time);
		task[i].ready_time = P[i].ready_time;
		P[i].exec_time = P[i].ready_time;

		insert(task_heap, &P[i]);
    }

	while (!empty())

#ifdef COMMENT
    // run!
	int current_task = 0; // which task is waiting
	int now = 0; // the current time unit
	int next_ready_time;
	int main_counter = 0;

    while(current_task<n){
		
		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
			insert(task_heap, task[current_task].p);
			
			pid_t pid = fork();
			// get start time
			if(pid == 0){ // child
			    struct sched_param para;
				sched_setscheduler(getpid(), SCHED_IDLE, &para);
				
				printf("%s %d\n", task[current_task].p->name, getpid());
				period(task[current_task].p->left_time);
				// get end time
				// send signal to parent
				// mmap
				exit(0);
			}
			// assign pid and counter to process, and throw the struct process P into heap
task[current_task].p->pid = pid;
			task[current_task].p->counter = current_task;
			
			insert(task_heap, task[current_task].p);
			
			current_task++;
		}

		if(current_task==n){ // then there is nothing need to fork
			break;
		}else{
			// detemine next ready time
			next_ready_time = task[current_task].ready_time - now;
        	// wait until the next for time
        	period(next_ready_time);
			// renew now
			now = task[current_task].ready_time;
		}

    }
	// wait child
#endif

    return 0;
}
