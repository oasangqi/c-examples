#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "sendfd.h"

int main()
{
	int optval = 1;
	int unix_sock;
	struct sockaddr_un  unaddr;
	socklen_t	len; 
	pid_t pid;
	 int ret = 0;

	if ((unix_sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "create socket failed\n");
		return -1; 
	}   
	memset(&unaddr, 0, sizeof(struct sockaddr_un));
	unaddr.sun_family = AF_UNIX;
	strncpy(unaddr.sun_path, "/tmp/kbmd.sock", sizeof(unaddr.sun_path)-1);
#if 0
	if (connect(unix_sock, (struct sockaddr *)&unaddr, sizeof(unaddr.sun_path)) < 0) {
		fprintf(stderr, "connect failed, errno<%d>\n", errno);
	}
#endif
	unlink(unaddr.sun_path);
	bind(unix_sock, (struct sockaddr *)&unaddr, sizeof(struct sockaddr_un));
	listen(unix_sock, 5);
	struct sockaddr_un  cliaddr;

	len = sizeof(cliaddr);
	int connfd = accept(unix_sock, (struct sockaddr *)&cliaddr, &len);
	int newfd = recv_fd(connfd);

	char msgbuf[32];
	int msglen = 0;
	if((msglen = read(newfd, msgbuf, 32)) < 0) {
		printf("read from client failed\n");
		return -1; 
	} else {
		msgbuf[msglen] = '\0';
		printf("msg from client:[%s], size=[%d]\n"
				"input your message:", msgbuf, msglen);
		scanf("%s", msgbuf);
		//printf("%s\n", msgbuf);
		if (write(newfd, msgbuf, strlen(msgbuf)) < 0) {
			printf("write to client failed\n");
			return -1; 
		}   
	}  
	//write(unix_sock, "hello", strlen("hello"));
	//write(unix_sock, "hello", strlen("hello"));
	pause();
	return 0;
	if ((ret = fork()) < 0) {
		fprintf(stderr, "fork failed, errno<%d>\n", errno);
	} else if (ret == 0) { /*child*/
		close(unix_sock);
		sleep(20);
	} else { /*parent*/
		sleep(40);
	}
	return 0;
}
