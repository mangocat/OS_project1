#include "schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
    /* heap_t *FIFO_test,*RR_test,*SJF_test,*PSJF_test; */
    /* FIFO_test = heap_create(fifo_pri); */
    /* RR_test = heap_create(rr_pri); */
    /* SJF_test = heap_create(sjf_pri); */
    /* PSJF_test = heap_create(psjf_pri); */
    heap_t *task_heap;
    char type[8];
	scanf("%s", type);
	if(type[0]=='F'){ // FIFO
		/* policy = FIFO; */
		task_heap = heap_create(fifo_pri);
	}else if(type[0]=='R'){ // R
		/* policy = RR; */
		task_heap = heap_create(rr_pri);
	}else if(type[0]=='S'){ // SJF
		/* policy = SJF; */
		task_heap = heap_create(sjf_pri);
	}else if(type[0]=='P'){ // PSJF
		/* policy = PSJF; */
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
		P[i].exec_time = P[i].ready_time;
		P[i].exec_count = 0;
        P[i].pid = -1;
		heap_insert(task_heap, &P[i]);
	}

    return 0;
}
