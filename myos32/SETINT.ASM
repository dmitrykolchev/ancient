
_TEXT		SEGMENT	byte public 'CODE'
		ASSUME	cs:_TEXT

_SetInterrupts	proc	near
		push	bp
		mov	bp,sp
		mov	bl,[bp+6]
		mov	bh,[bp+4]
        	mov     al,00                          
        	out     0F1h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,11h                          
        	out     0A0h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	out     20h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,bl                          
        	out     0A1h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,bh                          
        	out     21h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,02h                          
        	out     0A1h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,04h                          
        	out     21h,al                          
        	jmp     $+2                            
        	jmp     $+2                            
        	mov     al,01h                          
        	out     0A1h,al                   
	        jmp     $+2                            
        	jmp     $+2                           
        	out     21h,al                          
        	jmp     $+2                            
        	jmp     $+2                        
        	mov     al,0FFh                          
        	out     0A1h,al                          
        	jmp     $+2                        
        	jmp     $+2                        
        	out     21h,al
		pop	bp                          
        	ret
_SetInterrupts	endp
          
		.386
_KeyboardInt	proc	near
		pushad
		push	ds fs gs es 

		mov	ax,4 SHL 3
		mov	ds,ax

		call	near ptr _KeyboardHandler

		pop	es gs fs ds		
		popad
		iret
_KeyboardInt	endp

_TimerInt	proc	near
		pushad
		push	ds fs gs es
		mov	ax,4 SHL 3
		mov	ds,ax
		call	near ptr _TimerHandler
		pop	es gs fs ds
		popad
		iret
_TimerInt	endp

_TEXT		ENDS

		PUBLIC	_SetInterrupts
		EXTRN	_KeyboardHandler:near
		PUBLIC	_KeyboardInt
		EXTRN	_TimerHandler:near
		PUBLIC	_TimerInt

		END           