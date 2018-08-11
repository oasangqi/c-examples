#include <syslog.h>
#include <stdio.h>

int main(void)
{
	FILE *f;

	f = fopen("abc","r");
	if(!f)                                   
		syslog(LOG_ERR|LOG_USER,"test - %m/n");      
}
