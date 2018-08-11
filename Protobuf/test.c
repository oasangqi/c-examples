#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "msg.pb-c.h"

#define MAX_LEN 1024

void submsg_serialize()
{
	SubMsg msg; 
	void	*buf = NULL;
	size_t	len, i;
	FILE *f = NULL;

	sub_msg__init(&msg); // 初始化要序列化的对象
	msg.name = "张三"; 
	msg.has_age = 1;
	msg.age = 25;
	msg.n_array = 2; // 设array这个数组有两个元素
	msg.array = malloc (sizeof(int) * msg.n_array); // Allocate memory to store int32
	if (msg.array == NULL) {
		printf("malloc error\n");
		exit(1);
	}
	for (i = 0; i < msg.n_array; i++) {
		msg.array[i] = i;
	}

	len = sub_msg__get_packed_size(&msg); // 计算序列化后的总体长度
	printf("len = %d\n", len);

	buf = malloc(sizeof(size_t) * len); // 开辟内存，用于存放序列化后的数据
	if (buf == NULL) {
		free(msg.array);
		printf("malloc error\n");
		exit(1);
	}

	sub_msg__pack(&msg, buf); // 进行序列化

	// 在这里，以写到文件中为例，如果是网络套接字，原理类似。
	if ((f = fopen("1.txt", "wb+")) == NULL) {
		printf("fopen error\n");
		free(msg.array);
		free(buf);
		exit(1);
	}
	fwrite(buf, len, 1, f);

	free(msg.array);
	free(buf); // 记得释放buf
	fclose(f);
}

void submsg_deserialize()
{
	SubMsg *msg = NULL; 
	size_t	cur_len = 0;
	size_t nread;
	FILE *f = NULL;
	uint8_t	buf[MAX_LEN] = {'\0'};
	int	i;

	if ((f = fopen("1.txt", "r")) == NULL) {
		printf("fopen error\n");
		exit(1);
	}

	while ((nread = fread(buf + cur_len, 1, MAX_LEN/2, f)) != 0) {
		cur_len += nread;
		if (cur_len > MAX_LEN) {
			printf("fread buf error\n");
			exit(1);
		}
	}

	cur_len += nread;
	fclose(f);

	// 对数据进行反序列化,得到原始未序列化的数据
	msg = sub_msg__unpack(NULL, cur_len, buf);
	if (msg == NULL) {
		printf("unpack error\n");
		exit(1);
	}

	printf("name = %s\n", msg->name);
	if (msg->has_age) {
		printf("age = %d\n", msg->age);
	}
	for (i = 0; i < msg->n_array; i++)
	{
		if (i == 0)
			printf ("array = ");
		if (i > 0)
			printf (", ");
		printf ("%d", msg->array[i]);
		if (i == msg->n_array -1)
			printf ("\n");
	}

	// 释放反序列化时开辟的内存
	sub_msg__free_unpacked(msg, NULL);
}

void mainmsg_serialize()
{
	SubMsg req_sub; 
	SubMsg opt_sub; 
	SubMsg **rep_sub = NULL; 
	MainMsg mainmsg;
	void	*buf = NULL;
	unsigned len, i;
	FILE *f = NULL;

	sub_msg__init(&req_sub);
	sub_msg__init(&opt_sub);
	main_msg__init(&mainmsg);

	// 因为rep_sub为数组指针，为其开辟内存（如果知道数组大小，也可以直接定义好数组） 
	rep_sub = malloc (sizeof(SubMsg*) * 5);
	for (i = 0; i < 2; i++)
	{
		rep_sub[i] = malloc(sizeof(SubMsg));
		sub_msg__init(rep_sub[i]);

		rep_sub[i]->name = "chuobazi"; 
		rep_sub[i]->has_age = 1;
		rep_sub[i]->age = 27 + 10 * i;
	}
	// 加入到mainmsg对象中
	mainmsg.n_rep_sub = 2;
	mainmsg.rep_sub = rep_sub;

	req_sub.name = "zhangsan"; 
	req_sub.has_age = 1;
	req_sub.age = 25;
	mainmsg.req_sub = &req_sub;

	opt_sub.name = "hanmeimei"; 
	opt_sub.has_age = 1;
	opt_sub.age = 30;
	// 在对象中内嵌子对象时，如果子对象为optional，则其在主对象中没有has_xxx的变量，
	// 在反序列化时，为了判断子对象是否存在与有值，可以根据子对象是否为NULL判断，参加反序列化。
	mainmsg.opt_sub = &opt_sub;

	len = main_msg__get_packed_size(&mainmsg);
	printf("len = %d\n", len);

	buf = malloc(sizeof(size_t) * len);
	if (buf == NULL) {
		printf("malloc error\n");
		exit(1);
	}

	main_msg__pack(&mainmsg, buf);

	if ((f = fopen("1.txt", "w+")) == NULL) {
		printf("fopen error\n");
		free(buf);
		exit(1);
	}
	fwrite(buf, len, 1, f);

	free(buf);
	fclose(f);
}

void mainmsg_deserialize()
{
	SubMsg *req_sub = NULL; 
	SubMsg *opt_sub = NULL; 
	SubMsg **rep_sub = NULL; 
	MainMsg *mainmsg = NULL;

	size_t	cur_len = 0;
	size_t nread;
	FILE *f = NULL;
	uint8_t	buf[MAX_LEN] = {'\0'};
	int	i;

	if ((f = fopen("1.txt", "r")) == NULL) {
		printf("fopen error\n");
		exit(1);
	}

	while ((nread = fread(buf + cur_len, 1, MAX_LEN/2, f)) != 0) {
		cur_len += nread;
		if (cur_len > MAX_LEN) {
			printf("fread buf error\n");
			exit(1);
		}
	}

	cur_len += nread;
	fclose(f);

	mainmsg = main_msg__unpack(NULL, cur_len, buf);
	if (mainmsg == NULL) {
		printf("unpack error\n");
		exit(1);
	}

	req_sub = mainmsg->req_sub;
	opt_sub = mainmsg->opt_sub;
	rep_sub = mainmsg->rep_sub;

	// req_sub
	printf("req_sub name = %s", req_sub->name);
	if (req_sub->has_age) {
		printf(", age = %d\n", req_sub->age);
	}
	for (i = 0; i < req_sub->n_array; i++)
	{
		if (i == 0)
			printf ("req_sub array = ");
		if (i > 0)
			printf (", ");
		printf ("%d", req_sub->array[i]);
		if (i == req_sub->n_array -1)
			printf ("\n");
	}
	// opt_sub
	if (opt_sub != NULL) {
		printf("opt_sub name = %s", opt_sub->name);
		if (opt_sub->has_age) {
			printf(", age = %d", opt_sub->age);
		}
		printf("\n");
	}
	// rep_sub
	printf("rep_sub list\n");
	for (i = 0; i < mainmsg->n_rep_sub; i++) {
		printf("name = %s", (rep_sub[i])->name);
		if (rep_sub[i]->has_age) {
			printf(", age = %d", (rep_sub[i])->age);
		}
		printf("\n");
	}

	main_msg__free_unpacked(mainmsg, NULL);
}

int main(int argc, char **argv)
{
#if 0
	submsg_serialize();
	//submsg_deserialize();
#endif
#if 1
	mainmsg_serialize();
	mainmsg_deserialize();
#endif
	return 0;
}

