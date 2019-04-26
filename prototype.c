//implement min heap
//int insert(int flag, process* p); with flag
//FIFO: insert with counter flag=1
//RR:  insert with counter 
//SJF:insert with job time flag=2
//PSJF:insert with job time
//int extract_min(); return id
//int peek_min(); return id
//int isempty(); return 0, 1
//struct{
    //int process id, left_time, counter;
//}
//
//implement interrupt contain:
//sched_setscheduler(IDLE)
//insert() extract_min
//
//
//signal_handler : when child process terminate, run next process and record the execution time of it
//, we can use waitpid() in nonblocking mode, or we can catch SIGCHILD
//
//use global variable to record current running process

#include <stdlib.h>
#include <assert.h>

/* TODO */
struct process {
	pid_t pid;
	int id;
	int ready_time, exec_time;
	int remain_time;
};

typedef struct process process_t;


struct heap {
    process_t *data;
    int heap_len, heap_size;
	int (*priority)(process_t *, process_t *);
};

typedef struct heap heap_t;

heap_t *create_heap(int (*priority)(process_t *, process_t *)) {
	heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
	assert(heap != NULL);
	heap->data = (process_t *)malloc(sizeof(process_t));
	assert(heap->data != NULL);
	heap->heap_len = heap->heap_size = 0;
	heap->priority = priority;
	return heap;
}

int insert(heap_t *heap, process_t *process) {
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

process_t peek_min(heap_t *heap) {
	assert(heap->heap_len == 0);
	return heap->data[0];
}

int isempty(heap_t *heap) {
	return heap->heap_len == 0;
}

int main() {
	return 0;
}
