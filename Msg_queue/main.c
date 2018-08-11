#define _GNU_SOURCE //MSG_NOERROR需要定义该宏
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mymesg {
	long mtype; // 特别注意，消息类型必须为长整型
	char	mtext[512]; // 消息体
};

int main(int argc, char* argv[])
{
	key_t	key;

	key = ftok("./Makefile", 2); //生成key，用于生成消息队列ID
	if (key == -1) {
		return -1;
	}
	fprintf(stderr, "key<%ld>\n", key);

	int msgID= 0;
	msgID = msgget(key, IPC_CREAT|IPC_EXCL); //指定消息队列ID，IPC_EXCL选项类似open的O_EXCL
	//msgID = msgget(IPC_PRIVATE, IPC_CREAT); //自动生成消息队列ID
	fprintf(stderr, "msgID<%ld>\n", msgID);

	int	pid = 0;
	pid = fork();
	if (pid > 0) {//父进程
		struct mymesg msg;
		msg.mtype = 2;
		strcpy(msg.mtext, "msg queue test data");
		int ret = 0;

		return ;
		ret = msgsnd(msgID, &msg, strlen(msg.mtext), IPC_NOWAIT);
		fprintf(stderr, "parent send msg<%s> ret<%d> errno<%d>\n", msg.mtext, ret, errno);
	} else if(pid == 0) { //子进程
		sleep(2);
		struct mymesg msg;
		memset(&msg, 0, sizeof(msg));
		int ret = 0;

		ret = msgrcv(msgID, &msg, sizeof(msg.mtext), 1, MSG_EXCEPT | IPC_NOWAIT); // 获取类型为2的消息，配合MSG_EXCEPT可指定获取不为1的消息类型
		fprintf(stderr, "child recv msg<%s> ret<%d> errno<%d>\n", msg.mtext, ret, errno);
		msgctl(msgID, IPC_RMID, NULL); //删除指定的消息队列,消息队列位于操作系统内核，进程退出后不自动销毁, 还能根据消息队列ID获取消息
	} else { //出错
		return -1;
	}
	return 0;
}
