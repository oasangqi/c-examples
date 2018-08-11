#include <stdio.h>
#include <time.h>

// 用于获取允许时间差
#ifdef __DEBUG_RTIME__
#define TIME_START(key) \
	        struct timespec ts_TB_##key; \
        struct timespec ts_TE_##key; \
        clock_gettime (CLOCK_MONOTONIC, &ts_TB_##key);
#define TIME_END(key)   \
	        clock_gettime (CLOCK_MONOTONIC, &ts_TE_##key); \
        printf("[%s:%s:%d]  costtime:[%ld]\n", __FILE__, __func__, __LINE__, ((ts_TE_##key.tv_sec - ts_TB_##key.tv_sec) * 1000 + (ts_TE_##key.tv_nsec - ts_TB_##key.tv_nsec) / 1000000));
#else
#define TIME_START(key) do { } while (0)    
#define TIME_END(key)   do { } while (0)
#endif

void current_time()
{
	time_t  iTime = time(NULL); // 0时区
	struct tm* pTM = localtime(&iTime); // 已转当前时区
	printf("%02d-%02d-%02d %02d:%02d:%02d\n", 1900 + pTM->tm_year, 1 + pTM->tm_mon, pTM->tm_mday, pTM->tm_hour, pTM->tm_min,
			            pTM->tm_sec);
}

void parse_time()
{
	struct tm   tmp; 
#define TIME "2015-11-27 15:26:00"

	strptime(TIME, "%Y-%m-%d %H:%M:%S %z", &tmp); // 从字符串解析时间
	printf("%02d-%02d-%02d %02d:%02d:%02d\n", 1900 + tmp.tm_year, 1 + tmp.tm_mon, tmp.tm_mday, tmp.tm_hour, tmp.tm_min,
			            tmp.tm_sec);
}

int main(int argc, char* argv[])
{
	current_time();
	parse_time();
	TIME_START(1);
	sleep(1);
	TIME_END(1);
	return 0;
}
