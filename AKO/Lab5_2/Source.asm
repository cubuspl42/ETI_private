.686
.model flat

public _nowy_cosh

.data

null dd 0
two dd +2.0

.code

_nowy_cosh PROC

push ebp
mov ebp, esp

mov eax, 2

fld1 ; sigma
fld dword ptr [ebp + 8]
fld st(0)
fmulp ; x^2
fld1 ; !
fld1 ; x^2n

; x^2n (1)
; (2n)! (1)
; x^2 (x^2)
; sigma (1)

mov ecx, 9
mov edx, 1
loop0:

	fmul st(0), st(2)

	; x^2n (x^2)
	; (2n)! (1)
	; x^2 (x^2) const
	; sigma (1)

	fxch

	; (2n)! (1)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1)

	push edx
	fild dword ptr [esp]
	pop edx
	inc edx

	push edx
	fild dword ptr [esp]
	pop edx
	inc edx

	; 2
	; 1
	; (2n)! (1)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1)

	fmulp
	fmulp

	; (2n)! (1*1*2)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1)

	fld st(1)

	; x^2n (x^2)
	; (2n)! (1*1*2)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1)

	fdiv st(0), st(1)

	; x^2n/(2n)! (x^2/(1*1*2))
	; (2n)! (1*1*2)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1)

	faddp st(4), st(0)

	; (2n)! (1*1*2)
	; x^2n (x^2)
	; x^2 (x^2)
	; sigma (1 + (x^2/(1*1*2)))

	fxch

	; x^2n (x^2)
	; (2n)! (1*1*2)
	; x^2 (x^2)
	; sigma (1 + (x^2/(1*1*2)))

loop loop0

fstp st(0)
fstp st(0)
fstp st(0)

pop ebp
ret
_nowy_cosh ENDP

END
