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

extern int policy, now, next_rr_time;

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

    if(policy == RR){
        if(data->left_time < 500){
            next_rr_time = now + 500;
        }else{
            next_rr_time = -1;
        }
    }

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
    /* kill(p->pid,SIGUSR1); */
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
    /* kill(p->pid,SIGUSR2); */
    struct sched_param para;
    para.sched_priority = 0;
    int ret = sched_setscheduler(p->pid,SCHED_OTHER,&para);
    if(ret<0){
        perror("sched_setscheduler with OTHER error!");
        return -1;
    }
    return ret;
}
void sig_stop(int sig){
    puts("HI");
    pause();
}
void sig_cont(int sig){
    // nothing
    puts("sig_cont");
}
void child_running(struct process *p){
#ifdef SIGNAL
    struct sigaction stop,cont;
    stop.sa_handler = sig_stop;
    cont.sa_handler = sig_cont;
    stop.sa_flags = cont.sa_flags = 0;
    sigemptyset(&stop.sa_mask);
    sigemptyset(&cont.sa_mask);
    if(sigaction(SIGUSR1,&stop,NULL)<0){
        perror("failed when sigaction SIGUSR1!");
    }
    if(sigaction(SIGUSR2,&cont,NULL)<0){
        perror("failed when sigaction SIGUSR2!");
    }
    sigprocmask(SIG_SETMASK,0,NULL);
#endif
    period(p->left_time);
    clock_gettime(CLOCK_REALTIME,p->ptr);
    printf("(in child)%s end=%09ld.%09ld\n",p->name,p->ptr->tv_sec,p->ptr->tv_nsec);
    /* kill(getppid(),SIGPIPE); */
    // died , p->ptr store end time
}
void exec_process(struct process *p){
    if(p->pid==-1){ // the process haven't been forked
        // need to get the time process start running
        /* clock_gettime(CLOCK_REALTIME,&p->start); */
        p->ptr = (struct timespec*)mmap(NULL,sizeof(struct timespec),PROT_READ|PROT_WRITE,
                MAP_SHARED|MAP_ANONYMOUS,-1,0);
        // p->ptr is used for record child end time in child process
#ifdef SIGNAL
        sigset_t new,old;
        sigemptyset(&new);
        sigaddset(&new,SIGUSR1);
        sigaddset(&new,SIGUSR2);
        sigprocmask(SIG_BLOCK,&new,&old);
#endif
        pid_t tmp = fork();
        if(tmp<0){
            perror("Failed when fork()!");
        }
        else if(tmp==0){
            // child process
            child_running(p);
            exit(0);
        }
        else{ // parent process
            p->pid=tmp;
           proc_assign_cpu(p->pid,1);
           wakeup_process(p); 
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
