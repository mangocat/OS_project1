#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int policy;
int main(int argc, char const *argv[])
{
    // determine policy type
    char type[8];
    scanf("%s", type);
    if(type[0]=='F'){ // FIFO
        policy = FIFO;
    }else if(type[0]=='R'){ // R
        policy = RR;
    }else if(type[0]=='S'){ // SJF
        policy = SJF;
    }else if(type[0]=='P'){ // PSJF
        policy = PSJF;
    }else{
        printf("The input policy is not correct.\n");
    }

    // get number of processes
    int n, current=0;
    scanf("%d", &n);
    // get each line
    char name[32];
    int ready_time, total_time;
    for(int i=0;i<n;i++){
        scanf("%s%d%d", name, &ready_time, &total_time);
        // put the things into the waiting queue
    }

    // run!
    while(current<n){
        // check the next time need to fork
        int next_fork_time;
        // wait until the next for time
        for(int k=0;k<next_fork_time;k++)
            for(volatile unsigned long i=0;i<1000000UL;i++);
        // fork

    }


    return 0;
}
