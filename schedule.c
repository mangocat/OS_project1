#include "schedule.h"
#include <stdio.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <assert.h>

heap_t *heap_create(int (*priority)(process_t *, process_t *)){
	heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
	assert(heap != NULL);
	heap->data = (process_t *)malloc(sizeof(process_t));
	assert(heap->data != NULL);
	heap->heap_len = heap->heap_size = 0;
	heap->priority = priority;
	return heap;
}

void insert(heap_t *heap, process_t *process) {
	/* dynamic array */
	if (heap->heap_len + 1 >= heap->heap_size) {
		heap->heap_size = heap->heap_size ? heap->heap_size * 2 : 8;
		heap->data = realloc(heap->data, sizeof(void *) * heap->heap_size);
	}
	
	int i = heap->heap_len;
	int j = (i - 1) / 2;
	while (i > 0 && heap->priority(process, &(heap->data[j]))) {
		heap->data[i] = heap->data[j];
		i = j;
		j = j / 2;
	}
	heap->data[i] = *process;
	heap->heap_len++;
}

process_t extract_min(heap_t *heap) {
	assert(heap->heap_len != 0);
	process_t data = heap->data[0];

	heap->data[0] = heap->data[heap->heap_len - 1];
	heap->heap_len--;

	int i = 0, j, k;
	while (i < heap->heap_len) {
		int left = i * 2 + 1;
		int right = i * 2 + 2;
		int highest = i;
		if (left < heap->heap_len && heap->priority(&(heap->data[left]), &(heap->data[highest]))) {
			highest = left;
		}
		if (right < heap->heap_len && heap->priority(&(heap->data[right]), &(heap->data[highest]))) {
			highest = right;
		}
		if (highest != i) {
			process_t temp = heap->data[i];
			heap->data[i] = heap->data[highest];
			heap->data[highest] = temp;
			i = highest;
		}
		else break;
	}
	return data;
}

process_t peek(heap_t *heap) {
	assert(heap->heap_len == 0);
	return heap->data[0];
}

int isempty(heap_t *heap) {
	return heap->heap_len == 0;
}

int priority(process_t *proc0, process_t *proc1){
	if(extern policy & 2){ // SJF or PSJF
		return proc0->left_time - proc1->left_time;
	}else{ // FIFO or RR
		return proc0->counter - proc1->counter;
	}
}

void interrupt(struct process *p)
{
    struct sched_param para;
    para.sched_priority = 0;

    if((sched_setscheduler(p->pid, SCHED_IDLE, para)) < 0){
        perror("interrupt error");
    }

    if(!(policy & 2)){
        queue_enqueue(p);
    }else{
        heap_insert(p);
    }

    return;
}
