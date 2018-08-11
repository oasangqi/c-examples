#define _GNU_SOURCE
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>

static int lgcpus;

int get_thread_affinity()
{
	cpu_set_t	cpuset;
	int i;

	CPU_ZERO(&cpuset);
	if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) < 0) {
		fprintf(stderr, "pthread_getaffinity_np failed, errno<%d>\n", errno);
		return -1;
	}

	for (i = 0; i < lgcpus ; i++) {
		if (CPU_ISSET(i, &cpuset)) {
			fprintf(stderr, "get_thread_affinity done, binded to CORE<%d>\n", i);
			break;
		}
	}
	return 0;
}

int set_thread_affinity(int id)
{
	cpu_set_t	cpuset;

	if (id < 0 || id > lgcpus) {
		fprintf(stderr, "invalid CORE id<%d> lgcpus<%d>\n", id, lgcpus);
		return -1;
	}

	CPU_ZERO(&cpuset);
	CPU_SET(id, &cpuset);
	if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) < 0) {
		fprintf(stderr, "pthread_setaffinity_np failed, errno<%d>\n", errno);
		return -1;
	}
	fprintf(stderr, "set_thread_affinity done, binded to CORE<%d>\n", id);
	return 0;
}

int set_thread_affinity_beta(int id)
{
	cpu_set_t	cpuset;
	pid_t	pid;

	if (id < 0 || id > lgcpus) {
		fprintf(stderr, "invalid CORE id<%d> lgcpus<%d>\n", id, lgcpus);
		return -1;
	}
	pid = syscall(SYS_gettid);
	fprintf(stderr, "gettid<%d>\n", pid);

	CPU_ZERO(&cpuset);
	CPU_SET(id, &cpuset);
	if (sched_setaffinity(pid, sizeof(cpu_set_t), &cpuset) < 0) {
		fprintf(stderr, "sched_setaffinity failed, errno<%d>\n", errno);
		return -1;
	}
	fprintf(stderr, "set_thread_affinity done, binded to CORE<%d>\n", id);
	return 0;
}

void busy_cpu()
{
	long i = 1, j = 1;

	for (i = 1; i < 500000000; i++) {
		j = j + sqrt(i);
	}
}

void *worker_thread(void *arg)
{
	/* 获取参数地址，即编号 */
	int id = (int)arg;

	fprintf(stderr, "id<%d>\n", id);
	if (set_thread_affinity_beta((int)id) < 0) {
		pthread_exit(-1);
	}
	get_thread_affinity();
	busy_cpu();
}

int main(int argc, char* argv[])
{
	int i = 0;
	pthread_t	tid;

	lgcpus = sysconf(_SC_NPROCESSORS_CONF);
	fprintf(stderr, "<%d> cores\n", lgcpus);

	for (i = 0; i < lgcpus; i++) {
		/* 将编号作为参数地址传入 */
		if (pthread_create(&tid, NULL, worker_thread, (void *)i) < 0) {
			fprintf(stderr, "pthread_create failed, errno<%d>\n", errno);
			break;
		}
		sleep(5);
	}
	pause();

	return 0;
}
