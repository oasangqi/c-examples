#define _GNU_SOURCE
#include <sched.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int lgcpus;

int get_affinity()
{
	cpu_set_t	cpuset;
	int i;

	CPU_ZERO(&cpuset);
	if (sched_getaffinity(getpid(), sizeof(cpu_set_t), &cpuset) < 0) {
		fprintf(stderr, "sched_getaffinity failed, errno<%d>\n", errno);
		return -1;
	}

	for (i = 0; i < lgcpus ; i++) {
		if (CPU_ISSET(i, &cpuset)) {
			fprintf(stderr, "get_affinity done, binded to CORE<%d>\n", i);
			break;
		}
	}
	return 0;
}

int set_affinity(int id)
{
	cpu_set_t	cpuset;

	if (id < 0 || id > lgcpus) {
		fprintf(stderr, "invalid CORE id<%d> lgcpus<%d>\n", id, lgcpus);
		return -1;
	}

	CPU_ZERO(&cpuset);
	CPU_SET(id, &cpuset);
	if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset) < 0) {
		fprintf(stderr, "sched_setaffinity failed, errno<%d>\n", errno);
		return -1;
	}
	fprintf(stderr, "set_affinity done, binded to CORE<%d>\n", id);
	return 0;
}

void busy_cpu()
{
	long i = 1, j = 1;

	for (i = 1; i < 500000000; i++) {
		j = j + sqrt(i);
	}
}

int main(int argc, char* argv[])
{
	int i = 0;

	lgcpus = sysconf(_SC_NPROCESSORS_CONF);
	fprintf(stderr, "<%d> cores\n", lgcpus);

	for (i = 0; i <= lgcpus; i++) {
		if (set_affinity(i) < 0) {
			continue;
		}
		get_affinity();
		busy_cpu();
	}

	return 0;
}
