.global mymemcpy
.type mymemcpy,@function
mymemcpy:
	push %esi
	push %edi
	mov 12(%esp),%edi
	mov 16(%esp),%esi
	mov 20(%esp),%ecx
	mov %edi,%eax
	mov %ecx,%edx
	shr $2,%ecx
	rep
	movsl
	and $3,%edx
	jz 1f
2:	mov (%esi),%cl
	mov %cl,(%edi)
	inc %esi
	inc %edi
	dec %edx
	jnz 2b
1:	pop %edi
	pop %esi
	ret
