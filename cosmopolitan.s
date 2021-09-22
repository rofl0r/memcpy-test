# Copyright 2020 Justine Alexandra Roberts Tunney
#
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all copies.

# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
# WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
# AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
# DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
# PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
# TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

#	Copies memory.
#
#	DEST and SRC must not overlap, unless DEST<=SRC.
#
#	@param	rdi is dest
#	@param	rsi is src
#	@param	rdx is number of bytes
#	@return	original rdi copied to rax
#	@mode	long
#	@asyncsignalsafe
mymemcpy:
	mov	%rdi,%rax
.align	16
.size mymemcpy,.-mymemcpy
.type mymemcpy,@function
.globl mymemcpy

#	Copies memory w/ minimal impact ABI.
#
#	@param	rdi is dest
#	@param	rsi is src
#	@param	rdx is number of bytes
#	@clob	flags,rcx,xmm3,xmm4
#	@mode	long
__memcpy:
	push %rbp
	mov %rsp,%rbp
	mov	$.L__memcpytab.ro.size,%ecx
	cmp	%rcx,%rdx
	cmovb	%rdx,%rcx
## ifndef __PIC__
	jmp	*__memcpytab(,%rcx,8)
## else
#	shl $3, %rcx  # mul by 8
#	add __memcpytab@GOTPCREL(%rip), %rcx
#	jmp *%rcx
##	jmp	*__memcpytab@GOTPCREL(%rip)(,%rcx,8)
## endif
.Lanchorpoint:
.L32r:	cmp	$1024,%rdx
	jae	.Lerms
.L32:	vmovdqu	-32(%rsi,%rdx),%ymm4
	mov	$32,%rcx
0:	add	$32,%rcx
	vmovdqu	-64(%rsi,%rcx),%ymm3
	vmovdqu	%ymm3,-64(%rdi,%rcx)
	cmp	%rcx,%rdx
	ja	0b
	vmovdqu	%ymm4,-32(%rdi,%rdx)
	vxorps	%ymm4,%ymm4,%ymm4
	vxorps	%ymm3,%ymm3,%ymm3
	jmp	.L0
.L16r:	cmp	$1024,%rdx
	jae	.Lerms
.L16:	movdqu	-16(%rsi,%rdx),%xmm4
	mov	$16,%rcx
0:	add	$16,%rcx
	movdqu	-32(%rsi,%rcx),%xmm3
	movdqu	%xmm3,-32(%rdi,%rcx)
	cmp	%rcx,%rdx
	ja	0b
	movdqu	%xmm4,-16(%rdi,%rdx)
	pxor	%xmm4,%xmm4
	pxor	%xmm3,%xmm3
	jmp	.L0
.L8:	push	%rbx
	mov	(%rsi),%rcx
	mov	-8(%rsi,%rdx),%rbx
	mov	%rcx,(%rdi)
	mov	%rbx,-8(%rdi,%rdx)
1:	pop	%rbx
.L0:	pop %rbp
	ret
.L4:	push	%rbx
	mov	(%rsi),%ecx
	mov	-4(%rsi,%rdx),%ebx
	mov	%ecx,(%rdi)
	mov	%ebx,-4(%rdi,%rdx)
	jmp	1b
.L3:	push	%rbx
	mov	(%rsi),%cx
	mov	-2(%rsi,%rdx),%bx
	mov	%cx,(%rdi)
	mov	%bx,-2(%rdi,%rdx)
	jmp	1b
.L2:	mov	(%rsi),%cx
	mov	%cx,(%rdi)
	jmp	.L0
.L1:	mov	(%rsi),%cl
	mov	%cl,(%rdi)
	jmp	.L0
.Lerms:
#### if 1 || defined(TINY)
	cmp	$1024*1024,%rdx
#### else
#	cmp	kHalfCache3(%rip),%rdx
#### endif
	ja	.Lnts
	push	%rdi
	push	%rsi
	mov	%rdx,%rcx
	rep movsb
	pop	%rsi
	pop	%rdi
	jmp	.L0
.Lnts:	movdqu	(%rsi),%xmm3
	movdqu	%xmm3,(%rdi)
	lea	16(%rdi),%rcx
	and	$-16,%rcx
	sub	%rdi,%rcx
	add	%rcx,%rdi
	add	%rcx,%rsi
	sub	%rcx,%rdx
	mov	$16,%rcx
0:	add	$16,%rcx
	movdqu	-32(%rsi,%rcx),%xmm3
	movntdq	%xmm3,-32(%rdi,%rcx)
	cmp	%rcx,%rdx
	ja	0b
	sfence
	movdqu	-16(%rsi,%rdx),%xmm3
	movdqu	%xmm3,-16(%rdi,%rdx)
	pxor	%xmm3,%xmm3
	jmp	.L0
.size __memcpy,.-__memcpy
.type __memcpy,@function
.globl __memcpy
.hidden __memcpy

.section .initro.300._init_memcpy,"a",@progbits
__memcpytab.ro:
	.byte	.L0-.Lanchorpoint
	.byte	.L1-.Lanchorpoint
	.byte	.L2-.Lanchorpoint
	.byte	.L3-.Lanchorpoint
	.rept	4
	.byte	.L4-.Lanchorpoint
	.endr
	.rept	8
	.byte	.L8-.Lanchorpoint
	.endr
	.rept	16
	.byte	.L16-.Lanchorpoint
	.endr
	.equ	.L__memcpytab.ro.size,.-__memcpytab.ro
	.size __memcpytab.ro,.-__memcpytab.ro
	.type __memcpytab.ro,@object
	.if	.L__memcpytab.ro.size % 8
	.error	"memcpytab alignment wrong"
	.endif
	.byte	.L16-.Lanchorpoint		# SSE2
	.byte	.L16r-.Lanchorpoint		# SSE2 + ERMS
	.byte	.L32-.Lanchorpoint		# AVX
	.byte	.L32r-.Lanchorpoint		# AVX + ERMS
	.byte	0,0,0,0
	.previous

.section .piro.bss.init.2.300._init_memcpy,"aw",@nobits
.align 8
__memcpytab:
	.rept	.L__memcpytab.ro.size
	.quad	0
	.endr
	.quad	0
	.size __memcpytab,.-__memcpytab
	.type __memcpytab,@object
	.previous


.section .init.300._init_memcpy,"ax",@progbits
.align 8
_init_memcpy:
	# i had to add these 2 lines to match ABI of __memjmpinit
	# apparently cosmopolitan does some other trickery to pass these
# these 2 work for static link
## ifndef __PIC__
	mov $__memcpytab, %rdi
	mov $__memcpytab.ro, %rsi
## else
#	mov __memcpytab@GOTPCREL(%rip), %rdi
#	mov __memcpytab.ro@GOTPCREL(%rip), %rsi
## endif
	# -----------------------
	#  pushpop .Lmemcpytab.ro.size,%rcx
	push	$.L__memcpytab.ro.size
	pop %rcx
## ifndef __PIC
#	mov .Lanchorpoint@GOTPCREL(%rip),%edx
## else
	mov $.Lanchorpoint,%edx
## endif
	testb $1 << (28 % 8), 28 / 8 + 1 * 16 + 2 * 4 +__kcpuids(%rip)
#testb	X86_HAVE(AVX)+__kcpuids(%rip)
## ifndef __PIC__
	call	__memjmpinit
## else
#	call	*__memjmpinit@GOTPCREL(%rip)
## endif
	#.init.end 300,_init_memcpy
	.size _init_memcpy,.-_init_memcpy
	.type _init_memcpy,@function
	.globl _init_memcpy
	.previous

# cosmopolitan libc/nexgen32e/memjmpinit.S

.section .text.startup,"ax",@progbits

#	Initializes jump table for memset() and memcpy().

#	@param	!ZF if required cpu vector extensions are available
#	@param	rdi is address of 64-bit jump table
#	@param	rsi is address of 8-bit jump initializers
#	@param	rdx is address of indirect branch
#	@param	ecx is size of jump table
__memjmpinit:
        push %rbp
        mov %rsp,%rbp
	setnz	%r8b
	shl	%r8b
0:	xor	%eax,%eax
	lodsb
	add	%rdx,%rax
	stosq
#	.loop	0b
	.byte 0x83,0xe9,0x01 # sub $1,%ecx
	jnz 0b
	xor	%eax,%eax
	testb $1 << (9 % 8), 9 / 8 + 3 * 16 + 1 * 4 +__kcpuids(%rip)
#	testb	X86_HAVE(ERMS)+__kcpuids(%rip)
	setnz	%al
	or	%r8b,%al
	mov	(%rsi,%rax),%al
	add	%rdx,%rax
	stosq
	lodsq
	pop %rbp
        ret
.size __memjmpinit,.-__memjmpinit
.type __memjmpinit,@function
.global __memjmpinit
.hidden __memjmpinit
#	.endfn	__memjmpinit,globl,hidden
#	.source	__FILE__


# cosmopolitan: libc/nexgen32e/kcpuids.S

.section .piro.bss.init.2.201._init_kcpuids,"aw",@nobits
.align 8
__kcpuids:
 .long 0,0,0,0 # EAX=0 (Basic Processor Info)
 .long 0,0,0,0 # EAX=1 (Processor Info)
 .long 0,0,0,0 # EAX=2
 .long 0,0,0,0 # EAX=7 (Extended Features)
 .long 0,0,0,0 # EAX=0x80000001 (NexGen32e)
 .long 0,0,0,0 # EAX=0x80000007 (APM)
 .long 0,0,0,0 # EAX=16h (CPU Frequency)
.size __kcpuids,.-__kcpuids
.type __kcpuids,@object
.global __kcpuids
.previous

.section .init.201._init_kcpuids,"ax",@progbits
_init_kcpuids:
 push %rbx
 push $0
 push $0x16
 push $0xffffffff80000007
 push $0xffffffff80000001
 push $7
 push $2
 push $1
 mov %rdi,%r8
# rofl0r: added this line
## ifndef __PIC__
 mov $__kcpuids, %rdi
## else
# mov __kcpuids@GOTPCREL(%rip), %rdi
## endif

# -----------------------
 xor %eax,%eax
1: xor %ecx,%ecx
 cpuid
  stos   %eax,(%rdi)
# stosl
 xchg %eax,%ebx
 stosl
 xchg %eax,%ecx
 stosl
 xchg %eax,%edx
 stosl
2: pop %rax
 test %eax,%eax # EAX = stacklist->pop()
 jz 3f # EAX != 0 (EOL sentinel)
 cmp 0 * 16 + 0 * 4(%r8),%al # EAX <= CPUID.0 max leaf
 jbe 1b # CPUID too new to probe
 add $4*4,%rdi
 jmp 2b
3: nop
 testb $1 << (28 % 8), 28 / 8 + 1 * 16 + 2 * 4(%r8)
 jz 5f
 testb $1 << (27 % 8), 27 / 8 + 1 * 16 + 2 * 4(%r8)
 jz 4f
 xor %ecx,%ecx
 xgetbv
 and $0x02|0x04,%eax
 cmp $0x02|0x04,%eax
 je 5f
4: btr $28,1 * 16 + 2 * 4(%r8)
 btr $5,3 * 16 + 1 * 4(%r8)
5: pop %rbx
 .size _init_kcpuids,.-_init_kcpuids
 .type _init_kcpuids,@function
 .globl _init_kcpuids

.section .init_array,"aw"
.align 8
#.quad _init_kcpuids
.quad _init_memcpy
