#include <stdio.h>


#define TEST_MACRO 3

int main()
{
	int	a[TEST_MACRO] = {1,2,3};
	printf("%ld\n",sizeof(long unsigned int));
	printf("*(a+1)=%d\n",*(a+1));
	printf("sizeof(a)=%ld\n",sizeof(a));
	printf("sizeof(&a[3])=%ld\n",sizeof(&a[3]));
	printf("a[-1]=%d\t*(a-1)=%d\n",a[-1],*(a-1));
	printf("a=%p\t&a=%p\t&a[0]=%p\n",a,&a,&a[0]);
	printf("a=%p\t(a+1)=%p\t(&a+1)=%p\n",a,(a+1),(&a+1));
	return 0;
}
