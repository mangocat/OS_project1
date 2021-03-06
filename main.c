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
#include <sys/syscall.h>

int policy;
int next_rr_time = -1; // = now + 500, or -1 when the left_time is less than 500
long long main_counter = 0;
int now = 0; // the current time unit
int busy = 0;//1 means that a process is running
int done_num = 0; // indicates how many processes are done, only rr use it
struct process* cur_p=NULL;// the process which is running
int current_p_start_time; // when the current process is started, only PSJF need this
heap_t *task_heap;
int order[MAX_WAITING_NUM];

void handle_sigchld(int sig) {
    int saved_errno = errno;
    static int cnt = 0;
    //printf("cnt=%d %s\n",cnt,cur_p->name);
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
	done_num++;
}

int main(int argc, char const *argv[])
{
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    /* sa.sa_flags = 0; */
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror(0);
        exit(1);
    }
    assign_cpu_process(getpid(),0);
    process_t parent;
    parent.pid = getpid();
    wakeup_process(&parent);
	
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

	int next_ready_time;

if(policy == FIFO || policy == SJF){
	while(current_task<n){

		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
            main_counter++;
            clock_gettime(CLOCK_REALTIME,&task[current_task].p->start);

			heap_insert(task_heap, task[current_task].p);
            // consider interrupt case

			current_task++;
		}
		// if not running process, run the min process
		if(busy==0 && !isempty(task_heap)){
			cur_p = heap_extract_min(task_heap);
			exec_process(cur_p);
			busy = 1;
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
}else if(policy == RR){ // RR
	while(current_task<n || done_num<n){

		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
            main_counter++;
            clock_gettime(CLOCK_REALTIME,&task[current_task].p->start);

            // use sigpromask to avoid race condition, wait..... not needed
            if(busy==1){
			    heap_insert(task_heap, task[current_task].p);
            }
            else{
                exec_process(task[current_task].p);
                busy = 1;
                cur_p = task[current_task].p;
            }
            // consider interrupt case
			current_task++;
		}

		if(next_rr_time == now){ // only in RR, the next_rr_time will be number other than -1
			// interrupt the current process and insert it to heap
			interrupt(task_heap, cur_p); // change left_time and main_counter
			cur_p = heap_extract_min(task_heap); // change next_rr_time
			exec_process(cur_p);
		}

		// check once per unit
		unit();
		now++;

	}
}else{ // psjf
	while(current_task<n){

		if(busy==1){
			interrupt(task_heap, cur_p);
		}

		while(current_task<n && task[current_task].ready_time == now){
			// fork and mmap , a child can know where it is with current task
			task[current_task].p->pid = -1;
			task[current_task].p->counter = main_counter;
            		main_counter++;
            		clock_gettime(CLOCK_REALTIME,&task[current_task].p->start);
		
			heap_insert(task_heap, task[current_task].p);

			current_task++;
		}

		// if not running process, run the min process
		if(!isempty(task_heap)){
			cur_p = heap_extract_min(task_heap);
			exec_process(cur_p);
			busy = 1;
		}
		unit();
		now++;
	}
}
	
	// wait child
    while(waitpid((pid_t)-1,NULL,0)>0){}

    for(int i=0; i<n; i++){
        int idx = order[i];
        printf("%s %d\n", P[idx].name, P[idx].pid);
		syscall(335, P[idx].pid, &(P[idx].start), &(P[idx].end));
		/*
		printf("%s %d start=%09ld.%09ld end=%09ld.%09ld",P[idx].name,P[idx].pid,P[idx].start.tv_sec,
                P[idx].start.tv_nsec,P[idx].ptr->tv_sec,P[idx].ptr->tv_nsec);
        printf(" exec_time=%d\n",P[idx].exec_time);
    	*/
    }

	return 0;
}
