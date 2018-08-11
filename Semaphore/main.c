#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
	int val;			/* value for SETVAL */
	struct semid_ds *buf;	/* buffer for IPC_STAT & IPC_SET */
	unsigned short *array;	/* array for GETALL & SETALL */
	struct seminfo *__buf;	/* buffer for IPC_INFO */
	void *__pad;
};

int main(int argc, char* argv[])
{
	key_t	key;

	key = ftok("./Makefile", 2); //生成key，用于生成关联IPC对象的键
	if (key == -1) {
		return -1;
	}
	fprintf(stderr, "key<%ld>\n", key);

	int semset_id = 0;
	semset_id = semget(key, 2, IPC_CREAT|IPC_EXCL); //创建含两个信号量的信号量集合
	fprintf(stderr, "semset_id<%d>\n", semset_id);

	int ret = 0;
	union semun	su;
	su.val = 1;
	ret = semctl(semset_id, 0, SETVAL, su); //初始化信号量集合中的第0个信号量成员的semval为1
	su.val = 2;
	ret = semctl(semset_id, 1, SETVAL, su); //初始化信号量集合中的第1个信号量成员的semval为2
	// 此时可通过ipcs命令查看到多出了一个nsems为2的信号量集合

	struct sembuf	sops[2];
	int	pid = 0;
	pid = fork();
	if (pid > 0) {//父进程
		fprintf(stderr, "parent process start\n");

		// P
		sops[0].sem_num = 0; //集合中的第0个信号量
		sops[0].sem_op = -1;
		sops[0].sem_flg = SEM_UNDO;
		ret = semop(semset_id, sops, 1);
		if (ret) {
			fprintf(stderr, "parent process semop failed，%m\n");
			exit(1);
		}

		sleep(5);
		fprintf(stderr, "parent process wake up\n");

		// V
		sops[0].sem_num = 0;
		sops[0].sem_op = 1;
		sops[0].sem_flg = SEM_UNDO;
		ret = semop(semset_id, sops, 1);
		if (ret) {
			fprintf(stderr, "parent process semop failed，%m\n");
			exit(1);
		}
		semctl(semset_id, 0, IPC_RMID); //参数3为IPC_RMID时忽略参数2
		exit(0);

	} else if(pid == 0) { //子进程
		fprintf(stderr, "child process start\n");

		// P
		sops[0].sem_num = 0;
		sops[0].sem_op = -1;
		sops[0].sem_flg = SEM_UNDO;
		ret = semop(semset_id, sops, 1);
		if (ret) {
			fprintf(stderr, "child process semop failed，%m\n");
			exit(1);
		}
		fprintf(stderr, "child process wake up\n");
		exit(0);
	} else { //出错
		return -1;
	}
	return 0;
}
