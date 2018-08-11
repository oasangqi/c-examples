#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <getopt.h>
#include <kpf_debug.h>
#include "session.h"

#define MYSQL_USERNAME	"kpf"
#define MYSQL_PASSWD	"qwer1234"
#define MYSQL_PORT	3456
#define SESSION_TIMEOUT 10

#define STOOL_MAX 50

typedef struct global_args_s {
	int	process;
	int threads;
	int times; //单个线程更新、查询次数
	char host[SESSION_IP_MAXLEN];
	char log[PATH_MAX];
} global_args_t;

global_args_t	g_args;

typedef struct global_vars_s {
	int stool[STOOL_MAX];
	pthread_mutex_t mutex_stool;
	int threads_exited;
	pthread_mutex_t mutex_threads_exited;
	pthread_mutex_t mutex_statistic;
	pthread_cond_t cond_statistic;
} global_vars_t;

global_vars_t	g_vars;

void *search_update_session(void *arg)
{
	int	ret = 0, loop = 0;
	session_handle	handle;
	session_cfg_t	cfg;
	session_t	ses;
	time_t	tm_old, tm_new;

	strncpy(cfg.host, g_args.host, SESSION_IP_MAXLEN);
	cfg.port = MYSQL_PORT;
	strncpy(cfg.username, MYSQL_USERNAME, SESSION_ID_MAXLEN);
	strncpy(cfg.password, MYSQL_PASSWD, SESSION_PWD_MAXLEN);
	ret = session_init(&handle, &cfg, SESSION_TIMEOUT, ALG_SHA1);
	if (ret != 0) {
		DEBUG(LOG_ERROR, "session_init failed, ret<0x%x>\n", ret);
		goto exit_rude;
	}

#if 0
	memset(&ses, 0, sizeof(ses));
	ses.uid = pthread_self();
	snprintf(ses.alias, SESSION_ID_MAXLEN, "T_%u", ses.uid);
	DEBUG(LOG_ERROR, "thread<%u> started\n", ses.uid);

	ret = session_add(handle, &ses);
	if (ret != 0) {
		DEBUG(LOG_ERROR, "session_add failed, ret<0x%x>\n", ret);
		goto exit_dirty;
	}
#endif
	ses.sid = *(int*)(arg);
	for(loop = 1; loop < g_args.times; loop++) {
		time(&tm_old);
		ses.last_time = tm_old;
		ret = session_query(handle, &ses, SESSION_QUERY_BY_SID);
		if (ret != 0) {
			DEBUG(LOG_ERROR, "session_query failed, ret<0x%x>\n", ret);
			goto exit_clean;
		}
		ret = session_update(handle, &ses, SESSION_UPDATE_LAST_TIME);
		if (ret != 0) {
			DEBUG(LOG_ERROR, "session_update failed, ret<0x%x>\n", ret);
			goto exit_clean;
		}
		time(&tm_new);
		pthread_mutex_lock(&g_vars.mutex_stool);
		//统计耗时
		if (tm_new - tm_old >= STOOL_MAX) {
			g_vars.stool[STOOL_MAX]++;
		} else {
			g_vars.stool[tm_new - tm_old]++;
		}
		pthread_mutex_unlock(&g_vars.mutex_stool);
		if (loop % 500 == 0) {
			DEBUG(LOG_ERROR, "thread<%u> update<%d>times\n", pthread_self(), loop);
		}
	}
exit_clean:
	//session_delete(handle, ses.sid);
exit_dirty:
	session_exit(handle);
exit_rude:
	DEBUG(LOG_INFO, "thread<%u> exit\n", pthread_self());
	pthread_mutex_lock(&g_vars.mutex_threads_exited);
	g_vars.threads_exited++;
	if (g_vars.threads_exited >= g_args.threads) {
		pthread_cond_signal(&g_vars.cond_statistic);
	}
	pthread_mutex_unlock(&g_vars.mutex_threads_exited);
	pthread_exit(NULL);
}

int parse_args(int argc, char *argv[])
{
	memset(&g_args, 0, sizeof(g_args));
	g_args.process = 2;
	g_args.threads = 20;
	g_args.times = 2000;
	strncpy(g_args.log, "log", strlen("log"));
	strncpy(g_args.host, "localhost", SESSION_IP_MAXLEN);

	int	c, index;
	char	*optstr = "Hp:t:r:h:";
	struct option	optarr[] = {
		{"help", no_argument, NULL, (int)'H'},
		{"process", required_argument, NULL, (int)'p'},
		{"thread", required_argument, NULL, (int)'t'},
		{"times", required_argument, NULL, (int)'r'},
		{"host", required_argument, NULL, (int)'h'},
		{NULL, 0, NULL, 0}
	};

	opterr = 0;
	do {
		c = getopt_long(argc, argv, optstr, optarr, &index);
		if (c != -1) {
			switch (c) {
				case 'H':
					fprintf(stderr, "usage: session -p<进程数> -t<进程内线程数> "
							"-r<每个线程执行次数> -h<session服务器IP>\n");
					exit(0);
				case 'p':
					g_args.process = atoi(optarg);
					break;
				case 't':
					g_args.threads = atoi(optarg);
					break;
				case 'r':
					g_args.times = atoi(optarg);
					break;
				case 'h':
					strncpy(g_args.host, optarg, SESSION_IP_MAXLEN);
					break;
				default:
					fprintf(stderr, "unknow option \n", c);
					return -1;
			}
		} else {	/* reach end */
			break;
		}
	} while (1);
}

int child_process()
{
	int threads = 0;
	int index = 0;
	int	ses_id = 0;
	pthread_t tid;
	pthread_attr_t attr;

	memset(&g_vars, 0, sizeof(g_vars));
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_mutex_init(&g_vars.mutex_stool, NULL);
	pthread_mutex_init(&g_vars.mutex_threads_exited, NULL);
	pthread_mutex_init(&g_vars.mutex_statistic, NULL);
	pthread_cond_init(&g_vars.cond_statistic, NULL);

	{
		int	ret = 0, loop = 0;
		session_handle	handle;
		session_cfg_t	cfg;
		session_t	ses;

		strncpy(cfg.host, g_args.host, SESSION_IP_MAXLEN);
		cfg.port = MYSQL_PORT;
		strncpy(cfg.username, MYSQL_USERNAME, SESSION_ID_MAXLEN);
		strncpy(cfg.password, MYSQL_PASSWD, SESSION_PWD_MAXLEN);
		ret = session_init(&handle, &cfg, SESSION_TIMEOUT, ALG_SHA1);
		if (ret != 0) {
			DEBUG(LOG_ERROR, "session_init failed, ret<0x%x>\n", ret);
			exit(-1);
		}

		memset(&ses, 0, sizeof(ses));
		ses.uid = pthread_self();
		snprintf(ses.alias, SESSION_ID_MAXLEN, "T_%u", ses.uid);

		ret = session_add(handle, &ses);
		if (ret != 0) {
			DEBUG(LOG_ERROR, "session_add failed, ret<0x%x>\n", ret);
			exit(-1);
		}
		ses_id = ses.sid;
		DEBUG(LOG_ERROR, "new session id<%d>\n", ses_id);
		session_exit(handle);
	}
	for (threads = 0; threads < g_args.threads; threads++) {
		pthread_create(&tid, &attr, search_update_session, &ses_id);
	}

	pthread_mutex_lock(&g_vars.mutex_statistic);
	pthread_cond_wait(&g_vars.cond_statistic, &g_vars.mutex_statistic);
	pthread_mutex_unlock(&g_vars.mutex_statistic);

	for (index = 0; index < STOOL_MAX - 1; index++) {
		if(g_vars.stool[index] > 0) {
			DEBUG(LOG_CRIT, "seconds<%d> times<%d>", 
					index, g_vars.stool[index]);
		}
	}
	DEBUG(LOG_CRIT, "seconds more than<%d> times<%d>", 
			STOOL_MAX, g_vars.stool[index]);

	close_log();
	return 0;
}

int start_childs()
{
	int pros;
	pid_t	pid;

	for (pros = 0; pros < g_args.process; pros++) {
		pid = fork();
		if (pid > 0) {//parent
		} else if (pid == 0) {//child
			child_process();
			exit(0);
		} else { //error
			DEBUG(LOG_ERROR, "fork failed");
		}
	}

	//wait
}

int main(int argc, char *argv[])
{
	//解析命令行参数
	parse_args(argc, argv);
	set_log_level(LOG_ERROR);
	set_log_output(OUTPUT_TO_FILE);
	open_log(g_args.log);
	start_childs();

	//创建子进程
	return 0;
}

