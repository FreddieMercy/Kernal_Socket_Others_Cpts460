        OSSEG  = 0x1000
	
       .globl _main,_running,_scheduler,_proc,_procSize, _disp
       .globl _tswitch
	
        jmpi   start,OSSEG

start:	mov  ax,cs
		mov  ds,ax
		mov  ss,ax

        mov  sp,#_proc
		add  sp,_procSize

		#call _disp

		call _main


idle:	jmp  idle
	
_tswitch:
SAVE:	
	push ax
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf

	push ds
    push ss

	mov  bx,_running
	mov  2[bx],sp

FIND:	call _scheduler

RESUME:

	mov bx,_running
	mov sp,2[bx]

    pop ss
    pop ds

	popf
	pop di
	pop si
	pop bp
	pop dx
	pop cx
	pop bx
	pop ax

	ret
