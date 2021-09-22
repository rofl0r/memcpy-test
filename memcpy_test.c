#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
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

static inline void serialize () {
    __asm__ __volatile__ ("cpuid" : : "a"(0) : "ebx", "ecx", "edx" );
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

#include <sched.h>
static void lock_affinity(void) {
	cpu_set_t cs;
	assert(0 == sched_getaffinity(0, sizeof(cs), &cs));
	CPU_ZERO_S(sizeof(cs), &cs);
	CPU_SET_S(0, sizeof(cs), &cs);
	assert(0 == sched_setaffinity(0, sizeof(cs), &cs));
}

static inline unsigned long test(size_t size, char* src, char* dst) {
	unsigned long ticka, tickb;
	fillmem(src, size); // dummy call so that gcc can not guess the mem contents.
	ticka = rdtsc();
	void* p = mymemcpy(dst, src, size);
	tickb = rdtsc();
	assert(p == dst);
	if(dummy_access(dst, size) == 1) abort(); // dummy call so that gcc can not assume mem content is never accessed.
	return tickb - ticka;
}

int main(int argc, char** argv) {
#define K(X) (1024UL * X)
#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))
#define MIN(A,B) ((A) > (B) ? (B) : (A))

	const size_t testsizes[] = {
		0, 1, 2, 3 ,4 ,5, 6, 7,
		8, 9, 10, 11, 12, 13, 14,
		15, 16,
		23, 24, 25,
		31, 32, 33,
		63, 64, 65,
		79, 80, 81,
		95, 96, 97,
		127, 128, 129,
		159, 160, 161,
		191, 192, 193,
		224,
		255, 256, 257,
		288, 320, 348,
		383, 384, 385,
		416, 448, 476,
		511, 512, 513,
		548, 640, 732,
		767, 768, 769,
		1023, 1024, 1025, 1152, 1280, 1408,
		1535, 1536, 1537,
		2048, 4096, 8192,
		16384, 32768, 65536,
		K(128), K(160), K(192), K(208), K(216), K(220), K(224), K(240),
		K(256), K(384), K(512),
		K(1024), K(1280), K(1536), K(1792), K(2048),
		K(2560), K(3072), K(3584), K(4096),
		K(4352), K(4608), K(4864),
		K(5120), K(5376), K(5632), K(5888),
		K(6144), K(6400), K(6656), K(6912), K(7168),
		K(8192), K(16384), K(32768), K(65536),
	};

	lock_affinity();

	unsigned long x, y, ymax;
	unsigned long long smallest;
	double res;

	// warm up
	for(x = 1 << 28; x > 0; --x);

	FILE *f = fopen("/dev/urandom", "r");
	for (x = 0 ; x < ARRAY_SIZE(testsizes); x++) {
		char *src, *dst;

		//smallest = 0xffffffff;
		y = testsizes[x] ? testsizes[x] : 1;
		ymax = (K(65536)*(100ULL - (MIN(99ULL, ARRAY_SIZE(testsizes) - x -1ULL))))/y;
		if(testsizes[x] >= 64) ymax*=2;
		if(testsizes[x] >= 1024) ymax*=2;

		src = malloc(testsizes[x] + 64);
		dst = malloc(testsizes[x] + 64);
		/* check that the function works correctly -
		   the +1 stuff is to get unaligned start offset,
		   the 0xaa/bb stuff to check whether it writes off bounds. */
		dst[0] = src[0] = 0xee;
		memset(src+1+testsizes[x], 0xbb, 32);
		memset(dst+1+testsizes[x], 0xaa, 32);
		fread(src+1, 1, testsizes[x], f);
		if(testsizes[x] >= 4) {
			/* make sure we copy into the right direction */
			src[1] = 'a'; src[2] = 'b'; src[3] = 'c'; src[4] = 'd';
			dst[1] = '0'; dst[2] = '1'; dst[3] = '2'; dst[4] = '3';
		}
		mymemcpy(dst+1, src+1, testsizes[x]);
		memset(src+1+testsizes[x], 0xaa, 32);
		if(memcmp(src+1, dst+1, testsizes[x]+32) || (testsizes[x] >= 4 && memcmp(dst+1, "abcd", 4))) {
			fprintf(stderr, "warning: %s didn't pass self-test with size %zu!\n", FILENAME, testsizes[x]);
		}

		unsigned long long curr, total, best = -1ULL;

		serialize();
		for(y = 0; y < ymax; y++) {
			//__builtin_ia32_clflush(src);
			//__builtin_ia32_clflush(dst);
			curr = test(testsizes[x], src, dst);
			if(curr < best) best = curr;
			total += curr;
		}
		serialize();

		fprintf(stdout, "%-8zu\t%llu\n", testsizes[x], best);
		fflush(stdout);
		free(src);
		free(dst);
	}
	fclose(f);
	return 0;
}



