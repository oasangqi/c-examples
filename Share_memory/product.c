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
	int ret = 0;

	key = ftok("./Makefile", 9527); //生成key，用于生成共享内存ID
	if (key == -1) {
		return -1;
	}
	fprintf(stderr, "key<%d>\n", key);

	int shmID;
		
	//shmID = shmget(key, PAGE_SIZE*2, IPC_CREAT | IPC_EXCL);
	shmID = shmget(key, 1024*1024*10, IPC_CREAT | IPC_EXCL);
	fprintf(stderr, "shmID<%d>\n", shmID);

	void *shmptr = NULL;
	shmptr = shmat(shmID, 0, 0); //attache
	if (shmptr == (void*)-1) {
		fprintf(stderr, "shmat failed, errno<%d %m>\n", errno);
		return -1;
	}
	//memset(shmptr, 1, 1024*1024*5);
	char buff[1024*1024*2];
	char *p = malloc(1024*1024*512);
	if (p == NULL) {
		fprintf(stderr, "malloc failed!\n");
	} else {
		memset(p, 1, 1024*1024*50);
		memset(buff, 1, 1024*1024);
		strcpy(shmptr, "hello share memory");
		sleep(20);
	}
	//shmdt(shmptr); //detache
	ret = shmctl(shmID, IPC_RMID, NULL); //删除共享内存
	if (ret == -1) {
		fprintf(stderr, "delete share memory failed, errno<%d %m>\n", errno);
		return -1;
	}
	return 0;

}
