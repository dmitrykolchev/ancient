		.386p
TRACE_TEXT      SEGMENT BYTE PUBLIC USE32 'TRACECODE'
TRACE_TEXT      ENDS

DGROUP          GROUP   _DATA,_BSS

                ASSUME  cs:TRACE_TEXT, ds:DGROUP

TRACE_TEXT      SEGMENT BYTE PUBLIC USE32 'TRACECODE'
		ASSUME	cs:TRACE_TEXT
		PUBLIC  _trace

			
;		flags   [ebp+0Ch]
;		cs	[ebp+8]	
;		eip     [ebp+4]

_trace          proc    far
		push	ebp
		mov	ebp,esp
		push	eax
		push	ds
		push	ecx
		push	edx
		push	ebx
		push	esi
		push	edi
		push	es
		push	fs
		push	gs
		sti
		mov	ax,4 SHL 3 ; DATA_SEGMENT
		mov	ds,ax
		mov	eax,[ebp+0Ch]
		mov	DGROUP:oldEFLAGS,eax
		mov	eax,[ebp+8]
		mov	DGROUP:oldCS,eax
		test	al,11b
		jz	@@NoSS_ESP

		mov	eax,[ebp+10h]
		mov	DGROUP:oldESP,eax
		mov	eax,[ebp+14h]
		mov	DGROUP:oldSS,eax
		jmp	@@2
@@NoSS_ESP:
		mov	DGROUP:oldSS,ss
		mov	DGROUP:oldESP,ebp
		add	dword ptr DGROUP:oldESP,16
@@2:
                mov	eax,[ebp+4]
		mov	DGROUP:oldEIP,eax
		mov	eax,[ebp]
		mov	DGROUP:regEBP,eax
		mov	eax,[ebp-4]
		mov	DGROUP:regEAX,eax
		mov	ax,word ptr [ebp-8]
		mov	word ptr DGROUP:regDS,ax
		mov	DGROUP:regECX,ecx
		mov	DGROUP:regEDX,edx
		mov	DGROUP:regEBX,ebx
		mov	DGROUP:regESI,esi
		mov	DGROUP:regEDI,edi
		mov	word ptr DGROUP:regES,es
		mov	word ptr DGROUP:regFS,fs
		mov	word ptr DGROUP:regGS,gs

		sgdt	fword ptr DGROUP:regGDTR
                sidt    fword ptr DGROUP:regIDTR
                sldt    word ptr DGROUP:regLDTR
                str     word ptr DGROUP:regTR

		
;               call    far ptr _interface

		DB	66h
                DB      9Ah
                DW      offset _interface
                DW      3 SHL 3

		pop	gs
		pop	fs
		pop	es
		pop	edi
		pop	esi
		pop	ebx
		pop	edx
		pop	ecx
		pop	ds
		pop	eax
		mov	esp,ebp
		pop	ebp		
                iretd
_trace          endp



TRACE_TEXT      ENDS

_DATA           SEGMENT word public USE16 'DATA'

_DATA           ENDS

_BSS            SEGMENT word public USE16 'BSS'

n               dd      ?

_R              label   dword
                PUBLIC  _R

regGS           dd      ?
regFS           dd      ?
regES           dd      ?
regDS           dd      ?

regEDI          dd      ?
regESI          dd      ?
regEBP          dd      ?
regESP          dd      ?
regEBX          dd      ?
regEDX          dd      ?
regECX          dd      ?
regEAX          dd      ?

oldEIP          dd      ?
oldCS           dd      ?
oldEFLAGS       dd      ?
oldESP          dd      ?
oldSS           dd      ?

regGDTR         df      ?
regIDTR         df      ?
regLDTR         dw      ?
regTR           dw      ?
regCR0          dd      ?
regCR2          dd      ?
regCR3          dd      ?
regDR0          dd      ?
regDR1          dd      ?
regDR2          dd      ?
regDR3          dd      ?
regDR6          dd      ?
regDR7          dd      ?
regTR6          dd      ?
regTR7          dd      ?

_BSS            ENDS

                EXTRN   _interface:far

                END