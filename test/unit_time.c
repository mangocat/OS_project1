#include <stdio.h>
#define unit() {for(volatile unsigned long i=0;i<1000000UL;i++);}
#define period(t) {for(int j=0;j<t;j++){unit();}}
int main(void){
    period(1000); 
    return 0;
}
