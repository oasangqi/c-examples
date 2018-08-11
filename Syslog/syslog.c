#include <syslog.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int logmask;

	openlog("mail", LOG_PID|LOG_CONS, LOG_MAIL); /*日志信息会包含进程id。*/
	syslog(LOG_INFO, "informative message, pid=%d", getpid());
	syslog(LOG_DEBUG,"debug message, should appear");   /*记录该日志信息。*/
	logmask = setlogmask(LOG_UPTO(LOG_NOTICE));     /*设置屏蔽低于NOTICE级别的日志信息。*/
	syslog(LOG_DEBUG, "debug message, should not appear");  /*该日志信息被屏蔽，不记录。*/
}
