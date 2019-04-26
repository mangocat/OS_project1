#include <stdlib.h>

int fifo_pri(process_t *proc0, process_t *proc1) {
	return proc0->id - proc1->id;
}
int rr_pri(process_t *proc0, process_t *proc1) {
	/*  */
}
int sjf_pri(process_t *proc0, process_t *proc1) {
	return proc0->exec_time - proc1->exec_time;
}
int psjf_pri(process_t *proc0, process_t *proc1) {
	return proc0->remain_time - proc1->remain_time;
}
