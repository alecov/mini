	bits 64

yellow	equ 0xFFFF
sierp	equ 256
video_w	equ 800
video_h	equ 600
col	equ video_w - 1
row	equ (sierp - 1)*video_w - (sierp + 1)
video_o	equ video_w*(video_h - sierp)/2 + video_w/2

	add edi, 4*video_o
plot:	test dl, al
	jnz skip
	mov dword [rdi], yellow
skip:	add edi, 4*col
	inc al
	jnz plot

	sub edi, 4*row
	cbw
	inc dl
	jnz plot
