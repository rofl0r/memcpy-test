.global mymemcpy
.global __mymemcpy_fwd
.hidden __mymemcpy_fwd
.type mymemcpy,@function
mymemcpy:
__mymemcpy_fwd:
	mov %rdi,%rax
	cmp $8,%rdx
	jc 1f
	test $7,%edi
	jz 1f
2:	movsb
	dec %rdx
	test $7,%edi
	jnz 2b
1:	mov %rdx,%rcx
	shr $3,%rcx
	rep
	movsq
	and $7,%edx
	jz 1f
2:	movsb
	dec %edx
	jnz 2b
1:	ret
