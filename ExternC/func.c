/* 必须包含自身头文件，告诉编译器按C语言进行编译 */
#include "func.h"
#include <stdio.h>

void func(int x, int y)
{
	fprintf(stderr, "func %d + %d = %d\n", x, y, x+y);
	return ;
}
