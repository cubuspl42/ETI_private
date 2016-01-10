.686
.model flat

public _srednia_arytmetyczna

.data

.code

_srednia_arytmetyczna PROC
push ebp
mov ebp, esp

mov esi, [ebp + 8]
mov ecx, [ebp + 12]

fldz

loop0:
fld dword ptr [esi]
faddp st(1), st(0)
add esi, 4
loop loop0

fild dword ptr [ebp + 12]
fdivp st(1), st(0)

pop ebp
ret
_srednia_arytmetyczna ENDP

END
