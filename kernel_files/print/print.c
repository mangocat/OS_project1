#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/linkage.h>

asmlinkage int __x64_sys_printlog(char* tag, int pid, struct timespec stime, struct timespec ftime)
{
	printk("[%s] %d %09ld.%09ld %09ld.%09ld\n", tag, pid, stime.tv_sec, stime.tv_nsec, ftime.tv_sec, ftime.tv_nsec);
	printk("PRINTK project1");	

	return 0;
}

