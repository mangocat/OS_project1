#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "schedule.h"
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#define TIME
int policy;
int busy = 0;//1 means that a process is running
struct process* cur_p=NULL;// the process which is running
heap_t *task_heap;
int order[MAX_WAITING_NUM];
void handle_sigchld(int sig) {
    int saved_errno = errno;
    static int cnt = 0; 
    printf("cnt=%d %s\n",cnt,cur_p->name);
    order[cnt] = cur_p->id;
    cnt++;
    cur_p->end = *(cur_p->ptr);
    //run next process, now busy==1
    if(isempty(task_heap)==0){// is not empty
          cur_p = heap_extract_min(task_heap); 
          exec_process(cur_p);
    }
    else{
        busy = 0;
        cur_p = NULL;
    }
    while (waitpid((pid_t)(-1), NULL, WNOHANG) > 0) {}
    errno = saved_errno;
}
int main(int argc, char const *argv[])
{
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    /* sa.sa_flags = 0; */
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
    /* if (sigaction(SIGUSR1, &sa, 0) == -1) { */
        perror(0);
        exit(1);
    }
    proc_assign_cpu(getpid(),0);
    process_t parent;
    parent.pid = getpid();
//    wakeup_process(&parent);
	// determine policy type
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
        task[i].p = &P[i];
        P[i].id = i;
		P[i].exec_time = P[i].left_time;
		P[i].exec_count = 0;
        P[i].counter = 0;
        P[i].pid = -1;
		/* heap_insert(task_heap, &P[i]); */
	}

	// run!
	int current_task = 0; // which task is waiting
	int now = 0; // the current time unit
	int next_ready_time;
    long long main_counter = 0;

	while(current_task<n){

		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
            main_counter++;
			//insert(task_heap, task[current_task].p);

			//heap_insert(task_heap, task[current_task].p);
            exec_process(task[current_task].p);
            // use sigpromask to avoid race condition, wait..... not needed
            if(busy==1){
                block_process(task[current_task].p);
			    heap_insert(task_heap, task[current_task].p);
            }
            else{
                busy = 1;
                cur_p = task[current_task].p;
            }
            // consider interrupt case

#ifdef Z	

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

			heap_insert(task_heap, task[current_task].p);
#endif

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
    while(waitpid((pid_t)-1,NULL,0)>0){}
#ifdef TIME
    for(int i=0; i<n; i++){
        int idx = order[i];
        printf("%s start=%09ld.%09ld end=%09ld.%09ld",P[idx].name,P[idx].start.tv_sec,
                P[idx].start.tv_nsec,P[idx].ptr->tv_sec,P[idx].ptr->tv_nsec);
        printf(" exec_time=%d\n",P[idx].exec_time);
    }
    
#endif

	return 0;
}
