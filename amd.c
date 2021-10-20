/*
 * Copyright (C) 2019-2020, Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <stdlib.h>

#undef memcpy

void * mymemcpy(void *dest, const void *src, size_t size)
{
	asm goto (
	"movq	%0, %%rsi\n\t"
	"movq	%1, %%rdi\n\t"
	"movq	%2, %%rdx\n\t"
	"movq    %%rdi, %%rax\n\t"
	"cmp     $32, %%rdx\n\t"
	"jb      less_vec\n\t"
	"cmp     $(32 * 2), %%rdx\n\t"
	"ja      more_2x_vec\n\t"
	"vmovdqu   (%%rsi), %%ymm0\n\t"
	"vmovdqu   -32(%%rsi,%%rdx), %%ymm1\n\t"
	"vmovdqu   %%ymm0, (%%rdi)\n\t"
	"vmovdqu   %%ymm1, -32(%%rdi,%%rdx)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"
	"less_vec:\n\t"
	/* Less than 1 VEC.  */
	"cmpb    $32, %%dl\n\t"
	"jae     between_32_63\n\t"
	"cmpb    $16, %%dl\n\t"
	"jae     between_16_31\n\t"
	"cmpb    $8, %%dl\n\t"
	"jae     between_8_15\n\t"
	"cmpb    $4, %%dl\n\t"
	"jae     between_4_7\n\t"
	"cmpb    $1, %%dl\n\t"
	"ja      between_2_3\n\t"
	"jb      1f\n\t"
	"movzbl  (%%rsi), %%ecx\n\t"
	"movb    %%cl, (%%rdi)\n\t"
	"1:\n\t"
	"jmp %l[done]\n\t"
	"between_32_63:\n\t"
	/* From 32 to 63.  No branch when size == 32.  */
	"vmovdqu (%%rsi), %%ymm0\n\t"
	"vmovdqu -32(%%rsi,%%rdx), %%ymm1\n\t"
	"vmovdqu %%ymm0, (%%rdi)\n\t"
	"vmovdqu %%ymm1, -32(%%rdi,%%rdx)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"
	/* From 16 to 31.  No branch when size == 16.  */
	"between_16_31:\n\t"
	"vmovdqu (%%rsi), %%xmm0\n\t"
	"vmovdqu -16(%%rsi,%%rdx), %%xmm1\n\t"
	"vmovdqu %%xmm0, (%%rdi)\n\t"
	"vmovdqu %%xmm1, -16(%%rdi,%%rdx)\n\t"
	"jmp %l[done]\n\t"
	"between_8_15:\n\t"
	/* From 8 to 15.  No branch when size == 8.  */
	"movq    -8(%%rsi,%%rdx), %%rcx\n\t"
	"movq    (%%rsi), %%rsi\n\t"
	"movq    %%rcx, -8(%%rdi,%%rdx)\n\t"
	"movq    %%rsi, (%%rdi)\n\t"
	"jmp %l[done]\n\t"
	"between_4_7:\n\t"
	/* From 4 to 7.  No branch when size == 4.  */
	"movl    -4(%%rsi,%%rdx), %%ecx\n\t"
	"movl    (%%rsi), %%esi\n\t"
	"movl    %%ecx, -4(%%rdi,%%rdx)\n\t"
	"movl    %%esi, (%%rdi)\n\t"
	"jmp %l[done]\n\t"
	"between_2_3:\n\t"
	/* From 2 to 3.  No branch when size == 2.  */
	"movzwl  -2(%%rsi,%%rdx), %%ecx\n\t"
	"movzwl  (%%rsi), %%esi\n\t"
	"movw    %%cx, -2(%%rdi,%%rdx)\n\t"
	"movw    %%si, (%%rdi)\n\t"
	"jmp %l[done]\n\t"
	"more_2x_vec:\n\t"
	/* More than 2 * VEC and there may be overlap between destination */
	/* and source.  */
	"cmpq    $(32 * 8), %%rdx\n\t"
	"ja      more_8x_vec\n\t"
	"cmpq    $(32 * 4), %%rdx\n\t"
	"jb      last_4x_vec\n\t"
	/* Copy from 4 * VEC to 8 * VEC, inclusively. */
	"vmovdqu   (%%rsi), %%ymm0\n\t"
	"vmovdqu   32(%%rsi), %%ymm1\n\t"
	"vmovdqu   (32 * 2)(%%rsi), %%ymm2\n\t"
	"vmovdqu   (32 * 3)(%%rsi), %%ymm3\n\t"
	"vmovdqu   -32(%%rsi,%%rdx), %%ymm4\n\t"
	"vmovdqu   -(32 * 2)(%%rsi,%%rdx), %%ymm5\n\t"
	"vmovdqu   -(32 * 3)(%%rsi,%%rdx), %%ymm6\n\t"
	"vmovdqu   -(32 * 4)(%%rsi,%%rdx), %%ymm7\n\t"
	"vmovdqu   %%ymm0, (%%rdi)\n\t"
	"vmovdqu   %%ymm1, 32(%%rdi)\n\t"
	"vmovdqu   %%ymm2, (32 * 2)(%%rdi)\n\t"
	"vmovdqu   %%ymm3, (32 * 3)(%%rdi)\n\t"
	"vmovdqu   %%ymm4, -32(%%rdi,%%rdx)\n\t"
	"vmovdqu   %%ymm5, -(32 * 2)(%%rdi,%%rdx)\n\t"
	"vmovdqu   %%ymm6, -(32 * 3)(%%rdi,%%rdx)\n\t"
	"vmovdqu   %%ymm7, -(32 * 4)(%%rdi,%%rdx)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"
	"last_4x_vec:\n\t"
	/* Copy from 2 * VEC to 4 * VEC. */
	"vmovdqu   (%%rsi), %%ymm0\n\t"
	"vmovdqu   32(%%rsi), %%ymm1\n\t"
	"vmovdqu   -32(%%rsi,%%rdx), %%ymm2\n\t"
	"vmovdqu   -(32 * 2)(%%rsi,%%rdx), %%ymm3\n\t"
	"vmovdqu   %%ymm0, (%%rdi)\n\t"
	"vmovdqu   %%ymm1, 32(%%rdi)\n\t"
	"vmovdqu   %%ymm2, -32(%%rdi,%%rdx)\n\t"
	"vmovdqu   %%ymm3, -(32 * 2)(%%rdi,%%rdx)\n\t"
	"vzeroupper\n\t"
	"nop:\n\t"
	"jmp %l[done]\n\t"
	"more_8x_vec:\n\t"
	"cmpq    %%rsi, %%rdi\n\t"
	"ja      more_8x_vec_backward\n\t"
	/* Source == destination is less common.  */
	"je      nop\n\t"
	/* Load the first VEC and last 4 * VEC to support overlapping addresses.  */
	"vmovdqu   (%%rsi), %%ymm4\n\t"
	"vmovdqu   -32(%%rsi, %%rdx), %%ymm5\n\t"
	"vmovdqu   -(32 * 2)(%%rsi, %%rdx), %%ymm6\n\t"
	"vmovdqu   -(32 * 3)(%%rsi, %%rdx), %%ymm7\n\t"
	"vmovdqu   -(32 * 4)(%%rsi, %%rdx), %%ymm8\n\t"
	/* Save start and stop of the destination buffer.  */
	"movq    %%rdi, %%r11\n\t"
	"leaq    -32(%%rdi, %%rdx), %%rcx\n\t"
	/* Align destination for aligned stores in the loop.  Compute */
	/* how much destination is misaligned.  */
	"movq    %%rdi, %%r8\n\t"
	"andq    $(32 - 1), %%r8\n\t"
	/* Get the negative of offset for alignment.  */
	"subq    $32, %%r8\n\t"
	/* Adjust source.  */
	"subq    %%r8, %%rsi\n\t"
	/* Adjust destination which should be aligned now.  */
	"subq    %%r8, %%rdi\n\t"
	/* Adjust length.  */
	"addq    %%r8, %%rdx\n\t"
	/* Check non-temporal store threshold.  */
	"cmpq	 $(1024*1024), %%rdx\n\t"
	"ja      large_forward\n\t"
	"loop_4x_vec_forward:\n\t"
	/* Copy 4 * VEC a time forward.  */
	"vmovdqu   (%%rsi), %%ymm0\n\t"
	"vmovdqu   32(%%rsi), %%ymm1\n\t"
	"vmovdqu   (32 * 2)(%%rsi), %%ymm2\n\t"
	"vmovdqu   (32 * 3)(%%rsi), %%ymm3\n\t"
	"addq    $(32 * 4), %%rsi\n\t"
	"subq    $(32 * 4), %%rdx\n\t"
	"vmovdqa   %%ymm0, (%%rdi)\n\t"
	"vmovdqa   %%ymm1, 32(%%rdi)\n\t"
	"vmovdqa   %%ymm2, (32 * 2)(%%rdi)\n\t"
	"vmovdqa   %%ymm3, (32 * 3)(%%rdi)\n\t"
	"addq    $(32 * 4), %%rdi\n\t"
	"cmpq    $(32 * 4), %%rdx\n\t"
	"ja      loop_4x_vec_forward\n\t"
	/* Store the last 4 * VEC.  */
	"vmovdqu   %%ymm5, (%%rcx)\n\t"
	"vmovdqu   %%ymm6, -32(%%rcx)\n\t"
	"vmovdqu   %%ymm7, -(32 * 2)(%%rcx)\n\t"
	"vmovdqu   %%ymm8, -(32 * 3)(%%rcx)\n\t"
	/* Store the first VEC.  */
	"vmovdqu   %%ymm4, (%%r11)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"
	"more_8x_vec_backward:\n\t"
	/* Load the first 4*VEC and last VEC to support overlapping addresses.*/
	"vmovdqu   (%%rsi), %%ymm4\n\t"
	"vmovdqu   32(%%rsi), %%ymm5\n\t"
	"vmovdqu   (32 * 2)(%%rsi), %%ymm6\n\t"
	"vmovdqu   (32 * 3)(%%rsi), %%ymm7\n\t"
	"vmovdqu   -32(%%rsi,%%rdx), %%ymm8\n\t"
	/* Save stop of the destination buffer.  */
	"leaq    -32(%%rdi, %%rdx), %%r11\n\t"
	/* Align destination end for aligned stores in the loop.  Compute */
	/* how much destination end is misaligned.  */
	"leaq    -32(%%rsi, %%rdx), %%rcx\n\t"
	"movq    %%r11, %%r9\n\t"
	"movq    %%r11, %%r8\n\t"
	"andq    $(32 - 1), %%r8\n\t"
	/* Adjust source.  */
	"subq    %%r8, %%rcx\n\t"
	/* Adjust the end of destination which should be aligned now.  */
	"subq    %%r8, %%r9\n\t"
	/* Adjust length.  */
	"subq    %%r8, %%rdx\n\t"
	 /* Check non-temporal store threshold.  */
	"cmpq	 $(1024*1024), %%rdx\n\t"
	"ja      large_backward\n\t"
	"loop_4x_vec_backward:\n\t"
	/* Copy 4 * VEC a time backward.  */
	"vmovdqu   (%%rcx), %%ymm0\n\t"
	"vmovdqu   -32(%%rcx), %%ymm1\n\t"
	"vmovdqu   -(32 * 2)(%%rcx), %%ymm2\n\t"
	"vmovdqu   -(32 * 3)(%%rcx), %%ymm3\n\t"
	"subq    $(32 * 4), %%rcx\n\t"
	"subq    $(32 * 4), %%rdx\n\t"
	"vmovdqa   %%ymm0, (%%r9)\n\t"
	"vmovdqa   %%ymm1, -32(%%r9)\n\t"
	"vmovdqa   %%ymm2, -(32 * 2)(%%r9)\n\t"
	"vmovdqa   %%ymm3, -(32 * 3)(%%r9)\n\t"
	"subq    $(32 * 4), %%r9\n\t"
	"cmpq    $(32 * 4), %%rdx\n\t"
	"ja      loop_4x_vec_backward\n\t"
	/* Store the first 4 * VEC. */
	"vmovdqu   %%ymm4, (%%rdi)\n\t"
	"vmovdqu   %%ymm5, 32(%%rdi)\n\t"
	"vmovdqu   %%ymm6, (32 * 2)(%%rdi)\n\t"
	"vmovdqu   %%ymm7, (32 * 3)(%%rdi)\n\t"
	/* Store the last VEC. */
	"vmovdqu   %%ymm8, (%%r11)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"

	"large_forward:\n\t"
	/* Don't use non-temporal store if there is overlap between */
	/* destination and source since destination may be in cache */
	/* when source is loaded. */
	"leaq    (%%rdi, %%rdx), %%r10\n\t"
	"cmpq    %%r10, %%rsi\n\t"
	"jb      loop_4x_vec_forward\n\t"
	"loop_large_forward:\n\t"
	/* Copy 4 * VEC a time forward with non-temporal stores.  */
	"prefetcht0 (32*4*2)(%%rsi)\n\t"
	"prefetcht0 (32*4*2 + 64)(%%rsi)\n\t"
	"prefetcht0 (32*4*3)(%%rsi)\n\t"
	"prefetcht0 (32*4*3 + 64)(%%rsi)\n\t"
	"vmovdqu   (%%rsi), %%ymm0\n\t"
	"vmovdqu   32(%%rsi), %%ymm1\n\t"
	"vmovdqu   (32 * 2)(%%rsi), %%ymm2\n\t"
	"vmovdqu   (32 * 3)(%%rsi), %%ymm3\n\t"
	"addq    $(32*4), %%rsi\n\t"
	"subq    $(32*4), %%rdx\n\t"
	"vmovntdq  %%ymm0, (%%rdi)\n\t"
	"vmovntdq  %%ymm1, 32(%%rdi)\n\t"
	"vmovntdq  %%ymm2, (32 * 2)(%%rdi)\n\t"
	"vmovntdq  %%ymm3, (32 * 3)(%%rdi)\n\t"
	"addq    $(32*4), %%rdi\n\t"
	"cmpq    $(32*4), %%rdx\n\t"
	"ja      loop_large_forward\n\t"
	"sfence\n\t"
	/* Store the last 4 * VEC.  */
	"vmovdqu   %%ymm5, (%%rcx)\n\t"
	"vmovdqu   %%ymm6, -32(%%rcx)\n\t"
	"vmovdqu   %%ymm7, -(32 * 2)(%%rcx)\n\t"
	"vmovdqu   %%ymm8, -(32 * 3)(%%rcx)\n\t"
	/* Store the first VEC.  */
	"vmovdqu   %%ymm4, (%%r11)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]\n\t"
	"large_backward:\n\t"
	/* Don't use non-temporal store if there is overlap between */
	/* destination and source since destination may be in cache */
	/* when source is loaded.  */
	"leaq    (%%rcx, %%rdx), %%r10\n\t"
	"cmpq    %%r10, %%r9\n\t"
	"jb      loop_4x_vec_backward\n\t"
	"loop_large_backward:\n\t"
	/* Copy 4 * VEC a time backward with non-temporal stores. */
	"prefetcht0 (-32 * 4 * 2)(%%rcx)\n\t"
	"prefetcht0 (-32 * 4 * 2 - 64)(%%rcx)\n\t"
	"prefetcht0 (-32 * 4 * 3)(%%rcx)\n\t"
	"prefetcht0 (-32 * 4 * 3 - 64)(%%rcx)\n\t"
	"vmovdqu   (%%rcx), %%ymm0\n\t"
	"vmovdqu   -32(%%rcx), %%ymm1\n\t"
	"vmovdqu   -(32 * 2)(%%rcx), %%ymm2\n\t"
	"vmovdqu   -(32 * 3)(%%rcx), %%ymm3\n\t"
	"subq    $(32*4), %%rcx\n\t"
	"subq    $(32*4), %%rdx\n\t"
	"vmovntdq  %%ymm0, (%%r9)\n\t"
	"vmovntdq  %%ymm1, -32(%%r9)\n\t"
	"vmovntdq  %%ymm2, -(32 * 2)(%%r9)\n\t"
	"vmovntdq  %%ymm3, -(32 * 3)(%%r9)\n\t"
	"subq    $(32 * 4), %%r9\n\t"
	"cmpq    $(32 * 4), %%rdx\n\t"
	"ja      loop_large_backward\n\t"
	"sfence\n\t"
	/* Store the first 4 * VEC.  */
	"vmovdqu   %%ymm4, (%%rdi)\n\t"
	"vmovdqu   %%ymm5, 32(%%rdi)\n\t"
	"vmovdqu   %%ymm6, (32 * 2)(%%rdi)\n\t"
	"vmovdqu   %%ymm7, (32 * 3)(%%rdi)\n\t"
	/* Store the last VEC.  */
	"vmovdqu   %%ymm8, (%%r11)\n\t"
	"vzeroupper\n\t"
	"jmp %l[done]"
	:
	: "r"(src), "r"(dest), "r"(size)
	: "rax", "rcx", "rdx", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "ymm0",
       	"ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "memory"
	: done
	);
done:
	return dest;

}
