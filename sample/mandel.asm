	bits 64
	org 0x00400000

start:
	lea ebp, [rdi + 1]
	lea ebx, [rsi + (data - start)]
	add ecx, 0x11

	; Main loop.
mloop:
	mov edi, ebp
	xor esi, esi ; x variable;
	cdq          ; y variable.

	; Plotting loop.
ploop:
	mov eax, ecx ; i variable (counted backwards).
	fild word [rbx + (p4_0 - data)]

	; Calculate xscale.
	mov [rsp], edx
	fild dword [rsp]
	fmul dword [rbx + (scale - data)]
	fadd dword [rbx + (m2_125 - data)]

	; Calculate yscale.
	mov [rsp], esi
	fild dword [rsp]
	fmul dword [rbx + (scale - data)]
	fld1
	fsubrp st1, st0

	fld st1 ; zx
	fld st1 ; zy

	; Escape time loop.
eloop:
	; Calculate zx² and zy².
	fld st1
	fmul st0, st0
	fld st1
	fmul st0, st0

	; zx² + zy² > 4.0?
	fld st1
	fadd st0, st1
	fucomip st0, st7
	ja oplot

	; ++i >= max?
	dec eax
	jz plot

	; Calculate zxnew.
	fsubp st1, st0
	fadd st0, st4

	; Calculate zynew.
	fld st1
	fadd st0, st0
	fmul st0, st3
	fadd st0, st4

	; Update zx and zy with zxnew and zynew.
	fxch st2
	fstp st0
	fxch st2
	fstp st0
	jmp eloop

	; Plot an outside point.
oplot:
	neg eax
	add eax, ecx
	mov ah, al

plot:
	stosd
	fninit

	inc edx
	cmp dx, 800
	jb ploop

	cdq
	inc esi
	cmp si, 600
	jb ploop

	add ecx, 0x10
	test cl, cl
	jnz mloop

data:
scale	dd 0.0033333333333333333333 ; 2/600
m2_125	dd -2.125
p4_0	dw +4
