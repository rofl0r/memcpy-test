#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/time.h>

unsigned long long microsecpassed(struct timeval* t) {
	struct timeval now, diff;
	gettimeofday(&now, NULL);
	timersub(&now, t, &diff);
	return (diff.tv_sec * 1000 * 1000)  + diff.tv_usec;
}

#if defined(__i386__)

inline unsigned long long rdtsc() {
  unsigned int lo, hi;
  __asm__ volatile (
     "cpuid \n"
     "rdtsc" 
   : "=a"(lo), "=d"(hi) /* outputs */
   : "a"(0)             /* inputs */
   : "%ebx", "%ecx");     /* clobbers*/
  return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}
#elif 0
static inline unsigned long long rdtsc(void) {
    unsigned long long hi, lo;
    __asm__ __volatile__(
            "xorl %%eax, %%eax;\n\t"
            "push %%ebx;"
            "cpuid\n\t"
            ::
            :"%eax", "%ebx", "%ecx", "%edx");
    __asm__ __volatile__(
            "rdtsc;"
            : "=a" (lo),  "=d" (hi)
            ::);
    __asm__ __volatile__(
            "xorl %%eax, %%eax; cpuid;"
            "pop %%ebx;"
            ::
            :"%eax", "%ebx", "%ecx", "%edx");
   
    return (unsigned long long)hi << 32 | lo;
}

#elif 0
static inline unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
     return x;
}
#elif defined(__x86_64__)
/*static inline unsigned long long rdtsc(void) {
    unsigned long long hi, lo;
    __asm__ __volatile__(
            "xorl %%eax, %%eax;\n\t"
            "push %%rbx;"
            "cpuid\n\t"
            ::
            :"%rax", "%rbx", "%rcx", "%rdx");
    __asm__ __volatile__(
            "rdtsc;"
            : "=a" (lo),  "=d" (hi)
            ::);
    __asm__ __volatile__(
            "xorl %%eax, %%eax; cpuid;"
            "pop %%rbx;"
            ::
            :"%rax", "%rbx", "%rcx", "%rdx");
   
    return (unsigned long long)hi << 32 | lo;
}
#elif 0
*/
static inline unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#elif defined(__powerpc__)


static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int result=0;
  unsigned long int upper, lower,tmp;
  __asm__ volatile(
                "0:                  \n"
                "\tmftbu   %0           \n"
                "\tmftb    %1           \n"
                "\tmftbu   %2           \n"
                "\tcmpw    %2,%0        \n"
                "\tbne     0b         \n"
                : "=r"(upper),"=r"(lower),"=r"(tmp)
                );
  result = upper;
  result = result<<32;
  result = result|lower;

  return(result);
}

#endif


extern void *mymemcpy(void *dest, const void *src, size_t n);
extern void fillmem(void* mem, size_t size);
extern int dummy_access(void* mem, size_t size);


static long long counter = 0;
static inline unsigned long test(size_t size) {
	char *src, *dst;
	long result;
	long counter_mod_4;
	
	if(size > 4)
		counter_mod_4 = counter % 4;
	else 
		counter_mod_4 = 0;
	
	struct timeval start;
	
	
	src = malloc(size);
	dst = malloc(size);
	
	fillmem(src, size); // dummy call so that gcc can not guess the mem contents.

	
	//gettimeofday(&start, NULL);
	unsigned long long ticka = rdtsc(), tickb;
	mymemcpy(dst + counter_mod_4, src + counter_mod_4, size - counter_mod_4);
	tickb = rdtsc();
	result = tickb - ticka;
	//result = microsecpassed(&start);
	
	if(dummy_access(dst, size) == 1) abort(); // dummy call so that gcc can not assume mem content is never accessed.
	
	counter ++;
	free(src);
	free(dst);
	return result;
}

int main(int argc, char** argv) {
#define K(X) (1024 * X)
#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

	size_t testsizes[] = {
		3 ,4 ,5,
		8, 
		15, 16,
		23, 24, 25, 
		31, 32, 33,
		63, 64, 65,
		95, 96, 97,
		127, 128, 129,
		159, 160, 161,
		191, 192, 193,
		255, 256, 257,
		383, 384, 385,
		511, 512, 513,
		767, 768, 769,
		1023, 1024, 1025,
		1535, 1536, 1537,
		2048, 4096, 8192,
		16384, 32768, 65536,
		K(128), K(256), K(512),
		K(1024), K(2048), K(4096),
		K(8192), K(16384), K(32768), K(65536),
	};
		
	unsigned long x, y, ymax;
	unsigned long long res, smallest;
	for (x = 0 ; x < ARRAY_SIZE(testsizes); x++) {
		//smallest = 0xffffffff;
		res = 0;
		ymax = testsizes[x] > K(100) ? 100 : 10000;
		for(y = 0; y < ymax; y++) {
			res += test(testsizes[x]);
			//if(res < smallest) smallest = res;
			
		}
		res /= ymax;
		fprintf(stdout, "%zu\t%lu ticks\n", testsizes[x], res);
		fflush(stdout);
	}
	return 0;
}



