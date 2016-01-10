.686
.model flat
.xmm

public _szybki_max

.code

_szybki_max proc

push ebp
mov ebp, esp

mov eax, 0
mov edx, 0

mov ebx, [esp + 8] ; tab1
mov esi, [esp + 12] ; tab2
mov edi, [esp + 16] ; tabw
mov eax, [esp + 20] ; n

mov cx, 4
div cx
mov ecx, eax

loop0:

	vmovupd ymm1, [ebx]

	vmovupd ymm2, [esi]

	vmaxps ymm0, ymm1, ymm2

	vmovupd [edi], ymm0

	add ebx, 32
	add esi, 32
	add edi, 32

loop loop0

pop ebp
ret

_szybki_max ENDP

END