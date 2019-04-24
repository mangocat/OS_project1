#define FIFO    0
#define RR      1
#define SJF     2
#define PSJF    3

#define MAX_WAITING_NUM 8192

#define unit() {for(volatile unsigned long i=0;i<1000000UL;i++);}
#define period(t) {for(int j=0;j<t;j++){unit();}}

struct process{
    pid_t pid; // -1 means haven't fork yet
    char name[32];
    int left_time, counter;
    struct timespec start, end;
}

struct waiting_task{
    int ready_time;
    struct process *p;
}

// functions for heap
void heap_insert(struct process *p);

struct process* heap_extract_min(void);

struct process* heap_peek_min(void);

int heap_isempty(void);

//functions for queue
void queue_enqueue(struct process *p);

struct process* queue_dequeue(void);

struct process* queue_peek(void);

int queue_isempty(void);


//other funtions

void interrupt(struct process *p);