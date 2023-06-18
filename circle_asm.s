        extern sqrt_c
        extern debug

	section .text
        global  circle_asm
	global	draw_pixel

;-----------------------------------------------------------------------------
; Draw circle
; Arguments:
;   rdi - buffer
;   rsi - width
;   rdx - heigth
;   rcx - xc
;   r8  - yc
;   r9  - radius
;   [rsp+16] - color
;
; C equivalent:
;        int rad2 = radius * radius;
;        for(int x=0;x<radius;x++) {
;                int x2 = x * x;
;                int y = sqrt(rad2-x2);
;                draw_asm(image,width,height,xc+x,yc+y,color);
;                draw_asm(image,width,height,xc-x,yc+y,color);
;                draw_asm(image,width,height,xc-x,yc-y,color);
;                draw_asm(image,width,height,xc+x,yc-y,color);
;
;        }

circle_asm:
; save registers
	push 	rbp
	mov	rbp,rsp
	push	rbx
	push	r12			; we use r12 to keep current 'x'
	push	r13			
	push	r14
	push	r15

	mov	r14, rcx
	mov	r15, r8


	mov	rax, r9			; rax = radius	
	mul	rax			; rax = radius*radius
	mov	rbx, rax		; rbx = radius*radius
	mov	r12, r9			; r12 = x = radius
l:
	mov	rax, r12
	call	print_rax		; print x

	mul	rax			; rax = x * x
	call	print_rax		; print x^2
	mov	r13, rax
	mov	rax, rbx
	call	print_rax		; print radius^2
	sub	rax, r13		; rax = radius*radius - x*x
	call	print_rax		; print radius*radius - x*x

; call external sqrt
	push	rdi
	mov	rdi, rax
	call	sqrt
	pop	rdi			
; here rax = sqrt(radius*radius - x*x)
	call	print_rax
	mov	r11, rax		; save y to r11


; get color
	mov	r9,[rsp+16] 		; 7th argument
; calculate final x and y for first point
	add	rax, r15		; rax = yc + y
	mov	r8,rax			; r8 = y
	mov	rcx, r14
	add	rcx, r12		; rcx = xc + x
	call	draw_pixel

; calculate x and y for 2nd point
	mov	r8, r15
	sub	r8, r11			; rax = yc - y
	call	draw_pixel

; 3rd pixel
	mov	rcx, r14
	sub	rcx, r12		; rcx = xc - x
	call	draw_pixel

; 4th
	mov	r8, r15
	add	r8, r11			; r8 = y
	call	draw_pixel


; print end of cycle
	mov	rax,9999
	call	print_rax

; increment x
	sub	r12,1
	jnz	l

; restore registers and return
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	rbx	
	pop	rbp
	ret

;-----------------------------------------------------------------------------
; Draw pixel in memory buffer. 
; The function intends that buffer format is 2 bytes per pixel.
; Arguments:
;   rdi - buffer
;   rsi - width
;   rdx - heigth
;   rcx - x
;   r8  - y
;   r9  - color
; The function does the following:
; 1) Calculate requested point offset: offset = image + width * 2 * y + x * 2;
; 2) Save 'color' value as 2 byte integer on calculated addres 'offset'
; Return value: void
;
draw_pixel:
	push	rdi
	push	r10
	push	rax
	mov	rax, rsi	; width
	mov	r10, 2		; 2
	mul	r10		; rax = width * 2
	mul	r8		; rax = width * 2 * y
	add	rdi, rax	; rdi = image + width * 2 * y
	mov	rax, rcx	; rax = x
	mul	r10		; rax = x * 2
	add	rdi, rax	; rdi = image + width * 2 * y + x * 2
	mov	rax, r9
	mov	[rdi],ax
	pop	rax
	pop	r10
	pop	rdi
	ret

;-----------------------------------------------------------------------------
; Debug outpu rax register value using printf C function
print_rax:
	push	rdi
	push	rsi
	push	rdx
	push	rcx
	push	r8
	push	r9
	push	rax

	mov	rdi, rax
	call	debug

	pop	rax
	pop	r9
	pop	r8
	pop	rcx
	pop	rdx
	pop	rsi
	pop	rdi
	ret

;-----------------------------------------------------------------------------
; external sqrt function
sqrt:
	push	rdi
	push	rsi
	push	rdx
	push	rcx
	push	r8
	push	r9

	call	sqrt_c

	pop	r9
	pop	r8
	pop	rcx
	pop	rdx
	pop	rsi
	pop	rdi
	ret
