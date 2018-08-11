#define _GNU_SOURCE //MSG_NOERROR需要定义该宏
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/user.h>


int main(int argc, char* argv[])
{
	key_t	key;
#if 0
	char*p=malloc(1024*1024*512);
	if (p==NULL) {
		fprintf(stderr, "malloc failed\n");
		return 0;
	}
	sleep(30);
	return 0;
#endif

	key = ftok("./Makefile", 9527); //生成key，用于获取共享内存ID
	if (key == -1) {
		return -1;
	}
	int shmID;
		
	shmID = shmget(key, PAGE_SIZE*2, 0);
	fprintf(stderr, "shmID<%ld>\n", shmID);

	void *shmptr = NULL;
	shmptr = shmat(shmID, 0, 0); //attache
	if (shmptr == (void*)-1) {
		fprintf(stderr, "shmat failed, errno<%d %m>\n", errno);
		return -1;
	}
	fprintf(stderr, "MEM<%s>\n", shmptr);
	shmdt(shmptr); //detache

	return 0;

}
