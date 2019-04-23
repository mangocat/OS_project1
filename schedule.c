#include "schedule.h"
#include <stdio.h>
#include <errno.h>
#include <sched.h>

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
