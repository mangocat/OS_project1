#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "schedule.h"
#include <stdio.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

heap_t *heap_create(int (*priority)(process_t *, process_t *)){
	heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
	assert(heap != NULL);
	heap->data = (process_t **)malloc(sizeof(process_t *));
	assert(heap->data != NULL);
	heap->heap_len = heap->heap_size = 0;
	heap->priority = priority;
	return heap;
}

void heap_insert(heap_t *heap, process_t *process) {
	/* dynamic array */
	if (heap->heap_len + 1 >= heap->heap_size) {
		heap->heap_size = heap->heap_size ? heap->heap_size * 2 : 8;
		heap->data = (process_t**)realloc(heap->data, sizeof(process_t *) * heap->heap_size);
	}
	
	int i = heap->heap_len;
	int j = (i - 1) / 2;
	while (i > 0 && heap->priority(process, heap->data[j]) >= 0) {
		heap->data[i] = heap->data[j];
		i = j;
		j = j / 2;
	}
	heap->data[i] = process;
	heap->heap_len++;
}

process_t *heap_extract_min(heap_t *heap) {
	assert(heap->heap_len != 0);
	process_t *data = heap->data[0];

	heap->data[0] = heap->data[heap->heap_len - 1];
	heap->heap_len--;

	int i = 0;
	while (i < heap->heap_len) {
		int left = i * 2 + 1;
		int right = i * 2 + 2;
		int highest = i;
		if (left < heap->heap_len && heap->priority(heap->data[left], heap->data[highest]) >= 0) {
			highest = left;
		}
		if (right < heap->heap_len && heap->priority(heap->data[right], heap->data[highest]) >= 0) {
			highest = right;
		}
		if (highest != i) {
			process_t *temp = heap->data[i];
			heap->data[i] = heap->data[highest];
			heap->data[highest] = temp;
			i = highest;
		}
		else break;
	}
	return data;
}

process_t *heap_peek(heap_t *heap) {
	assert(heap->heap_len != 0);
	return heap->data[0];
}

int isempty(heap_t *heap) {
	return heap->heap_len == 0;
}
int fifo_pri(process_t *proc0, process_t *proc1) {
	/* return proc0->ready_time - proc1->ready_time; */
    return proc1->counter - proc0->counter;
}
int rr_pri(process_t *proc0, process_t *proc1) {
	/* return proc1->exec_count - proc0->exec_count; */
    return proc1->counter - proc0->counter;
}
int sjf_pri(process_t *proc0, process_t *proc1) {
	return proc1->exec_time - proc0->exec_time;
}
int psjf_pri(process_t *proc0, process_t *proc1) {
	return proc1->left_time - proc0->left_time;
}

int block_process(struct process *p){
    struct sched_param para;
    para.sched_priority = 0;
    int ret = sched_setscheduler(p->pid,SCHED_IDLE,&para);
    if(ret<0){
        perror("sched_setscheduler with IDLE error!");
        return -1;
    }
    return ret;
    
}
int wakeup_process(struct process *p){
    struct sched_param para;
    para.sched_priority = 0;
    int ret = sched_setscheduler(p->pid,SCHED_OTHER,&para);
    if(ret<0){
        perror("sched_setscheduler with OTHER error!");
        return -1;
    }
    return ret;
}
void child_running(struct process *p){
    period(p->left_time);
    clock_gettime(CLOCK_REALTIME,p->ptr);
    /* kill(getppid(),SIGUSR1); */
    // died , p->ptr store end time
}
void exec_process(struct process *p){
    /* static long long cnt=0; */
    /* if(type[0]=='R'||type[0]=='F'){ */
        /* p->counter = cnt; */
    /* } */
    /* cnt++; */

    if(p->pid==-1){ // the process haven't been forked
        // need to get the time process start running
        clock_gettime(CLOCK_REALTIME,&p->start);
        p->ptr = (struct timespec*)mmap(NULL,sizeof(struct timespec),PROT_READ|PROT_WRITE,
                MAP_SHARED|MAP_ANONYMOUS,-1,0);
        // p->ptr is used for record child end time in child process
        p->pid = fork();
        if(p->pid<0){
            perror("Failed when fork()!");
        }
        else if(p->pid==0){
            // child process
            child_running(p);
            exit(0);
        }
        else{ // parent process
           proc_assign_cpu(p->pid,1);
           /* wakeup_process(p);  */
        }
    }
    else{ //just set to high priority
        wakeup_process(p);
    }
}
void interrupt(heap_t *heap, struct process *p)
{
    if(block_process(p)<0){
        perror("interrupt error");
    }
    //remeber to change counter or left_time before insert

    heap_insert(heap, p);

    return;
}
int proc_assign_cpu(int pid, int core)
{
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);

	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity");
		exit(1);
	}

	return 0;
}
