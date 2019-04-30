#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>

SYSCALL_DEFINE3(printklog_0, int, pid, struct timespec, stime, struct timespec, ftime)
{
	printk("[Project1] %d %09ld.%09ld %09ld.%09ld\n", pid, stime.tv_sec, stime.tv_nsec, ftime.tv_sec, ftime.tv_nsec);
	return 0;
}

SYSCALL_DEFINE1(printklog_1, int, pid)
{
	printk("[Project1] %d\n", pid);
	return 0;
}

SYSCALL_DEFINE5(printklog_2, int, pid, int, stsec, int, stnsec, int, ftsec, int, ftnsec)
{
	printk("[Project1] %d %09d.%09d %09d.%09d\n", pid, stsec, stnsec, ftsec, ftnsec);
	return 0;
}

SYSCALL_DEFINE5(printklog_3, int, pid, long, stsec, long, stnsec, long, ftsec, long, ftnsec)
{
	printk("[Project1] %d %09ld.%09ld %09ld.%09ld\n", pid, stsec, stnsec, ftsec, ftnsec);
	return 0;
}

