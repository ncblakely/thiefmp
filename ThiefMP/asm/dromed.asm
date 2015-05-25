.386
.model flat, c

.data

INCLUDE asmdefs.inc

.code
EXTERN PushConsoleMessage: PROC

__GhostPrintGhostPos proc
var_49C		= qword	ptr -49Ch
var_494		= qword	ptr -494h
var_48C		= qword	ptr -48Ch
var_470		= qword	ptr -470h
var_468		= qword	ptr -468h
var_460		= qword	ptr -460h
var_440		= byte ptr -440h
var_400		= byte ptr -400h
arg_0		= dword	ptr  8
Data		= dword	ptr  0Ch
arg_8		= dword	ptr  10h
V		= dword	ptr  14h
ArriveOrder	= dword	ptr  18h

		push	ebp
		mov	ebp, esp
		and	esp, 0FFFFFFF8h
		sub	esp, 440h
		mov	eax, [ebp+ArriveOrder]
		push	esi
		cmp	eax, 0FFFFFFFFh
		push	edi
		jz	short loc_58896B
		push	eax
		lea	eax, [esp+44Ch+var_440]
		push	offset aSeqD	; " seq	%d"
		push	eax		; char *
		call	_sprintf
		add	esp, 0Ch
		jmp	short loc_588970
; 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

loc_58896B:				; CODE XREF: __GhostPrintGhostPos+14j
		mov	[esp+448h+var_440], 0

loc_588970:				; CODE XREF: __GhostPrintGhostPos+29j
		mov	esi, [ebp+Data]
		test	byte ptr [esi+1Eh], 2
		mov	eax, offset aFull ; "Full"
		jnz	short loc_588983
		mov	eax, offset aDtz ; "Dtz"

loc_588983:				; CODE XREF: __GhostPrintGhostPos+3Cj
		movsx	edx, word ptr [esi+1Ah]
		fld	dword ptr [esi+8]
		xor	ecx, ecx
		mov	edi, [ebp+V]
		mov	cx, [esi+18h]
		push	ecx		; %x
		xor	ecx, ecx
		mov	cx, [esi+1Ch]
		push	edx		; %x
		push	ecx		; %x
		push	eax		; a(%s)	("Full"	or "Dtz") based	on 1E
		sub	esp, 8
		fstp	[esp+460h+var_460] ; %.4f
		fld	dword ptr [esi+4]
		sub	esp, 8
		fstp	[esp+468h+var_468] ; %.4f
		fld	dword ptr [esi]
		sub	esp, 8
		fstp	[esp+470h+var_470] ; %.4f
		push	edi		; %sp (name of obj again?)
		call	_GetSimTime
		push	eax		; @ %d (time)
		xor	eax, eax
		mov	ax, [esi+1Eh]
		lea	edx, [esp+478h+var_440]
		fld	dword ptr [esi+14h]
		xor	ecx, ecx
		push	edx		; %s
		mov	cl, [esi+20h]
		push	eax		; f %x
		push	ecx		; m %d
		lea	edx, [esp+484h+var_400]
		sub	esp, 8
		fstp	[esp+48Ch+var_48C] ; %.4f
		fld	dword ptr [esi+10h]
		sub	esp, 8
		fstp	[esp+494h+var_494] ; %.4f
		fld	dword ptr [esi+0Ch]
		sub	esp, 8
		fstp	[esp+49Ch+var_49C] ; %.4f
		push	edi		; name for "v"?	(vector?)
		push	offset aSv_4f_4f_4fMDF ; " %sv:	%.4f %.4f %.4f m %d f %x%s @ %d\n "...
		push	edx		; char *
		call	_sprintf
		lea	eax, [esp+4A8h+var_400]
		push	eax		; void *
		call	PushConsoleMessage
		add	esp, 64h
		pop	edi
		pop	esi
		mov	esp, ebp
		pop	ebp
		retn
__GhostPrintGhostPos endp