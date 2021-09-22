/*
 * optimized memcpy for x86_64 with AVX.
 * beats musl 0.9.11, 1.1.24 C&ASM version,
 * beats cosmopolitan for all sizes
 * tested with GCC 6.5.0 -O2 -mavx.
 *
 * (C) 2021 rofl0r - licensed under the standard MIT license,
 * as shipped with musl.
 */

#include <string.h>
#include <stdint.h>

typedef uint8_t  u16 __attribute__ ((vector_size (16), aligned (1), __may_alias__));
typedef uint64_t u8  __attribute__((__may_alias__));
typedef uint32_t u4  __attribute__((__may_alias__));
typedef uint16_t u2  __attribute__((__may_alias__));
typedef uint8_t  u1  __attribute__((__may_alias__));

#if  __GNUC__ +0 <= 3
/* gcc 3.4.6 generates movaps instead of moveups, ignoring the aligned(1) */
#define COPY16(D, S) __builtin_ia32_storeups((void*)(D), __builtin_ia32_loadups((void*)(S)))
#define UNREACHABLE() break
#else
#define COPY16(D, S) do { *(u16*)(D) = *(u16*)(S); } while(0)
#define UNREACHABLE() __builtin_unreachable()
#endif

#ifdef __AVX__
#define COPY32(D, S) do { __builtin_ia32_storedqu256((void*)(D), __builtin_ia32_loaddqu256((void*)(S))); } while(0)
#endif

#define OP8(N) do {*(u8*)(d+N) = *(u8*)(s+N); } while(0)
#define OP4(N) do {*(u4*)(d+N) = *(u4*)(s+N); } while(0)
#define OP2(N) do {*(u2*)(d+N) = *(u2*)(s+N); } while(0)
#define OP1(N) do {*(u1*)(d+N) = *(u1*)(s+N); } while(0)

void *mymemcpy(void *__restrict dest, const void *__restrict src, size_t count)
{
	register u1 *  d /*__asm__("rdi")*/ = dest;
	register const u1 * s /*__asm__("rsi")*/ = src;
	register size_t n /*__asm__("rdx")*/ = count;
	register size_t c /*__asm__("rcx")*/ = n & 0xf;

	switch(c) {
	case 15: { OP8(0); OP4(8); OP2(12); OP1(14); } break;
	case 14: { OP8(0); OP4(8); OP2(12); } break;
	case 13: { OP8(0); OP4(8); OP1(12); } break;
	case 12: { OP8(0); OP4(8); } break;
	case 11: { OP8(0); OP2(8); OP1(10);} break;
	case 10: { OP8(0); OP2(8); } break;
	case 9:  { OP8(0); OP1(8); } break;
	case 8:  { OP8(0); } break;
	case 7:  { OP4(0); OP2(4); OP1(6);} break;
	case 6:  { OP4(0); OP2(4); } break;
	case 5:  { OP4(0); OP1(4); } break;
	case 4:  { OP4(0); } break;
	case 3:  { OP2(0); OP1(2); } break;
	case 2:  { OP2(0); } break;
	case 1:  { OP1(0); } break;
	case 0:  break;
	default: UNREACHABLE();
	}

	if(n & 0x10) {
		COPY16(d+c, s+c);
		c += 16;
	}

#ifdef COPY32
	for( ; c < n ; c += 32) COPY32(d+c, s+c);
#else
	for( ; c < n ; ) {
		COPY16(d+c, s+c);
		c += 16;
		COPY16(d+c, s+c);
		c += 16;
	}
#endif
	return dest;
}
