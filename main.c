#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int policy;

int main(int argc, char const *argv[])
{
    // determine policy type
    char type[8];
    scanf("%s", type);
    if(type[0]=='F'){ // FIFO
        policy = FIFO;
    }else if(type[0]=='R'){ // R
        policy = RR;
    }else if(type[0]=='S'){ // SJF
        policy = SJF;
    }else if(type[0]=='P'){ // PSJF
        policy = PSJF;
    }else{
        printf("The input policy is not correct.\n");
    }

    // get number of processes
    int n;
    scanf("%d", &n);
    // get each line
    struct waiting_task task[MAX_WAITING_NUM];
    struct process P[MAX_WAITING_NUM];

    for(int i=0;i<n;i++){
        task[i].p=&P[i];
        scanf("%s%d%d", P[i].name, &task[i].ready_time, &P[i].left_time);
        // put the things into the waiting queue

    }

    // run!
	int current_task=0; // which task is waiting
	int now=0; // the current time unit
	int next_ready_time;
	int main_counter = 0;
    while(current_task<n){
		
		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
			insert(heap, &task[current_task].p);
			
			pid = fork();
			// get start time
			if(pid == 0){ // child
			    struct sched_param para;
				sched_setscheduler(getpid(), SCHED_IDLE, para);
				
				printf("%s %d\n", task[current_task].p->name, getpid());
				period(task[current_task].p->left_time);
				// get end time
				// send signal to parent
				// mmap
				exit();
			}
			// assign pid and counter to process, and throw the struct process P into heap
			task[current_task].p->pid = pid;
			task[current_task].p->counter = current_task;
			
			insert(heap, &task[current_task].p);
			
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

    return 0;
}
