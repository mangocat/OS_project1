#include <linux/kernel.h>
#include <time.h>

asmlinkage void sys_print(char* tag, int pid, struct timespec stime, struct timespec ftime)
{
	printk("[%s] %d %09ld.%09ld %09ld.%09ld\n", tag, pid, stime.tv_sec, stime.tv_nsec, ftime.tv_sec, ftime.tv_nsec);
	return;
}

