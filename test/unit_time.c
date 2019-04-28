#include <stdio.h>
#include <stdlib.h>
#define unit() {for(volatile unsigned long i=0;i<1000000UL;i++);}
#define period(t) {for(int j=0;j<t;j++){unit();}}
int main(int argc, char *argv[]){
    int len = atoi(argv[1]);
    period(len);
    return 0;
}
