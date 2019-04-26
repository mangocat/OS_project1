#define FIFO    0
#define RR      1
#define SJF     2
#define PSJF    3

#define MAX_WAITING_NUM 8192

#define unit() {for(volatile unsigned long i=0;i<1000000UL;i++);}
#define period(t) {for(int j=0;j<t;j++){unit();}}

typedef struct process{
    pid_t pid; // -1 means haven't fork yet
    char name[32];
    int left_time, counter;
    struct timespec start, end;
}process_t;

typedef struct waiting_task{
    int ready_time;
    struct process *p;
}waiting_task_t;

typedef struct heap{
    struct process *data;
    int heap_len, heap_size;
    int (*priority)(struct process *, struct process *);
}heap_t;

// functions for heap

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

//other funtions

void interrupt(struct process *p);
