	.file	"amd.c"
	.text
	.globl	mymemcpy
	.type	mymemcpy, @function
mymemcpy:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	.cfi_offset 3, -48
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-48(%rbp), %rbx
	movq	-40(%rbp), %r13
	movq	-56(%rbp), %r14
#APP
# 27 "amd.c" 1
	movq	%rbx, %rsi
	movq	%r13, %rdi
	movq	%r14, %rdx
	movq    %rdi, %rax
	cmp     $32, %rdx
	jb      less_vec
	cmp     $(32 * 2), %rdx
	ja      more_2x_vec
	vmovdqu   (%rsi), %ymm0
	vmovdqu   -32(%rsi,%rdx), %ymm1
	vmovdqu   %ymm0, (%rdi)
	vmovdqu   %ymm1, -32(%rdi,%rdx)
	vzeroupper
	jmp .L2
	less_vec:
	cmpb    $32, %dl
	jae     between_32_63
	cmpb    $16, %dl
	jae     between_16_31
	cmpb    $8, %dl
	jae     between_8_15
	cmpb    $4, %dl
	jae     between_4_7
	cmpb    $1, %dl
	ja      between_2_3
	jb      1f
	movzbl  (%rsi), %ecx
	movb    %cl, (%rdi)
	1:
	jmp .L2
	between_32_63:
	vmovdqu (%rsi), %ymm0
	vmovdqu -32(%rsi,%rdx), %ymm1
	vmovdqu %ymm0, (%rdi)
	vmovdqu %ymm1, -32(%rdi,%rdx)
	vzeroupper
	jmp .L2
	between_16_31:
	vmovdqu (%rsi), %xmm0
	vmovdqu -16(%rsi,%rdx), %xmm1
	vmovdqu %xmm0, (%rdi)
	vmovdqu %xmm1, -16(%rdi,%rdx)
	jmp .L2
	between_8_15:
	movq    -8(%rsi,%rdx), %rcx
	movq    (%rsi), %rsi
	movq    %rcx, -8(%rdi,%rdx)
	movq    %rsi, (%rdi)
	jmp .L2
	between_4_7:
	movl    -4(%rsi,%rdx), %ecx
	movl    (%rsi), %esi
	movl    %ecx, -4(%rdi,%rdx)
	movl    %esi, (%rdi)
	jmp .L2
	between_2_3:
	movzwl  -2(%rsi,%rdx), %ecx
	movzwl  (%rsi), %esi
	movw    %cx, -2(%rdi,%rdx)
	movw    %si, (%rdi)
	jmp .L2
	more_2x_vec:
	cmpq    $(32 * 8), %rdx
	ja      more_8x_vec
	cmpq    $(32 * 4), %rdx
	jb      last_4x_vec
	vmovdqu   (%rsi), %ymm0
	vmovdqu   32(%rsi), %ymm1
	vmovdqu   (32 * 2)(%rsi), %ymm2
	vmovdqu   (32 * 3)(%rsi), %ymm3
	vmovdqu   -32(%rsi,%rdx), %ymm4
	vmovdqu   -(32 * 2)(%rsi,%rdx), %ymm5
	vmovdqu   -(32 * 3)(%rsi,%rdx), %ymm6
	vmovdqu   -(32 * 4)(%rsi,%rdx), %ymm7
	vmovdqu   %ymm0, (%rdi)
	vmovdqu   %ymm1, 32(%rdi)
	vmovdqu   %ymm2, (32 * 2)(%rdi)
	vmovdqu   %ymm3, (32 * 3)(%rdi)
	vmovdqu   %ymm4, -32(%rdi,%rdx)
	vmovdqu   %ymm5, -(32 * 2)(%rdi,%rdx)
	vmovdqu   %ymm6, -(32 * 3)(%rdi,%rdx)
	vmovdqu   %ymm7, -(32 * 4)(%rdi,%rdx)
	vzeroupper
	jmp .L2
	last_4x_vec:
	vmovdqu   (%rsi), %ymm0
	vmovdqu   32(%rsi), %ymm1
	vmovdqu   -32(%rsi,%rdx), %ymm2
	vmovdqu   -(32 * 2)(%rsi,%rdx), %ymm3
	vmovdqu   %ymm0, (%rdi)
	vmovdqu   %ymm1, 32(%rdi)
	vmovdqu   %ymm2, -32(%rdi,%rdx)
	vmovdqu   %ymm3, -(32 * 2)(%rdi,%rdx)
	vzeroupper
	nop:
	jmp .L2
	more_8x_vec:
	cmpq    %rsi, %rdi
	ja      more_8x_vec_backward
	je      nop
	vmovdqu   (%rsi), %ymm4
	vmovdqu   -32(%rsi, %rdx), %ymm5
	vmovdqu   -(32 * 2)(%rsi, %rdx), %ymm6
	vmovdqu   -(32 * 3)(%rsi, %rdx), %ymm7
	vmovdqu   -(32 * 4)(%rsi, %rdx), %ymm8
	movq    %rdi, %r11
	leaq    -32(%rdi, %rdx), %rcx
	movq    %rdi, %r8
	andq    $(32 - 1), %r8
	subq    $32, %r8
	subq    %r8, %rsi
	subq    %r8, %rdi
	addq    %r8, %rdx
	cmpq	 $(1024*1024), %rdx
	ja      large_forward
	loop_4x_vec_forward:
	vmovdqu   (%rsi), %ymm0
	vmovdqu   32(%rsi), %ymm1
	vmovdqu   (32 * 2)(%rsi), %ymm2
	vmovdqu   (32 * 3)(%rsi), %ymm3
	addq    $(32 * 4), %rsi
	subq    $(32 * 4), %rdx
	vmovdqa   %ymm0, (%rdi)
	vmovdqa   %ymm1, 32(%rdi)
	vmovdqa   %ymm2, (32 * 2)(%rdi)
	vmovdqa   %ymm3, (32 * 3)(%rdi)
	addq    $(32 * 4), %rdi
	cmpq    $(32 * 4), %rdx
	ja      loop_4x_vec_forward
	vmovdqu   %ymm5, (%rcx)
	vmovdqu   %ymm6, -32(%rcx)
	vmovdqu   %ymm7, -(32 * 2)(%rcx)
	vmovdqu   %ymm8, -(32 * 3)(%rcx)
	vmovdqu   %ymm4, (%r11)
	vzeroupper
	jmp .L2
	more_8x_vec_backward:
	vmovdqu   (%rsi), %ymm4
	vmovdqu   32(%rsi), %ymm5
	vmovdqu   (32 * 2)(%rsi), %ymm6
	vmovdqu   (32 * 3)(%rsi), %ymm7
	vmovdqu   -32(%rsi,%rdx), %ymm8
	leaq    -32(%rdi, %rdx), %r11
	leaq    -32(%rsi, %rdx), %rcx
	movq    %r11, %r9
	movq    %r11, %r8
	andq    $(32 - 1), %r8
	subq    %r8, %rcx
	subq    %r8, %r9
	subq    %r8, %rdx
	cmpq	 $(1024*1024), %rdx
	ja      large_backward
	loop_4x_vec_backward:
	vmovdqu   (%rcx), %ymm0
	vmovdqu   -32(%rcx), %ymm1
	vmovdqu   -(32 * 2)(%rcx), %ymm2
	vmovdqu   -(32 * 3)(%rcx), %ymm3
	subq    $(32 * 4), %rcx
	subq    $(32 * 4), %rdx
	vmovdqa   %ymm0, (%r9)
	vmovdqa   %ymm1, -32(%r9)
	vmovdqa   %ymm2, -(32 * 2)(%r9)
	vmovdqa   %ymm3, -(32 * 3)(%r9)
	subq    $(32 * 4), %r9
	cmpq    $(32 * 4), %rdx
	ja      loop_4x_vec_backward
	vmovdqu   %ymm4, (%rdi)
	vmovdqu   %ymm5, 32(%rdi)
	vmovdqu   %ymm6, (32 * 2)(%rdi)
	vmovdqu   %ymm7, (32 * 3)(%rdi)
	vmovdqu   %ymm8, (%r11)
	vzeroupper
	jmp .L2
	large_forward:
	leaq    (%rdi, %rdx), %r10
	cmpq    %r10, %rsi
	jb      loop_4x_vec_forward
	loop_large_forward:
	prefetcht0 (32*4*2)(%rsi)
	prefetcht0 (32*4*2 + 64)(%rsi)
	prefetcht0 (32*4*3)(%rsi)
	prefetcht0 (32*4*3 + 64)(%rsi)
	vmovdqu   (%rsi), %ymm0
	vmovdqu   32(%rsi), %ymm1
	vmovdqu   (32 * 2)(%rsi), %ymm2
	vmovdqu   (32 * 3)(%rsi), %ymm3
	addq    $(32*4), %rsi
	subq    $(32*4), %rdx
	vmovntdq  %ymm0, (%rdi)
	vmovntdq  %ymm1, 32(%rdi)
	vmovntdq  %ymm2, (32 * 2)(%rdi)
	vmovntdq  %ymm3, (32 * 3)(%rdi)
	addq    $(32*4), %rdi
	cmpq    $(32*4), %rdx
	ja      loop_large_forward
	sfence
	vmovdqu   %ymm5, (%rcx)
	vmovdqu   %ymm6, -32(%rcx)
	vmovdqu   %ymm7, -(32 * 2)(%rcx)
	vmovdqu   %ymm8, -(32 * 3)(%rcx)
	vmovdqu   %ymm4, (%r11)
	vzeroupper
	jmp .L2
	large_backward:
	leaq    (%rcx, %rdx), %r10
	cmpq    %r10, %r9
	jb      loop_4x_vec_backward
	loop_large_backward:
	prefetcht0 (-32 * 4 * 2)(%rcx)
	prefetcht0 (-32 * 4 * 2 - 64)(%rcx)
	prefetcht0 (-32 * 4 * 3)(%rcx)
	prefetcht0 (-32 * 4 * 3 - 64)(%rcx)
	vmovdqu   (%rcx), %ymm0
	vmovdqu   -32(%rcx), %ymm1
	vmovdqu   -(32 * 2)(%rcx), %ymm2
	vmovdqu   -(32 * 3)(%rcx), %ymm3
	subq    $(32*4), %rcx
	subq    $(32*4), %rdx
	vmovntdq  %ymm0, (%r9)
	vmovntdq  %ymm1, -32(%r9)
	vmovntdq  %ymm2, -(32 * 2)(%r9)
	vmovntdq  %ymm3, -(32 * 3)(%r9)
	subq    $(32 * 4), %r9
	cmpq    $(32 * 4), %rdx
	ja      loop_large_backward
	sfence
	vmovdqu   %ymm4, (%rdi)
	vmovdqu   %ymm5, 32(%rdi)
	vmovdqu   %ymm6, (32 * 2)(%rdi)
	vmovdqu   %ymm7, (32 * 3)(%rdi)
	vmovdqu   %ymm8, (%r11)
	vzeroupper
	jmp .L2
#NO_APP
.L2:
.L3:
	movq	-40(%rbp), %rax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	mymemcpy, .-mymemcpy
	.ident	"GCC: (GNU) 10.3.0"
	.section	.note.GNU-stack,"",@progbits
