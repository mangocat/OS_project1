#include <stdlib.h>

int priority(process_t *proc0, process_t *proc1){
	if(extern policy & 2){ // SJF or PSJF
		return proc0->left_time - proc1->left_time;
	}else{ // FIFO or RR
		return proc0->counter - proc1->counter;
	}
}

/*
int fifo_pri(process_t *proc0, process_t *proc1) {
	return proc0->id - proc1->id;
}
int rr_pri(process_t *proc0, process_t *proc1) {
	//  
}
int sjf_pri(process_t *proc0, process_t *proc1) {
	return proc0->exec_time - proc1->exec_time;
}
int psjf_pri(process_t *proc0, process_t *proc1) {
	return proc0->remain_time - proc1->remain_time;
}
*/
