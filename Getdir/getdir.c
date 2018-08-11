#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define SYSERRMSG	strerror(errno)

int main()
{
	struct dirent	**list = NULL;
	int	n = 0, i = 0;

	if ((n = scandir("./", &list, 0, alphasort)) < 0) {
		fprintf(stderr, "scandir failed, err<%s>\n", SYSERRMSG);
		return -1;
	}
	fprintf(stderr, "DDDDDDDDDDDDDDDDDDDDDD\n");
	for (i = 0; i < n; i++) {
		if (list[i]->d_type == DT_DIR) {
			fprintf(stderr, "name<%s> type<%d>\n", list[i]->d_name, list[i]->d_type);
		}
	}
	fprintf(stderr, "FFFFFFFFFFFFFFFFFFFFFF\n");
	for (i = 0; i < n; i++) {
		if (list[i]->d_type == DT_REG) {
			fprintf(stderr, "name<%s> type<%d>\n", list[i]->d_name, list[i]->d_type);
		}
	}
	for (i = 0; i < n; i++) {
		free(list[i]);
	}
	free(list);
	return 0;
}
