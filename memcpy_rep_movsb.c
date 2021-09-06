typedef unsigned long size_t;
void *mymemcpy(void *restrict dest, const void *restrict src, size_t n)
{
	__asm__("cld; rep; movsb" :: "S"(src),"D"(dest),"c"(n) : "memory");
	return dest;
}
