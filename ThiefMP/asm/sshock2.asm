.386
.model flat, c

.data 

INCLUDE asmdefs.inc
INCLUDE debugdefs.inc

.code

; Hook externals
EXTERN PlayerAction_GetAnimName: PROC
EXTERN PlayerAction_GetTagName: PROC
EXTERN AddConsoleMessage: PROC

cDynArray_ctor proc	; cDynArray_<sScrDeferredAction *,4>::cDynArray_<sScrDeferredAction *,4>(void)
		mov	eax, ecx
		mov	dword ptr [eax], 0
		mov	dword ptr [eax+4], 0
		retn
cDynArray_ctor endp

cTag_ctor proc
arg_0		= dword	ptr  4

		mov	eax, ecx
		mov	ecx, [esp+arg_0]
		mov	edx, [ecx]
		mov	[eax], edx
		mov	ecx, [ecx+4]
		mov	[eax+4], ecx
		retn	4
cTag_ctor endp

cDABase_dtor1 proc	;  cDABase<class cTag,4,class cDARawSrvFns<class cTag>>::~cDABase<class cTag,4,class cDARawSrvFns<class cTag>>(void)
		cmp	dword ptr [ecx], 0
		jz	short locret_56CCD2
		push	0
		push	8
		push	ecx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch
locret_56CCD2:
		retn
cDABase_dtor1	endp

DarkPlayerActionToSchemaIdx proc
var_44		= dword	ptr -44h
var_40		= dword	ptr -40h
var_38		= dword	ptr -38h
cMotionSchema	= dword	ptr -34h
cTagDBInput	= dword	ptr -30h
var_28		= dword	ptr -28h
var_24		= dword	ptr -24h
var_20		= dword	ptr -20h
var_1C		= dword	ptr -1Ch
var_18		= dword	ptr -18h
var_14		= dword	ptr -14h
var_10		= dword	ptr -10h
animName		= dword	ptr  8
tagName		= dword	ptr  0Ch

		sub	esp, 34h				; called from 51D8D3
		push	esi
		push	edi
		lea	ecx, [esp+3Ch+cTagDBInput]
		mov	[esp+3Ch+cMotionSchema], 0
		call	cDynArray_ctor
		lea	ecx, [esp+3Ch+var_20]
		call	cDynArray_ctor
		mov	eax, [esp+38h+tagName]
		push	eax
		lea	ecx, [esp+40h+var_20]
		call	cTagSet__FromString
		mov	edi, [esp+3Ch+var_1C]
		xor	esi, esi
		test	edi, edi
		jle	short loc_51CC89

loc_51CC57:
		mov	eax, [esp+3Ch+var_20]
		lea	ecx, [eax+esi*8]
		push	ecx
		lea	ecx, [esp+40h+var_18]
		call	cTag_ctor				; cTag::cTag(cTag	const &)
		push	0
		lea	edx, [esp+40h+var_18]
		push	0
		push	edx
		lea	ecx, [esp+48h+var_10]
		call	cTagDBInputTag_ctor
		push	eax
		lea	ecx, [esp+40h+cTagDBInput]
		call	cDABase__Append
		inc	esi
		cmp	esi, edi
		jl	short loc_51CC57

loc_51CC89:	
		lea	ecx, [esp+3Ch+var_28]
		call	cDynArray_ctor
		mov   eax, [esp+38h+animName]
		;push eax
		;mov	eax, [esp+40h+ePlayerAction]
		;push eax
		;call PlayerAction_GetAnimName ; just returns anim name so remove
		;add esp, 8
loc_51CCFB:
		test eax, eax
		jz	loc_51CE1A ; return
		mov esi, eax
		push	esi
		lea	ecx, [esp+40h+var_28]
		call	cTagSet__FromString
		mov	edi, [esp+3Ch+var_24]
		xor	esi, esi
		test	edi, edi
		jle	short loc_51CD4B

loc_51CD17:
		mov	eax, [esp+3Ch+var_28]
		push	0
		push	0
		mov	ecx, [eax+esi*8]
		mov	[esp+44h+var_18], ecx
		mov	edx, [eax+esi*8+4]
		lea	eax, [esp+44h+var_18]
		lea	ecx, [esp+44h+var_10]
		push	eax
		mov	[esp+48h+var_14], edx
		call	cTagDBInputTag_ctor
		push	eax
		lea	ecx, [esp+40h+cTagDBInput]
		call	cDABase__Append
		inc	esi
		cmp	esi, edi
		jl	short loc_51CD17

loc_51CD4B:
		mov	ecx, dword ptr ds: [006C52B4h]			;_g_pMotionDatabase
		lea	eax, [esp+3Ch+cMotionSchema]
		push	0
		push	eax
		mov	edx, [ecx]
		lea	eax, [esp+44h+cTagDBInput]
		push	eax
		push	0
		call	dword ptr [edx+4] ; cMotionDatabase::GetBestMatch
		test	eax, eax
		jnz	short loc_51CDC0 
		mov	eax, [esp+4Ch+var_38]
		test	eax, eax
		jz	short loc_51CD81
		push	0
		lea	ecx, [esp+50h+var_38]
		push	8
		push	ecx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CD81:
		mov	eax, [esp+4Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CD9A
		push	0
		lea	edx, [esp+50h+cTagDBInput]
		push	8
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CD9A:
		mov	eax, [esp+4Ch+var_40]
		test	eax, eax
		jz	loc_51CE45
		push	0
		lea	eax, [esp+50h+var_40]
		push	10h
		push	eax
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch
		or	eax, 0FFFFFFFFh
		pop	edi
		pop	esi
		add	esp, 34h
		retn
; ---------------------------------------------------------------------------

loc_51CDC0:
		mov	ecx, [esp+4Ch+var_44]
		mov	eax, [esp+4Ch+var_38]
		test	eax, eax
		mov	esi, [ecx+0Ch]
		jz	short loc_51CDE0
		push	0
		lea	edx, [esp+50h+var_38]
		push	8
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CDE0:
		mov	eax, [esp+4Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CDF9
		push	0
		lea	eax, [esp+50h+cTagDBInput]
		push	8
		push	eax
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CDF9:
		mov	eax, [esp+4Ch+var_40]
		test	eax, eax
		jz	short loc_51CE12
		push	0
		lea	ecx, [esp+50h+var_40]
		push	10h
		push	ecx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CE12:
		mov	eax, esi
		pop	edi
		pop	esi
		add	esp, 34h
		retn
; ---------------------------------------------------------------------------

loc_51CE1A:
		lea	ecx, [esp+3Ch+var_28]
		call	cDABase_dtor1
		lea	ecx, [esp+3Ch+var_20]
		call	cDABase_dtor1
		mov	eax, [esp+3Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CE45
		push	0
		lea	edx, [esp+40h+cTagDBInput]
		push	10h
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CE45:
		pop	edi
		or	eax, 0FFFFFFFFh
		pop	esi
		add	esp, 34h
		retn
DarkPlayerActionToSchemaIdx endp

ShockPlayerActionToSchemaIdx proc
var_44		= dword	ptr -44h
var_40		= dword	ptr -40h
var_38		= dword	ptr -38h
cMotionSchema	= dword	ptr -34h
cTagDBInput	= dword	ptr -30h
var_28		= dword	ptr -28h
var_24		= dword	ptr -24h
var_20		= dword	ptr -20h
var_1C		= dword	ptr -1Ch
var_18		= dword	ptr -18h
var_14		= dword	ptr -14h
var_10		= dword	ptr -10h
bCrouched		= dword	ptr  8
ePlayerAction		= dword	ptr  0Ch

		sub	esp, 34h				; called from 51D8D3
		push	esi
		push	edi
		lea	ecx, [esp+3Ch+cTagDBInput]
		mov	[esp+3Ch+cMotionSchema], 0
		call	cDynArray_ctor
		lea	ecx, [esp+3Ch+var_20]
		call	cDynArray_ctor
		mov	eax, [esp+3Ch+ePlayerAction]
		push	eax
		call PlayerAction_GetTagName
		add esp, 4
		push eax
		lea	ecx, [esp+40h+var_20]
		call	cTagSet__FromString
		mov	edi, [esp+3Ch+var_1C]
		xor	esi, esi
		test	edi, edi
		jle	short loc_51CC89

loc_51CC57:
		mov	eax, [esp+3Ch+var_20]
		lea	ecx, [eax+esi*8]
		push	ecx
		lea	ecx, [esp+40h+var_18]
		call	cTag_ctor				; cTag::cTag(cTag	const &)
		push	0
		lea	edx, [esp+40h+var_18]
		push	0
		push	edx
		lea	ecx, [esp+48h+var_10]
		call	cTagDBInputTag_ctor
		push	eax
		lea	ecx, [esp+40h+cTagDBInput]
		call	cDABase__Append
		inc	esi
		cmp	esi, edi
		jl	short loc_51CC57

loc_51CC89:	
		lea	ecx, [esp+3Ch+var_28]
		call	cDynArray_ctor
		mov   eax, [esp+3Ch+bCrouched]
		push eax
		mov	eax, [esp+40h+ePlayerAction]
		push eax
		call PlayerAction_GetAnimName
		add esp, 8
loc_51CCFB:
		test eax, eax
		jz	loc_51CE1A
		mov esi, eax
		push	esi
		lea	ecx, [esp+40h+var_28]
		call	cTagSet__FromString
		mov	edi, [esp+3Ch+var_24]
		xor	esi, esi
		test	edi, edi
		jle	short loc_51CD4B

loc_51CD17:
		mov	eax, [esp+3Ch+var_28]
		push	0
		push	0
		mov	ecx, [eax+esi*8]
		mov	[esp+44h+var_18], ecx
		mov	edx, [eax+esi*8+4]
		lea	eax, [esp+44h+var_18]
		lea	ecx, [esp+44h+var_10]
		push	eax
		mov	[esp+48h+var_14], edx
		call	cTagDBInputTag_ctor
		push	eax
		lea	ecx, [esp+40h+cTagDBInput]
		call	cDABase__Append
		inc	esi
		cmp	esi, edi
		jl	short loc_51CD17

loc_51CD4B:
		mov	ecx, dword ptr ds: [006C52B4h]			;_g_pMotionDatabase
		lea	eax, [esp+3Ch+cMotionSchema]
		push	0
		push	eax
		mov	edx, [ecx]
		lea	eax, [esp+44h+cTagDBInput]
		push	eax
		push	0
		call	dword ptr [edx+4] ; cMotionDatabase::GetBestMatch
		test	eax, eax
		jnz	short loc_51CDC0 
		mov	eax, [esp+4Ch+var_38]
		test	eax, eax
		jz	short loc_51CD81
		push	0
		lea	ecx, [esp+50h+var_38]
		push	8
		push	ecx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CD81:
		mov	eax, [esp+4Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CD9A
		push	0
		lea	edx, [esp+50h+cTagDBInput]
		push	8
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CD9A:
		mov	eax, [esp+4Ch+var_40]
		test	eax, eax
		jz	loc_51CE45
		push	0
		lea	eax, [esp+50h+var_40]
		push	10h
		push	eax
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch
		or	eax, 0FFFFFFFFh
		pop	edi
		pop	esi
		add	esp, 34h
		retn
; ---------------------------------------------------------------------------

loc_51CDC0:
		mov	ecx, [esp+4Ch+var_44]
		mov	eax, [esp+4Ch+var_38]
		test	eax, eax
		mov	esi, [ecx+0Ch]
		jz	short loc_51CDE0
		push	0
		lea	edx, [esp+50h+var_38]
		push	8
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CDE0:
		mov	eax, [esp+4Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CDF9
		push	0
		lea	eax, [esp+50h+cTagDBInput]
		push	8
		push	eax
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CDF9:
		mov	eax, [esp+4Ch+var_40]
		test	eax, eax
		jz	short loc_51CE12
		push	0
		lea	ecx, [esp+50h+var_40]
		push	10h
		push	ecx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CE12:
		mov	eax, esi
		pop	edi
		pop	esi
		add	esp, 34h
		retn
; ---------------------------------------------------------------------------

loc_51CE1A:
		lea	ecx, [esp+3Ch+var_28]
		call	cDABase_dtor1
		lea	ecx, [esp+3Ch+var_20]
		call	cDABase_dtor1
		mov	eax, [esp+3Ch+cTagDBInput]
		test	eax, eax
		jz	short loc_51CE45
		push	0
		lea	edx, [esp+40h+cTagDBInput]
		push	10h
		push	edx
		call	cDABaseSrvFns__DoResize
		add	esp, 0Ch

loc_51CE45:
		pop	edi
		or	eax, 0FFFFFFFFh
		pop	esi
		add	esp, 34h
		retn
ShockPlayerActionToSchemaIdx endp

ShockSetMode proc

var_2C		= dword	ptr -2Ch
var_28		= word ptr -28h
var_24		= dword	ptr -24h
var_20		= dword	ptr -20h
var_1C		= dword	ptr -1Ch
var_18		= dword	ptr -18h
var_14		= dword	ptr -14h
var_C		= dword	ptr -0Ch
arg_4		= dword	ptr  8

		sub	esp, 2Ch
		push	ebp
		push	esi
		mov	esi, [esp+34h+arg_4]
		push	edi
		push	esi
		call	_SetPlayerMode
		add	esp, 4
		mov	edi, dword ptr ds: [_gPlayerObj]
		mov	eax, dword ptr ds: [_g_PhysModels]
		lea	ecx, [esp+38h+var_18]
		push	edi
		push	ecx
		lea	ecx, [eax+1Ch]
		call	cHashSetBase__FindIndex
		mov	eax, [esp+38h+var_14]
		test	eax, eax
		jz	short loc_58D300
		mov	esi, [eax]
		jmp	short loc_58D302
; ---------------------------------------------------------------------------

loc_58D300:				; CODE XREF: cShockGameSrv::PlayerMode(int)+3Aj
		xor	esi, esi

loc_58D302:				; CODE XREF: cShockGameSrv::PlayerMode(int)+3Ej
		mov	[esp+38h+var_24], 0
		mov	[esp+38h+var_20], 0
		mov	[esp+38h+var_1C], 3FE66666h
		mov	eax, [esi+0F4h]
		lea	edx, [esp+38h+var_24]
		push	edx
		push	eax
		call	_mx_copy_vec
		mov	[esp+40h+var_24], 0
		mov	[esp+40h+var_20], 0
		mov	[esp+40h+var_1C], 0BF19999Ah
		mov	edx, [esi+0F4h]
		add	esp, 8
		lea	ecx, [esp+38h+var_24]
		add	edx, 18h
		push	ecx
		push	edx
		call	_mx_copy_vec
		mov	[esp+40h+var_24], 0
		mov	[esp+40h+var_20], 0
		mov	[esp+40h+var_1C], 0C0266666h
		mov	ecx, [esi+0F4h]
		add	esp, 8
		lea	eax, [esp+38h+var_24]
		add	ecx, 24h
		push	eax
		push	ecx
		call	_mx_copy_vec
		mov	[esp+40h+var_24], 0
		mov	[esp+40h+var_20], 0
		mov	[esp+40h+var_1C], 0C00CCCCDh
		mov	eax, [esi+0F4h]
		add	esp, 8
		lea	edx, [esp+38h+var_24]
		add	eax, 30h
		push	edx
		push	eax
		call	_mx_copy_vec
		mov	[esp+40h+var_24], 0
		mov	[esp+40h+var_20], 0
		mov	[esp+40h+var_1C], 0C0400000h
		mov	edx, [esi+0F4h]
		add	esp, 8
		lea	ecx, [esp+38h+var_24]
		add	edx, 0Ch
		push	ecx
		push	edx
		call	_mx_copy_vec
		mov	eax, [esi+0F0h]
		add	esp, 8
		add	eax, 10h
		mov	ecx, eax
		mov	edx, [ecx]
		mov	[esp+38h+var_2C], edx
		lea	edx, [esp+38h+var_2C]
		mov	cx, [ecx+4]
		push	edx
		sub	esp, 8
		mov	word ptr [esp+44h+var_2C+2], 0
		mov	word ptr [esp+44h+var_2C], 0
		mov	ebp, [esp+44h+var_2C]
		mov	edx, esp
		sub	esp, 8
		mov	[edx], ebp
		mov	[edx+4], cx
		mov	edx, [eax]
		mov	ax, [eax+4]
		mov	ecx, esp
		mov	[ecx], edx
		mov	[ecx+4], ax
		mov	ecx, esi
		call	cPhysModel__CheckAngleLimits
		mov	ecx, [esi+0F0h]
		mov	edx, [esp+38h+var_2C]
		add	ecx, 3Ch
		mov	[ecx], edx
		mov	ax, [esp+38h+var_28]
		mov	[ecx+4], ax
		mov	esi, [esi+0F0h]
		lea	ecx, [esi+3Ch]
		add	esi, 10h
		mov	edx, [ecx]
		mov	[esi], edx
		mov	ax, [ecx+4]
		lea	ecx, [esp+38h+var_C]
		push	ecx
		mov	[esi+4], ax
		call	_mx_zero_vec
		add	esp, 4
		lea	edx, [esp+38h+var_C]
		push	edx
		push	0
		push	edi
		nop ;call	_PhysSetSubModRotationalVelocity
		add	esp, 0Ch
		push	edi
		call	_ObjPosGet
		add	esp, 4
		push	eax
		push	edi
		call	_PhysSetModLocation
		add	esp, 8
		push	edi
		call	_PhysStopControlLocation
		add	esp, 4
		push	edi
		call	_PhysStopControlRotation
		add	esp, 4
		push	edi
		call	_PhysStopControlVelocity
		add	esp, 4
		push	edi
		call	_PhysStopControlRotationalVelocity
		add	esp, 4
		push	offset _IID_IAIManager
		call	_AppGetAggregated
		mov	edi, eax
		push	3E8h
		push	edi
		mov	eax, [edi]
		call	dword ptr [eax+40h] ; cAIManager::SetIgnoreSoundTime
		mov	ecx, dword ptr ds: [_gPlayerObj]
		; removed GhostNotify call
		test	edi, edi
		jz	short loc_58D513
		mov	edx, [edi]
		push	edi
		call	dword ptr [edx+8]

loc_58D513:
		xor	eax, eax
		pop	edi
		pop	esi
		pop	ebp
		add	esp, 2Ch
		retn
; ---------------------------------------------------------------------------
ShockSetMode endp

cPhysModels_Get proc

var_14		= dword	ptr -14h
var_4		= dword	ptr -4
arg_0		= dword	ptr  4

		mov ecx, dword ptr ds: [_g_PhysModels]
		push	ecx
		mov	eax, [ecx+1Ch]
		push	ebx
		push	ebp
		mov	ebp, [esp+0Ch+arg_0]
		push	esi
		lea	esi, [ecx+1Ch]
		push	edi
		mov	[esp+14h+var_4], ecx
		push	ebp
		mov	ecx, esi
		call	dword ptr [eax+10h]
		xor	edx, edx
		mov	ecx, [esi+4]
		div	dword ptr [esi+8]
		mov	edi, [ecx+edx*4]
		test	edi, edi
		jz	short loc_53C1B0

loc_53C188:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+42j
		mov	edx, [edi]
		mov	ebx, [esi]
		push	ebp
		push	edx
		mov	ecx, esi
		call	dword ptr [ebx+0Ch]
		push	eax
		mov	ecx, esi
		call	dword ptr [ebx+8]
		test	eax, eax
		jnz	short loc_53C1A6
		mov	edi, [edi+4]
		test	edi, edi
		jnz	short loc_53C188
		jmp	short loc_53C1B0
; 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

loc_53C1A6:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+3Bj
		test	edi, edi
		jz	short loc_53C1B0
		mov	eax, [edi]
		test	eax, eax
		jnz	short loc_53C1ED

loc_53C1B0:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+26j
					; class	cPhysModel * cPhysModels::Get(int)+44j	...
		mov	eax, [esp+14h+var_4]
		push	ebp
		mov	edx, [eax+30h]
		lea	esi, [eax+30h]
		mov	ecx, esi
		call	dword ptr [edx+10h]
		xor	edx, edx
		div	dword ptr [esi+8]
		mov	eax, [esi+4]
		mov	edi, [eax+edx*4]
		test	edi, edi
		jz	short loc_53C1EB

loc_53C1CF:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+89j
		mov	ecx, [edi]
		mov	ebx, [esi]
		push	ebp
		push	ecx
		mov	ecx, esi
		call	dword ptr [ebx+0Ch]
		push	eax
		mov	ecx, esi
		call	dword ptr [ebx+8]
		test	eax, eax
		jnz	short loc_53C1F5
		mov	edi, [edi+4]
		test	edi, edi
		jnz	short loc_53C1CF

loc_53C1EB:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+6Dj
					; class	cPhysModel * cPhysModels::Get(int)+97j
		xor	eax, eax

loc_53C1ED:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+4Ej
		pop	edi
		pop	esi
		pop	ebp
		pop	ebx
		pop	ecx
		retn
; 컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�

loc_53C1F5:				; CODE XREF: class cPhysModel *	cPhysModels::Get(int)+82j
		test	edi, edi
		jz	short loc_53C1EB
		mov	eax, [edi]
		pop	edi
		pop	esi
		pop	ebp
		pop	ebx
		pop	ecx
		retn
cPhysModels_Get endp

end