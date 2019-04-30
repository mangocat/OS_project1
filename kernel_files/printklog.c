#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/syscalls.h>


SYSCALL_DEFINE0(printklog, int, pid, struct timespec, stime, struct timespec, ftime)
{
	printk("[Project1] %d %09ld.%09ld %09ld.%09ld\n", pid, stime.tv_sec, stime.tv_nsec, ftime.tv_sec, ftime.tv_nsec);

	return 0;
}
