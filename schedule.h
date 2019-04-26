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
    int left_time, counter; // counter specifies the order of the process in FIFO and RR
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

heap_t *heap_create(int (*priority)(process_t *, process_t *));

void insert(heap_t *heap, process_t *process);

process_t extract_min(heap_t *heap);

process_t peek(heap_t *heap); 

int isempty(heap_t *heap);

int priority(process_t *proc0, process_t *proc1);

//other funtions

void interrupt(struct process *p);
