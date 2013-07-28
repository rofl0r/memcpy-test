#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define SS (sizeof(size_t))
#define ALIGN (sizeof(size_t)-1)
#define ONES ((size_t)-1/UCHAR_MAX)

typedef struct block { unsigned char data[4*SS]; } block;

void *mymemcpy(void *restrict dest, const void *restrict src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	for (; n>=4*SS; n-=4*SS, s+=4*SS, d+=4*SS)
		*(block *)d = *(block *)s;
	for (; n; n--) *d++ = *s++;

	return dest;
}
