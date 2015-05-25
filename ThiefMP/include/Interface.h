#pragma once

class cGenericAggregate;

#pragma pack(push, 1)

interface IScriptServiceBase : public IUnknown
{
	STDMETHOD_(void,Init)(void);
	STDMETHOD_(void,End)(void);
};

typedef void (*_NetListener)(unsigned long, unsigned long, void*);


interface INetManager : public IUnknown
{
public:
//#if (GAME == GAME_THIEF || GAME == GAME_SHOCK)
	cGenericAggregate* m_aggregate;	// 0x4
	DWORD unk;								// 0x8
	int m_refCount;						// 0xC
	int m_SuspendMessageDepth;			// 0x10
	char m_playerName[kMaxPlayerName];// ends at 0x49
	char m_mediaName[128];				// ends at 0xB1
	char m_gameName[256];				// ends at 0x1B1
	BOOL m_bSynchDone;						// 0x1B5
	int m_numPlayers;							// 0x1B9
	BYTE unk3[28];									// ends at 0x1D9
	int m_numStartedSynch;			// 0x1D9
	int m_numSynched;						// 0x1DD
	int m_playerNum; // 0x1E1
	DWORD x1e5;
	cDynArray<struct NetMsgParser> m_pParsers; // 0x1E9 - 0x1ED
	cDynArray<_NetListener> m_pListeners; // 0x1F1 - 0x1F5      cDynArray<sMessageParserInfo>? dromed.00595412
	int m_nextListenerHandle; // 0x1F9
	cDynArray<cAvatar*> m_Avatars; // 0x1FD - 0x201
	cDynArray<struct sPlayerInfoEntry> PlayerInfoTable; // 0x205 - 0x209
	DWORD x20d, x211;
	int m_maxPlayers; // 0x215 (forced to 4?)
	class cNetMsg* m_pPlayerContentsMsg; // 0x219
	class cNetMsg* m_pFinishSynchMsg; // 0x21D
	class cNetMsg* m_pSynchFlushMsg; // 0x221
	class cNetMsg* m_pStartSynchMsg; // 0x225
	class cNetMsg* m_pPlayerInfoMsg; // 0x229
	int m_maxMsgsFrame;					// 0x22D
	int m_timeout; // 0x231 - default timeout for packets (60000)
	int m_playerTimeout;					// 0x235 ???
	BOOL m_bInNonNetworkLevel; // 0x239
	BOOL m_bShouldSendPlayerContents; // 0x23D

	// Most of these appear to be dynamic arrays
	DWORD x241, x245, x249, x24d, x251, x255, x259, x25d, x261, x265, x269, x26d, x271, x275;
//#endif

	STDMETHOD_(int,Networking)(void);
	STDMETHOD_(int,IsNetworkGame)(void);
	STDMETHOD_(unsigned long,NumPlayers)(void);
	STDMETHOD_(unsigned long,MyPlayerNum)(void);
	STDMETHOD_(unsigned long,ObjToPlayerNum)(int);
	STDMETHOD_(int,PlayerNumToObj)(unsigned long);
	STDMETHOD_(void,SetPlayerName)(const char*);
	STDMETHOD_(char*,GetPlayerName)(int);
	STDMETHOD_(const char*,GetPlayerAddress)(int);
	STDMETHOD_(int,Host)(const char*,const char*);
	STDMETHOD_(int,Join)(const char*,const char*,const char*);
	STDMETHOD_(unsigned long,GetTimeout)(void);
	STDMETHOD_(void,SetTimeout)(unsigned long);
	STDMETHOD_(void,SuspendMessaging)(void);
	STDMETHOD_(int,Suspended)(void);
	STDMETHOD_(void,ResumeMessaging)(void);
	STDMETHOD_(void,Send)(int,void*,unsigned long,int);
	STDMETHOD_(void,Broadcast)(void*,unsigned long,int);
	STDMETHOD_(void,MetagameBroadcast)(void*,unsigned long,int);
	STDMETHOD_(int,DefaultHost)(void);
	STDMETHOD_(int,AmDefaultHost)(void);
	STDMETHOD_(const char*,GetDefaultHostName)(void);
	STDMETHOD_(unsigned char,RegisterMessageParser)(void (__stdcall *)(const sNetMsg_Generic*,unsigned long,int,void*),const char*,unsigned long,void*);
	STDMETHOD_(void,NetPlayerIterStart)(struct sPropertyObjIter*) const;
	STDMETHOD_(int,NetPlayerIterNext)(struct sPropertyObjIter*,int*) const;
	STDMETHOD_(int,Listen)(void (__cdecl *)(unsigned long,unsigned long,void*),unsigned long,void*);
	STDMETHOD_(struct sGlobalObjID,ToGlobalObjID)(int);
	STDMETHOD_(int,FromGlobalObjID)(struct sGlobalObjID*);
	STDMETHOD_(int,OriginatingPlayer)(void);
	STDMETHOD_(void,StartSynch)(void);
	STDMETHOD_(void,NonNetworkLevel)(void);
	STDMETHOD_(void,NormalLevel)(void);
	STDMETHOD_(void,PreFlush)(void);
	STDMETHOD_(int,SynchFlush)(int (__stdcall *)(void*),void*);
	STDMETHOD_(const char*,GetPlayerNameByNum)(int);
	STDMETHOD_(const char*,GetPlayerAddressByNum)(int);
	STDMETHOD_(void,Leave)(void);
	STDMETHOD_(void,Unlisten)(int);
	STDMETHOD_(void,SetMaxPlayers)(int);
	STDMETHOD_(void,dtor)(uint);
	STDMETHOD_(unsigned long,ToNetPlayerID)(int playerObject); /* Returns DPNID */
	STDMETHOD_(int,FromNetPlayerID)(unsigned long); /* Returns player object from DPNID */
};

interface INetAppServices : public IUnknown
{
	STDMETHOD_(int,MyAvatarArchetype)(void);
};

interface IObjectSystem : public IUnknown
{
	STDMETHOD_(int,Create)(int,unsigned long);
	STDMETHOD_(int,BeginCreate)(int,unsigned long);
	STDMETHOD_(long,EndCreate)(int);
	STDMETHOD_(long,Destroy)(int);
	STDMETHOD_(long,CloneObject)(int,int);
	STDMETHOD_(long,Reset)(void);
	STDMETHOD_(int,Exists)(int);
	STDMETHOD_(long,NameObject)(int,const char*);
	STDMETHOD_(const char*,GetName)(int);
	STDMETHOD_(int,GetObjectNamed)(const char*);
	STDMETHOD_(long,SetObjTransience)(int,int);
	STDMETHOD_(int,ObjIsTransient)(int);
	STDMETHOD_(long,SetObjImmobility)(int,int);
	STDMETHOD_(int,ObjIsImmobile)(int);
	STDMETHOD_(struct IObjectQuery*,Iter)(unsigned long);
	STDMETHOD_(struct IProperty*,GetCoreProperty)(unsigned long);
	STDMETHOD_(long,DatabaseNotify)(unsigned long,void*);
	STDMETHOD_(long,f17)(void);
	STDMETHOD_(int,Listen)(struct sObjListenerDesc*);
	STDMETHOD_(long,Unlisten)(int);
	STDMETHOD_(int,MaxObjID)(void);
	STDMETHOD_(int,MinObjID)(void);
	STDMETHOD_(int,ActiveObjects)(void);
	STDMETHOD_(long,Lock)(void);
	STDMETHOD_(long,Unlock)(void);
	STDMETHOD_(int,IsObjSavePartition)(int,unsigned long);
	STDMETHOD_(int,IsObjLoadPartition)(int,unsigned long);
	STDMETHOD_(long,ObjDefaultPartition)(int);
	STDMETHOD_(int,RemapOnLoad)(int);
	STDMETHOD_(long,SetSubPartitionFilter)(unsigned long,const struct sObjPartitionFilter*);

	BYTE unk[0x4C];
	int mLockCount;
};

interface IObjectQuery : public IUnknown
{
	STDMETHOD_(int,Done)(void) PURE;
	STDMETHOD_(int,Object)(void) PURE;
	STDMETHOD_(long,Next)(void) PURE;
};

interface INet: public IUnknown
{
	STDMETHOD_(long,AddPlayerToGroup)(unsigned long, unsigned long);
	STDMETHOD_(long,Close)(void);
	STDMETHOD_(long,CreateGroup)(ulong *,DPNAME *,void *,ulong,ulong);
	STDMETHOD_(long,CreatePlayer)(ulong *,DPNAME *,void *,void *,ulong,ulong);
	STDMETHOD_(long,DeletePlayerFromGroup)(ulong,ulong);
	STDMETHOD_(long,DestroyGroup)(ulong);
	STDMETHOD_(long,DestroyPlayer)(ulong);
	STDMETHOD_(long,EnumGroupPlayers)(ulong,struct _GUID *,int (*)(ulong,ulong,DPNAME const *,ulong,void *),void *,ulong);
	STDMETHOD_(long,EnumGroups)(struct _GUID *,int (*)(ulong,ulong,DPNAME const *,ulong,void *),void *,ulong);
	STDMETHOD_(long,EnumPlayers)(struct _GUID *,int (*)(ulong,ulong,DPNAME const *,ulong,void *),void *,ulong);
	STDMETHOD_(long,EnumSessions)(DPSESSIONDESC2 *,ulong,int (*)(DPSESSIONDESC2 const *,ulong *,ulong,void *),void *,ulong);
	STDMETHOD_(long,GetCaps)(struct DPCAPS *,ulong);
	STDMETHOD_(long,GetGroupData)(ulong,void *,ulong *,ulong);
	STDMETHOD_(long,GetGroupName)(ulong,void *,ulong *);
	STDMETHOD_(long,GetMessageCount)(ulong, ulong *);
	STDMETHOD_(long,GetPlayerAddress)(ulong,void *,ulong *);
	STDMETHOD_(long,GetPlayerCaps)(ulong, DPCAPS*, ulong);
	STDMETHOD_(long,GetPlayerData)(ulong,void *,ulong *,ulong);
	STDMETHOD_(long,GetPlayerName)(ulong,void *,ulong *);
	STDMETHOD_(long,GetSessionDesc)(void *, ulong *);
	STDMETHOD_(long,Initialize)(GUID*);
	STDMETHOD_(long,Open)(DPSESSIONDESC2 *,ulong);
	STDMETHOD_(long,Receive)(ulong *,ulong *,ulong,void *,ulong *);
	STDMETHOD_(long,Send)(ulong,ulong,ulong,void *,ulong);
	STDMETHOD_(long,SetGroupData)(ulong,void *,ulong,ulong);
	STDMETHOD_(long,SetGroupName)(ulong,DPNAME *,ulong);
	STDMETHOD_(long,SetPlayerData)(ulong,void *,ulong,ulong);

	// ...
};

interface IObjectNetworking : public IUnknown
{
	STDMETHOD_(void,ClearTables)(void);
	STDMETHOD_(void,ObjRegisterProxy)(int,short,int);
	STDMETHOD_(int,ObjGetProxy)(int,short);
	STDMETHOD_(void,ObjDeleteProxy)(int);
	STDMETHOD_(int,ObjIsProxy)(int);	
	STDMETHOD_(int,ObjHostedHere)(int);
	STDMETHOD_(int,ObjLocalOnly)(int);
	STDMETHOD_(int,ObjHostPlayer)(int);
	STDMETHOD_(short,ObjHostObjID)(int);
	STDMETHOD_(void,ObjHostInfo)(int,int*,short*);
	STDMETHOD_(void,ObjTakeOver)(int);
	STDMETHOD_(void,ObjGiveTo)(int,int,int);
	STDMETHOD_(void,ObjGiveWithoutObjID)(int,int);
	STDMETHOD_(void,StartBeginCreate)(int,int);
	STDMETHOD_(void,FinishBeginCreate)(int);
	STDMETHOD_(void,StartEndCreate)(int);
	STDMETHOD_(void,FinishEndCreate)(int);
	STDMETHOD_(void,StartDestroy)(int);
	STDMETHOD_(void,FinishDestroy)(int);
	STDMETHOD_(void,unk1)(int);
	STDMETHOD_(void,NotifyObjRemapped)(int,int);
	STDMETHOD_(void,ResolveRemappings)(ulong,int);
	STDMETHOD_(void,RequestDestroy)(int);

	DWORD x04, x08, x0c, x10;
	INetManager* mpNetMan; // 0x14
	IObjectSystem* mpObjSys; // 0x18
	DWORD x1c;
	int m_lastBeginCreateObject; // last local object ID sent to the begin create handler
	int m_lastBeginCreateSender; // object ID of the last player to send a begin create msg
	BOOL m_bGameModeStarted; // 0x28
	BOOL m_bIsNetworkGame; // 0x2C - set in network listener
	DWORD x30, x34, x38;
	class cHashTable* m_pProxyObjTable; // 0x3C
	DWORD x40, x44, x48, x4c, x50, x54, x58, x5c, x60;
	class cNetMsg* m_pReqDestroyMsg; // 0x64
	class cNetMsg* m_pCreateObjPosMsg; // 0x68
	class cNetMsg* m_pBeginCreateMsg; // 0x6C
	class cNetMsg* m_pEndCreateMsg; // 0x70
	class cNetMsg* m_pDestroyObjectMsg; // 0x74
	class cNetMsg* m_pNewOwnerMsg; // 0x78
	class cNetMsg* m_pRemapMsg; // 0x7C
	class cNetMsg* m_pCheckRemapMsg; // 0x80
	class cNetMsg* m_pGiveObjectMsg; // 0x84
};

interface  IInputBinder : public IUnknown
{
	STDMETHOD_(char*,Init)(struct _IB_var*,char*);
	STDMETHOD_(char*,Term)(void);
	STDMETHOD_(void,GetHandler)(unsigned char (__cdecl **)(struct _ui_event *,struct _Region *,void *));
	STDMETHOD_(void,SetValidEvents)(unsigned long);
	STDMETHOD_(unsigned long,GetValidEvents)(void);
	STDMETHOD_(int,Bind)(char*,char*);
	STDMETHOD_(int,Unbind)(char*);
	STDMETHOD_(void,QueryBind)(char*,char*,long);
	STDMETHOD_(void,GetVarValue)(char*,char*,long);
	STDMETHOD_(char*,ProcessCmd)(char*);
	STDMETHOD_(void,TrapBind)(char *,int (__cdecl *)(char *,char *,void *),void (__cdecl *)(int),void *);
	STDMETHOD_(char*,Update)(void);
	STDMETHOD_(void,PollAllKeys)(void);
	STDMETHOD_(void,RegisterJoyProcObj)(void*);
	STDMETHOD_(void,LoadBndContexted)(char*);
	STDMETHOD_(void,ContextAssociate)(struct _sBindContext*);
	STDMETHOD_(char*,LoadBndFile)(char*,unsigned long,char*);
	STDMETHOD_(char*,SaveBndFile)(char*,char*);
	STDMETHOD_(int,VarSet)(_IB_var*);
	STDMETHOD_(int,VarSetn)(_IB_var*,long);
	STDMETHOD_(char*,VarUnset)(char**,int);
	STDMETHOD_(char*,VarUnsetn)(char**,long,int);
	STDMETHOD_(char*,VarUnsetAll)(void);
	/* Arguments:
	/*
	/* bindConstant - ID constant associated with the bind file, set in a call to LoadBndFile.
	*/
	STDMETHOD_(int,SetContext)(unsigned long bindConstant,int);
	STDMETHOD_(void,GetContext)(unsigned long*);
	STDMETHOD_(void,SetMasterAggregation)(int (__cdecl *)(struct _intrnl_var_channel * *,long,char *));
	STDMETHOD_(void,SetMasterProcessCallback)(char * (__cdecl *)(char *,char *,int));
	STDMETHOD_(char*,GetControlFromCmdStart)(char*,char*);
	STDMETHOD_(char*,GetControlFromCmdNext)(char*);
	STDMETHOD_(void,SetResPath)(char*);
	STDMETHOD_(char*,DecomposeControl)(char*,char*[32],long*);
	STDMETHOD_(void,Reset)(void);
};

interface IContainSys : public IUnknown
{
	STDMETHOD_(long,Listen)(int,int (__cdecl *)(enum eContainsEvent,int,int,int,void*),void*);
	STDMETHOD_(int,CheckCallback)(int,eContainsEvent,int,int,int);
	STDMETHOD_(long,Add)(int,int,int,unsigned int);
	STDMETHOD_(long,Remove)(int,int);
	STDMETHOD_(long,SetContainType)(int,int,int);
	STDMETHOD_(int,IsHeld)(int,int);
	STDMETHOD_(int,Contains)(int,int);
	STDMETHOD_(int,GetContainer)(int);
	STDMETHOD_(long,MoveAllContents)(int,int,int);
	STDMETHOD_(long,CombineTry)(int,int,int);
	STDMETHOD_(int,CombineCount)(int,int);
	STDMETHOD_(long,CombineAdd)(int,int,int,unsigned long);
	STDMETHOD_(int,CanCombine)(int,int,int);
	STDMETHOD_(int,CanCombineContainer)(int,int,int);
	STDMETHOD_(int,RemoveFromStack)(int,unsigned long,unsigned long);
	STDMETHOD_(int,StackAdd)(int,int,unsigned long);
	STDMETHOD_(struct IIntProperty*,StackCountProp)(void);
	STDMETHOD_(sContainIter*,IterStart)(int object);
	STDMETHOD_(sContainIter*,IterStartType)(int,int,int);
#ifdef _THIEFBUILD
	STDMETHOD_(sContainIter*,IterStartInherits)(int);
	STDMETHOD_(sContainIter*,IterStartInheritsType)(int,int,int);
#endif
	STDMETHOD_(int,IterNext)(sContainIter*);
	STDMETHOD_(void,IterEnd)(sContainIter*);
#ifdef _THIEFBUILD
	STDMETHOD_(long,DatabaseMessage)(unsigned long,IUnknown*);
#endif
};

interface IStoredPropertyNetworking : public IUnknown
{
	virtual int __stdcall SendPropertyMsg(int,struct sDatum,unsigned long);
	virtual void __stdcall ReceivePropertyMsg(struct sNetMsg_Generic const *,unsigned long,int);
};

interface IReactions : public IUnknown
{
	STDMETHOD_(unsigned long,Add)(struct sReactionDesc*,unsigned long (__stdcall *)(struct sReactionEvent*,const struct sReactionParam*,void*),void*);
	STDMETHOD_(unsigned long,GetReactionNamed)(const char*);
	STDMETHOD_(const sReactionDesc*,DescribeReaction)(unsigned long);
	STDMETHOD_(struct IReactionQuery*,QueryAll)(void);
	STDMETHOD_(unsigned long,React)(unsigned long, struct sReactionEvent*,const struct sReactionParam);
};

interface IProperty : public IUnknown
{
	STDMETHOD_(const struct sPropertyDesc*,Describe)(void);
	STDMETHOD_(int,GetID)(void);
	STDMETHOD_(const struct sPropertyTypeDesc*,DescribeType)(void) const;
	STDMETHOD_(long,Create)(int);
	STDMETHOD_(long,Copy)(int,int);
	STDMETHOD_(long,Delete)(int);
	STDMETHOD_(int,IsRelevant)(int);
	STDMETHOD_(int,IsSimplyRelevant)(int);
	STDMETHOD_(int,Touch)(int);
	STDMETHOD_(void,Notify)(unsigned long,void*);
	STDMETHOD_(PropListenerHandle,Listen)(unsigned long, PropListenFunc, PropListenerData);
	STDMETHOD_(long,Unlisten)(struct PropListenerHandle);
	STDMETHOD_(void,IterStart)(struct sPropertyObjIter*);
	STDMETHOD_(int,IterNext)(struct sPropertyObjIter*,int*);
	STDMETHOD_(void,IterStop)(struct sPropertyObjIter*);
};

interface IPickLockSrv : public IScriptServiceBase
{
	STDMETHOD_(int,Ready)(int,int);
	STDMETHOD_(int,UnReady)(int,int);
	STDMETHOD_(int,StartPicking)(int playerObject, int lockpickObject, int lockedObject);
	STDMETHOD_(int,FinishPicking)(int lockpickObject);
	STDMETHOD_(int,CheckPick)(int,int,int);
	STDMETHOD_(int,DirectMotion)(int);
};

interface IGenericProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,void*);
	STDMETHOD_(int,TouchValue)(int,void*);
	STDMETHOD_(int,Get)(int,void**);
	STDMETHOD_(int,GetSimple)(int,void**);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,void**);
};

interface IIntProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,int);
	STDMETHOD_(int,TouchValue)(int,int);
	STDMETHOD_(int,Get)(int,int*);
	STDMETHOD_(int,GetSimple)(int,int*);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,int*);
};

interface IAIRatingProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,eAIRating);
	STDMETHOD_(int,TouchValue)(int,eAIRating);
	STDMETHOD_(int,Get)(int,eAIRating*);
	STDMETHOD_(int,GetSimple)(int,eAIRating*);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,eAIRating*);
};

interface ILabelProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,Label*);
	STDMETHOD_(int,TouchValue)(int,Label*);
	STDMETHOD_(int,Get)(int,Label**);
	STDMETHOD_(int,GetSimple)(int,Label**);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,Label**);
};

interface IFloatProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,float);
	STDMETHOD_(int,TouchValue)(int,float);
	STDMETHOD_(int,Get)(int,float*);
	STDMETHOD_(int,GetSimple)(int,float*);
	STDMETHOD_(int,IterNextValue)(sPropertyObjIter*,int*,float*);
};

interface IBoolProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,int);
	STDMETHOD_(int,TouchValue)(int,int);
	STDMETHOD_(int,Get)(int,int*);
	STDMETHOD_(int,GetSimple)(int,int*);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,int*);
};

interface IStringProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,const char*);
	STDMETHOD_(int,TouchValue)(int,const char*);
	STDMETHOD_(int,Get)(int,const char* *);
	STDMETHOD_(int,GetSimple)(int,const char**);
	STDMETHOD_(int,IterNextValue)(struct sPropertyObjIter*,int*,const char* *);
};

interface IVectorProperty : public IProperty
{
	STDMETHOD_(int,Set)(int,const mxs_vector*);
	STDMETHOD_(int,TouchValue)(int,const mxs_vector*);
	STDMETHOD_(int,Get)(int,mxs_vector**);
	STDMETHOD_(int,GetSimple)(int,mxs_vector**);
	STDMETHOD_(int,IterNextValue)(sPropertyObjIter*,int*,mxs_vector**) const;
};

interface IActReactSrv : public IScriptServiceBase
{
	STDMETHOD_(long,React)(class reaction_kind,float,object,object,const cMultiParm &,const cMultiParm &,const cMultiParm &,const cMultiParm &,const cMultiParm &,const cMultiParm &,const cMultiParm &,const cMultiParm &);
	STDMETHOD_(long,Stimulate)(int stimulatedObj, int stimObj ,float intensity, int stimSource);
	STDMETHOD_(int,GetReactionNamed)(const char *);
	STDMETHOD_(class cScrStr*,GetReactionName)(class cScrStr &,long);
	STDMETHOD_(long,SubscribeToStimulus)(int,int);
	STDMETHOD_(long,UnsubscribeToStimulus)(int,int);
	STDMETHOD_(long,BeginContact)(int,int);
	STDMETHOD_(long,EndContact)(int,int);
	STDMETHOD_(long,SetSingleSensorContact)(int,int);
};

interface IQuestData : public IUnknown
{
	STDMETHOD_(long,Create)(const char *,int, int);
	STDMETHOD_(long,Set)(const char *,int);
	STDMETHOD_(int,Get)(const char *);
	STDMETHOD_(int,Exists)(const char *);
	STDMETHOD_(long,Delete)(const char *);
	STDMETHOD_(struct IQuestDataIter*,Iter)(int);
	STDMETHOD_(long,DeleteAll)(void);
	STDMETHOD_(long,DeleteAllType)(int);
	STDMETHOD_(int,Save)(long (__cdecl *)(void *,unsigned int,unsigned int),int);
	STDMETHOD_(int,Load)(long (__cdecl *)(void *,unsigned int,unsigned int),int);
	STDMETHOD_(long,SubscribeMsg)(int,const char *,int);
	STDMETHOD_(long,UnsubscribeMsg)(int,const char *);
	STDMETHOD_(long,ObjDeleteListener)(int);
	STDMETHOD_(long,UnsubscribeAll)(void);
	STDMETHOD_(long,ObjSpewListen)(int);
	STDMETHOD_(long,Filter)(int (__cdecl *)(const char *,int,int),void *);
	STDMETHOD_(long,doCreate)(const char *,int, int);
	STDMETHOD_(long,doSet)(const char *,int);

	BYTE unk[0x50];
	class cNetMsg* pSetQuestData;
};

interface IKeySys : public IUnknown
{
	STDMETHOD_(int,TryToUseKey)(int,int,int eKeyUse);
};

struct IAIConversationManager : public IUnknown
{
	STDMETHOD_(int,Start)(int);
	STDMETHOD_(void,SetActorObj)(int,int,int);
	STDMETHOD_(int,GetActorObj)(int,int,int *);
	STDMETHOD_(void,RemoveActorObj)(int,int);
	STDMETHOD_(int,GetNumActors)(int);
	STDMETHOD_(int,GetActorIDs)(int,int *);
	STDMETHOD_(void,Frame)(void);
	STDMETHOD_(void,ListenConversationEnd)(void (__cdecl *)(int));
	STDMETHOD_(void,NotifyConversationEnd)(int);
	// ...
};

interface IRelation : public IUnknown
{
	STDMETHOD_(const struct sRelationDesc*,Describe)(void);
	STDMETHOD_(short,GetID)(void);
	STDMETHOD_(long,SetID)(short);
	STDMETHOD_(const struct sRelationDataDesc*,DescribeData)(void);
	STDMETHOD_(IRelation*,Inverse)(void);
	STDMETHOD_(long,Add)(int,int);
	STDMETHOD_(long,AddFull)(int,int,const void*);
	STDMETHOD_(long,Remove)(long);
	STDMETHOD_(int,Get)(long,sLink *) const;
	STDMETHOD_(long,SetData)(long,void *);
	STDMETHOD_(void*,GetData)(long);
	STDMETHOD_(struct ILinkQuery*,Query)(int,int);
	STDMETHOD_(long,Notify)(unsigned long,void *);	
	STDMETHOD_(long,Listen)(unsigned long,void (__stdcall *)(struct sRelationListenMsg *,void *),void*);
	STDMETHOD_(long,GetSingleLink)(int,int);
	STDMETHOD_(int,AnyLinks)(int,int);
};

interface ILinkManager : public IUnknown
{
	STDMETHOD_(struct IRelation*,GetRelation)(short);
	STDMETHOD_(struct IRelation*,GetRelationNamed)(const char *);
	STDMETHOD_(long,Notify)(unsigned long,void *);
	STDMETHOD_(long,IterStart)(struct sRelationIter *);
	STDMETHOD_(int,IterNext)(struct sRelationIter *,short *);
	virtual void nullsub() = 0;
	STDMETHOD_(long,Add)(int,int,short);
	STDMETHOD_(long,AddFull)(int,int,short,void *);
	STDMETHOD_(long,Remove)(long);
	STDMETHOD_(int,Get)(long,sLink *) const;
	STDMETHOD_(long,SetData)(long,void *);
	STDMETHOD_(void*,GetData)(long);
	STDMETHOD_(struct ILinkQuery*,Query)(int,int,short);
	STDMETHOD_(long,RemapOnLoad)(long);
	STDMETHOD_(long,GetSingleLink)(short,int,int);
	STDMETHOD_(int,AnyLinks)(short,int,int);
	STDMETHOD_(short,AddRelation)(struct IRelation *);
	STDMETHOD_(long,RemoveRelation)(short);
	STDMETHOD_(long,ReplaceRelation)(short,struct IRelation *);
	STDMETHOD_(long,AddQueryDB)(struct ILinkQueryDatabase *,unsigned long);
	STDMETHOD_(long,RemoveQueryDB)(struct ILinkQueryDatabase *,unsigned long);
	STDMETHOD_(int,Lock)(void);
	STDMETHOD_(long,Unlock)(int);
	STDMETHOD_(long,GetLockCount)(void);
	STDMETHOD_(int,LinkValid)(long,int);
	STDMETHOD_(long,LinkBirth)(long);
	STDMETHOD_(int,LinkDeath)(long);
};

interface IDamageSrv : public IScriptServiceBase
{
	STDMETHOD_(long,Damage)(class object,class object,int,int);
	STDMETHOD_(long,Slay)(class object,class object);
	STDMETHOD_(long,Resurrect)(class object,class object);
};

interface ILinkQuery : public IUnknown
{
	STDMETHOD_(int,Done)(void) const PURE;
	STDMETHOD_(long,Link)(sLink *) const PURE;
	STDMETHOD_(long,ID)(void) const PURE;
	STDMETHOD_(void*,Data)(void) const PURE;
	STDMETHOD_(long,Next)(void) PURE;
	STDMETHOD_(ILinkQuery*,Inverse)(void) PURE;
};

interface IInventory : public IUnknown
{
	STDMETHOD_(enum eInventoryType,GetType)(int);
	STDMETHOD_(long,Add)(int);
	STDMETHOD_(long,Remove)(int);
	STDMETHOD_(long,Select)(int);
	STDMETHOD_(int,Selection)(enum eWhichInvObj);
	STDMETHOD_(long,ClearSelection)(enum eWhichInvObj);
	STDMETHOD_(int,CycleSelection)(enum eWhichInvObj,enum eCycleDirection);
	STDMETHOD_(long,Wield)(eWhichInvObj);
	STDMETHOD_(int,WieldedObj)(void);
	STDMETHOD_(int,WieldingJunk)(void);
	STDMETHOD_(long,DatabaseNotify)(unsigned long,struct ITagFile*);

	DWORD x04, x08, x0c;
	IIntProperty* m_pInvTypeProp; // cInventory + 0x8
	IStringProperty* m_pCycleOrderProp; // cInventory + 0xC
};

interface ITraitManager : public IUnknown
{
	STDMETHOD_(long,CreateBaseArchetype)(const char *,int *);
	STDMETHOD_(int,CreateArchetype)(const char *,int);
	STDMETHOD_(int,CreateConcreteArchetype)(const char *,int);
	STDMETHOD_(int,GetArchetype)(int);
	STDMETHOD_(long,SetArchetype)(int,int);
	STDMETHOD_(int,IsArchetype)(int);
	STDMETHOD_(int,RootMetaProperty)(void);
	STDMETHOD_(int,CreateMetaProperty)(const char *,int);
	STDMETHOD_(int,IsMetaProperty)(int);
	STDMETHOD_(long,AddObjMetaProperty)(int,int);
	STDMETHOD_(long,AddObjMetaPropertyPrioritized)(int,int,long);
	STDMETHOD_(long,RemoveObjMetaProperty)(int,int);
	STDMETHOD_(long,RemoveObjMetaPropertyPrioritized)(int,int,long);
	STDMETHOD_(int,ObjHasDonor)(int,int);
	STDMETHOD_(int,ObjHasDonorIntrinsically)(int,int);
	STDMETHOD_(int,ObjIsDonor)(int);
	STDMETHOD_(IObjectQuery*,Query)(int,unsigned long);
	STDMETHOD_(struct ITrait*,CreateTrait)(const struct sTraitDesc *,const struct sTraitPredicate *);
	STDMETHOD_(long,Listen)(void (__stdcall *)(struct sHierarchyMsg const *,void *),void *,void *);
	STDMETHOD_(long,AddObject)(int,int);
	STDMETHOD_(long,RemoveObject)(int);
	STDMETHOD_(long,Notify)(unsigned long,void *);
};

interface ILinkSrv : public IScriptServiceBase
{
	STDMETHOD_(class link*,Create)(class link &,class linkkind,object,object);
	STDMETHOD_(long,Destroy)(class link);
	STDMETHOD_(struct true_bool*,AnyExist)(true_bool &,class linkkind,object,object);
	STDMETHOD_(class linkset*,GetAll)(class linkset &,long,object,object);
	STDMETHOD_(class link*,GetOne)(class link &,class linkkind,object,object);
	STDMETHOD_(long,BroadcastOnAllLinks)(const object &,const char *,class linkkind);
	STDMETHOD_(long,BroadcastOnAllLinksData)(const object &,const char *,class linkkind,const cMultiParm &);
	STDMETHOD_(long,CreateMany)(class linkkind,const cScrStr &,const cScrStr &);
	STDMETHOD_(long,DestroyMany)(class linkkind,const cScrStr &,const cScrStr &);
	STDMETHOD_(class linkset*,GetAllInherited)(linkset &,class linkkind,object,object);
	STDMETHOD_(class linkset*,GetAllInheritedSingle)(linkset &,class linkkind,object,object);
};

interface INetworkingSrv : public IScriptServiceBase
{
	STDMETHOD_(long,Broadcast)(const object &,const char*,int,const cMultiParm &);
	STDMETHOD_(long,SendToProxy)(const object &,const object &,const char*,const cMultiParm &);
	STDMETHOD_(long,TakeOver)(const object &);
	STDMETHOD_(long,GiveTo)(const object &,const object &);
	STDMETHOD_(int,IsPlayer)(const object &);
	STDMETHOD_(int,IsMultiplayer)(void);
	STDMETHOD_(int,SetProxyOneShotTimer)(const object &,const char *,float,const cMultiParm &);
	STDMETHOD_(object*,FirstPlayer)(object &);
	STDMETHOD_(long,Suspend)(void);
	STDMETHOD_(long,Resume)(void);
	STDMETHOD_(int,HostedHere)(const object &);
	STDMETHOD_(int,IsProxy)(const object &);
	STDMETHOD_(int,LocalOnly)(const object &);
	STDMETHOD_(int,IsNetworking)(void);
	STDMETHOD_(object*,Owner)(object &,const object &);
};

interface IDarkGameSrv : public IScriptServiceBase
{
	STDMETHOD_(long,KillPlayer)(void);
	STDMETHOD_(long,EndMission)(void);
	STDMETHOD_(long,FadeToBlack)(float);
	STDMETHOD_(long,FoundObject)(int);
	STDMETHOD_(int,ConfigIsDefined)(const char *);
	STDMETHOD_(int,ConfigGetInt)(const char *,int *);
	STDMETHOD_(int,ConfigGetFloat)(const char *,float *);
	STDMETHOD_(float,BindingGetFloat)(const char *);
	STDMETHOD_(int,GetAutomapLocationVisited)(int,int);
	STDMETHOD_(long,SetAutomapLocationVisited)(int,int);
};

interface IPanelMode : public IUnknown
{
public:	
	virtual struct sPanelModeDesc* const __stdcall Describe(void);
	virtual long __stdcall SetDescription(struct sPanelModeDesc* const);
	virtual long __stdcall SetImage(enum  ePanelModeImage,struct IDarkDataSource*);
	virtual struct IDarkDataSource* __stdcall GetImage(enum ePanelModeImage);
	virtual long __stdcall SetParams(struct sLoopModeInitParm*);
	virtual struct sLoopInstantiator const  __stdcall Instantiator(void);
	virtual long __stdcall Switch(int,struct sLoopModeInitParm*);
	virtual long __stdcall Exit(void);
	virtual void __stdcall dtor();
	virtual void __stdcall nullsub();

	DWORD x04, x08;
	char name[32]; // "Panel Mode"
};

interface IPropertyManager : public IUnknown
{
	STDMETHOD_(struct IProperty*,GetProperty)(int);
	STDMETHOD_(struct IProperty*,GetPropertyNamed)(const char *);
	STDMETHOD_(long,Notify)(unsigned long, void *);
	STDMETHOD_(long,BeginIter)(struct sPropertyIter *);
	STDMETHOD_(struct IProperty*,NextIter)(struct sPropertyIter *);
	STDMETHOD_(int,AddProperty)(struct IProperty *,int *);
	STDMETHOD_(int,DelProperty)(struct IProperty *);
};

interface ITagFile : public IUnknown
{
	virtual const VersionNum* __stdcall GetVersion() = 0;
	virtual int __stdcall unk0() = 0;
	virtual long __stdcall OpenBlock(struct _TagFileTag const*, VersionNum*) = 0;
	virtual long __stdcall CloseBlock() = 0;
	virtual struct _TagFileTag const * __stdcall CurrentBlock() = 0;
	virtual unsigned long __stdcall BlockSize(struct _TagFileTag const *) = 0;
	virtual struct ITagFileIter * __stdcall Iterate(void) = 0;
	virtual long __stdcall Seek(unsigned long,enum  TagFileSeekMode) = 0;
	virtual unsigned long __stdcall Tell(void) = 0;
	virtual unsigned long __stdcall TellFromEnd(void) = 0;
	virtual void __stdcall unk1() = 0;
	virtual long __stdcall Write(char const *,int) = 0;
};

class TagFileWrite : public ITagFile
{
public:
	virtual void __stdcall unk1();
	virtual long __stdcall Write(char const *,int);
};

class TagFileRead : public ITagFile
{
public:
	virtual long __stdcall Read(char *,int) = 0;
};

interface IMotionPlan
{
	virtual void unk() = 0;
	virtual class IManeuver* GetPrevManeuver(void) = 0;
	virtual class IManeuver* PopFirstManeuver(void) = 0;
};

interface IMotorResolver
{
	virtual void f0() { };
	virtual void CalcCollisionResponse(const mxs_vector*, int, const mxs_vector*, mxs_vector *) = 0;
	virtual void NotifyAboutBeingStuck() = 0;
	virtual void NotifyAboutMotionAbortRequest() = 0;
};

interface IMotionCoordinator
{
public:
	virtual ~IMotionCoordinator() = 0;

	virtual void Load(struct ITagFile*) = 0;
	virtual void Save(struct ITagFile*) = 0;
	virtual void SetMotor(class IMotor*) = 0;
	virtual void SetPersistentTags(class cTagSet const*) = 0;
	virtual IMotionPlan* BuildPlan(struct sMcMoveState const *,struct sMcMoveParams const *) = 0;
	virtual void Pose(class cTagSet const *,float) = 0;
	virtual enum eMCoordStatus GetStatus(void) = 0;
	virtual void SetNextManeuver(class IManeuver *) = 0;
	virtual void SetCurrentManeuver(class IManeuver *) = 0;
};

interface IResMan : public IUnknown
{
	STDMETHOD_(struct ISearchPath*,NewSearchPath)(const char *) PURE;
	STDMETHOD_(void,SetDefaultPath)(struct ISearchPath *) PURE;
	STDMETHOD_(void,SetGlobalContext)(struct ISearchPath *) PURE;
	STDMETHOD_(void,SetDefaultVariants)(struct ISearchPath *) PURE;
	STDMETHOD_(struct IRes*,Bind)(const char *,const char *,struct ISearchPath *,const char *,unsigned int) PURE;
	STDMETHOD_(void,BindAll)(const char *,const char *,struct ISearchPath *,void (__stdcall *)(struct IRes *,struct IStore *,void *),void *,const char *,unsigned int) PURE;
	STDMETHOD_(struct IRes*,BindSpecific)(const char *,const char *,struct IStore *,struct IStore *,unsigned int) PURE;
	STDMETHOD_(struct IRes *,Retype)(IRes *,const char *,unsigned int) PURE;
	STDMETHOD_(struct IRes*,Lookup)(const char *,const char *,const char *) PURE;
	STDMETHOD_(IStore*,GetStore)(const char *) PURE;
	STDMETHOD_(int,RegisterResType)(struct IResType *) PURE;
	STDMETHOD_(int,RegisterStoreFactory)(struct IStoreFactory *) PURE;
	STDMETHOD_(void,UnregisterResType)(struct IResType *) PURE;
	STDMETHOD_(void,MarkForRefresh)(struct IRes *) PURE;
	STDMETHOD_(void,GlobalRefresh)(void) PURE;
};

interface IRes : public IUnknown
{
	STDMETHOD_(void*, Lock)(void) PURE;
	STDMETHOD_(void, Unlock)(void) PURE;
	STDMETHOD_(uint, GetLockCount)(void) PURE;
	STDMETHOD_(interface IResType*, GetType)(void) PURE;
	STDMETHOD_(const char*,GetName)(void) PURE;
	STDMETHOD_(void,GetCanonPath)(char**) PURE;
	STDMETHOD_(void,GetCanonPathName)(char**) PURE;
	STDMETHOD_(interface IStore*,GetCanonStore)(void) PURE;
	STDMETHOD_(void,GetVariant)(char**) PURE;
	STDMETHOD_(interface IStore*,GetStore)(void) PURE;
	STDMETHOD_(void,GetStreamName)(int,char**) PURE;
	STDMETHOD_(void*,DataPeek)(void) PURE;
	STDMETHOD_(int,Drop)(void) PURE;
	STDMETHOD_(long,GetSize)(void) PURE;
	STDMETHOD_(void*,Extract)(void*) PURE;
	STDMETHOD_(void*,PreLoad)(void) PURE;
	STDMETHOD_(int,ExtractPartial)(long,long,void*) PURE;
	STDMETHOD_(void,ExtractBlocks)(void *,long,long (*)(IRes *,void *,long,long,void *),void *) PURE;
	STDMETHOD_(int,AsyncLock)(int) PURE;
	STDMETHOD_(int,AsyncExtract)(int,void *,long) PURE;
	STDMETHOD_(int,AsyncPreload)(void) PURE;
	STDMETHOD_(int,IsAsyncFulfilled)(void) PURE;
	STDMETHOD_(long,AsyncKill)(void) PURE;
	STDMETHOD_(long,GetAsyncResult)(void * *) PURE;
	STDMETHOD_(void,SetAppData)(ulong) PURE;
	STDMETHOD_(ulong,GetAppData)(void) PURE;
};

interface IDataSource : public IRes
{

};

interface IDarkDataSource : public IDataSource
{
};

class cStringResource : public IRes
{
public:
	STDMETHOD_(void,StringPreload)(const char*) PURE;
	STDMETHOD_(char*,StringLock)(const char*) PURE;
	STDMETHOD_(void,StringUnlock)(const char*) PURE;
	STDMETHOD_(int,StringExtract)(const char*, char*, int) PURE;
};

class cAIActor;
interface IAI;
interface IAIMoveAction;
interface IAILocoAction;
interface IAISoundAction;
interface IAIFrobAction;
interface IAIFollowAction;
interface IAIOrientAction;
interface IAIFrobAction;
interface IAIFollowAction;
interface IAIInvestAction;
interface IAIPsdScrAction;
interface IAIMotionAction;
interface IAIWanderAction;
interface IAIJointRotateAction;
interface IAIJointSlideAction;
interface IAIJointScanAction;
interface IAIWaitAction;
interface IAILaunchAction;

interface IAIAction : public IUnknown
{
	STDMETHOD_(void,Describe)(cAnsiStr*);
	STDMETHOD_(int,InProgress)(void);
	// ...

	DWORD x04;
	uint m_actionID; // 0x8
};

interface IAIBehaviorSet : public IUnknown
{
	STDMETHOD_(const char*,GetName)(void) PURE;
	STDMETHOD_(IAI*,CreateAI)(int) PURE;
	STDMETHOD_(long,CreateComponents)(void*) PURE;
	STDMETHOD_(IAIMoveAction*,CreateMoveAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAILocoAction*,CreateLocoAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIMotionAction*,CreateMotionAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAISoundAction*,CreateSoundAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIOrientAction*,CreateOrientAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIFrobAction*,CreateFrobAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIFollowAction*,CreateFollowAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIInvestAction*,CreateInvestAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIWanderAction*,CreateWanderAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIPsdScrAction*,CreatePsdScrAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIWaitAction*,CreateWaitAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAILaunchAction*,CreateLaunchAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIJointRotateAction*,CreateJointRotateAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIJointSlideAction*,CreateJointSlideAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIJointScanAction*,CreateJointScanAction)(cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIAction*,CreateGestureAction)(unsigned int,cAIActor *,unsigned long) PURE;
	STDMETHOD_(IAIAction*,CreateCustomAction)(unsigned int,cAIActor *,unsigned long) PURE;
	STDMETHOD_(void,EnactProxyLaunchAction)(IAI *,void *) PURE;
	STDMETHOD_(void,EnactProxyJointRotateAction)(IAI *,void *) PURE;
	STDMETHOD_(void,EnactProxyJointSlideAction)(IAI *,void *) PURE;
	STDMETHOD_(void,EnactProxyJointScanAction)(IAI *,void *) PURE;
	STDMETHOD_(void,EnactProxyCustomAction)(unsigned int,IAI *,void *) PURE;
	STDMETHOD_(void,SaveAction)(ITagFile *,IAIAction *) PURE;
	STDMETHOD_(IAIAction*,LoadAndCreateAction)(ITagFile *,cAIActor *) PURE;
};

interface IAINetManager : public IUnknown
{
	STDMETHOD_(int,IsProxy)(int) PURE;
	STDMETHOD_(long,GetAITransferInfo)(int,sAINetTransfer * *) PURE;
	STDMETHOD_(long,MakeProxyAI)(int,int) PURE;
	STDMETHOD_(long,MakeFullAI)(int,const sAINetTransfer *) PURE;
	STDMETHOD_(long, nullsub0)(void) PURE;
	STDMETHOD_(unsigned char,NetMsgHandlerID(void)) PURE;
};

interface IAIManager : public IUnknown
{
	STDMETHOD_(long,InstallBehaviorSet)(struct IAIBehaviorSet *);
	STDMETHOD_(interface IAI*,GetAI)(int);
	STDMETHOD_(int,CreateAI)(int,const char *);
	STDMETHOD_(int,DestroyAI)(int);
	STDMETHOD_(int,SelfSlayAI)(int);
	virtual void unknown();
	STDMETHOD_(struct tAIIter__ * *,GetFirst)(struct tAIIter*);
	STDMETHOD_(struct tAIIter__ * *,GetNext)(struct tAIIter*);
	virtual void nullsub();
	STDMETHOD_(long,SetMotor)(int,class IMotor *);
	STDMETHOD_(long,ResetMotionTags)(int);
	STDMETHOD_(long,NotifyFastObj)(int,int,const mxs_vector &);
	STDMETHOD_(long,NotifySound)(struct sSoundInfo const *);
	STDMETHOD_(void,SetIgnoreSoundTime)(unsigned long);
	STDMETHOD_(int,StartConversation)(int);
	STDMETHOD_(struct IAIConversationManager*,GetConversationManager)(void);
	STDMETHOD_(void,ListenToProperty)(struct IProperty *);
	STDMETHOD_(void,ListenForCollisions)(int);
	STDMETHOD_(void,UnlistenForCollissions)(int);
	virtual void dtor();
	STDMETHOD_(void,OnNormalFrame)(void);
	STDMETHOD_(void,DatabaseReset)(void);
	// ...

	DWORD x04;
	IAINetManager* m_pNetManager;
	char data[0x50];
	IAIConversationManager* m_ConvManager;
};

interface IAINetServices : public IUnknown
{
	STDMETHOD_(int,Networking)(void);
	STDMETHOD_(int,GetPlayers)(int (*)[8]);
	STDMETHOD_(void,SetObjImpulse)(int,float,float,float,float,int);
	STDMETHOD_(int,TransferAI)(int, int);
	STDMETHOD_(int,GetTargetVel)(int,struct sAIImpulse *);
};

interface IAIMoveEnactor
{
	DWORD vtable;

	DWORD x04, x08, x0c;
	sAIMoveEnactorx10* x10;
};

#define AI_STATE_BRAINSONLY 0x8

interface IAI : public IUnknown
{
	STDMETHOD_(int,GetID)(void);
	STDMETHOD_(void,NotifyGameEvent)(void *);
	STDMETHOD_(void,NotifyTripwire)(int,int);
	STDMETHOD_(void,Kill)(void);
	STDMETHOD_(void,Resurrect)(void);
	STDMETHOD_(int,IsDead)(void);
	STDMETHOD_(int,IsDying)(void);
	STDMETHOD_(void,Freeze)(unsigned long);
	STDMETHOD_(void,Unfreeze)(void);
	STDMETHOD_(struct IInternalAI*,AccessOtherAI)(int);
	STDMETHOD_(void,AccessAIs)(struct IInternalAI * * *,int *);
	STDMETHOD_(long,Init)(int,IAIBehaviorSet*);
	STDMETHOD_(long,End)(void);
	STDMETHOD_(long,GetComponent)(const char *,struct IAIComponent * *);
	STDMETHOD_(IAIComponent*,GetFirstComponent)(tAIIter *);
	STDMETHOD_(IAIComponent*,GetNextComponent)(tAIIter *);
	STDMETHOD_(void,GetComponentDone)(tAIIter *);
	STDMETHOD_(struct IAIMoveEnactor*,AccessMoveEnactor)(void);
	STDMETHOD_(struct IAISoundEnactor*,AccessSoundEnactor)(void);
	STDMETHOD_(struct IAISenses*,AccessSenses)(void);
	STDMETHOD_(struct IAIPathfinder*,AccessPathfinder)(void);
	STDMETHOD_(struct IAIBehaviorSet*,AccessBehaviorSet)(void);
	STDMETHOD_(void*,Describe)(void);
	STDMETHOD_(unsigned int,UpdatePathCell)(void);
	STDMETHOD_(void,GetRecentCells)(class cDynArray<uint>*);
	STDMETHOD_(float,GetGroundOffset)(void);
	STDMETHOD_(int,GetClosestPlayer)(void);
	STDMETHOD_(const struct sAIAwareness *,GetAwareness)(int);
	STDMETHOD_(void,NotifySimStart)(void);
	STDMETHOD_(void,NotifySimEnd)(void);
	STDMETHOD_(unsigned long,NotifyDamage)(const sDamageMsg *);
	STDMETHOD_(void,NotifyWeapon)(int,int,int);
	STDMETHOD_(void,NotifyFastObj)(int,int,const mxs_vector &);
	STDMETHOD_(void,NotifySound)(const struct sSoundInfo *,const struct sAISoundType *);
	STDMETHOD_(void,NotifyFoundBody)(int);
	STDMETHOD_(void,NotifyFoundSuspicious)(int);
	STDMETHOD_(void,NotifyScriptCommand)(unsigned int,void *);
	STDMETHOD_(void,NotifyProperty)(IProperty *,const struct sPropertyListenMsg *);
	STDMETHOD_(void,NotifyDefend)(const struct sAIDefendPoint *);
	STDMETHOD_(void,NotifySignal)(const struct sAISignal *);
	STDMETHOD_(void,NotifyWitnessCombat)(void);
	STDMETHOD_(void,NotifyCollision)(const struct sPhysListenMsg *);
	STDMETHOD_(long,f0)(void);
	STDMETHOD_(void,StartProxyAction)(struct IAIAction *,unsigned long);
	STDMETHOD_(IAIAction *,GetTheActionOfType)(unsigned int);
	STDMETHOD_(void,StopActionOfType)(unsigned int);
	STDMETHOD_(void,SetGoal)(class cAIGoal *);
	STDMETHOD_(void,SetModeAndNotify)(enum eAIMode);
	STDMETHOD_(void,SetIsBrainsOnly)(int);
	STDMETHOD_(int,PreSave)(void);
	STDMETHOD_(int,Save)(struct ITagFile *);
	STDMETHOD_(int,PostSave)(void);
	STDMETHOD_(int,PreLoad)(void);
	STDMETHOD_(int,Load)(struct ITagFile *);
	STDMETHOD_(int,PostLoad)(void);
	STDMETHOD_(void,DebugDraw)(void);
	STDMETHOD_(void,DebugSetPrimitiveMove)(const class cAIMoveSuggestions *,const struct sAIMoveGoal *);
	STDMETHOD_(void,DebugGetPrimitiveMove)(const class cAIMoveSuggestions * *,const struct sAIMoveGoal * *);

	DWORD x04;
	int m_objID; // 0x8
	eAIMode m_mode; // 0xC
	mxs_vector loc; // 0x10 - 0x18
	DWORD x1c, x20, x24, x28, x2c, x30, x34, x38, x3c, x40, x44, x48;
	DWORD m_stateFlags; // 0x4C
	DWORD x50, x54, x58, x5c, x60, x64, x68;
	DWORD x6c, x70;
	uint m_lastRenderedTime; // 0x74
	DWORD x78, x7c, x80, x84, x88, x8c;
	interface IAISenses* m_senses; // 90
	DWORD x94, x98, x9c, xa0;
	DWORD xa4; // (related to current goal)
	IAIMoveEnactor* m_moveEnactor; // A8
	IAISoundEnactor* m_soundEnactor; // AC
	interface IAIPathFinder* m_pathFinder; // B0
	DWORD xb4, xb8, xbc, xc0;
	cDynArray<IAIAction*> m_Actions; // 0xC4 - 0xC8
	//IAIBehaviorSet* m_behaviorSet; // 120
};

typedef unsigned long (__stdcall* DamageFilter)(int,int,sDamage*,void*);
typedef unsigned long (__stdcall* DamageListener)(const sDamageMsg*, void*);

struct DamageFilterEntry
{
	DamageFilter filter;
	void* pData;
};

struct DamageListenerEntry
{
	ulong x00;
	DamageListener listener;
	void* pData;
};

interface IDamageModel : public IUnknown
{
	STDMETHOD_(unsigned long,HandleImpact)(int,int,struct sImpact*,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,DamageObject)(int,int,sDamage*,sChainedEvent*,int) PURE;
	STDMETHOD_(unsigned long,SlayObject)(int,int,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,TerminateObject)(int,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,ResurrectObject)(int, int, sChainedEvent*) PURE;
	STDMETHOD_(long,Filter)(DamageFilter, void*) PURE;
	STDMETHOD_(long,Listen)(unsigned long, DamageListener, void*) PURE;
	STDMETHOD_(long,Init)(void) PURE;
	STDMETHOD_(long,End)(void) PURE;

	//BYTE unk[0x24];
	//IIntProperty* deathStageProp; // 0x20
};

class cBaseDamageModel : public IDamageModel
{
public:
	STDMETHOD_(unsigned long,HandleImpact)(int,int,struct sImpact*,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,DamageObject)(int,int,sDamage*,sChainedEvent*,int) PURE;
	STDMETHOD_(unsigned long,SlayObject)(int,int,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,TerminateObject)(int,sChainedEvent*) PURE;
	STDMETHOD_(unsigned long,ResurrectObject)(int, int, sChainedEvent*) PURE;
	STDMETHOD_(long,Filter)(DamageFilter, void*) PURE;
	STDMETHOD_(long,Listen)(unsigned long, DamageListener, void*) PURE;
	STDMETHOD_(long,Init)(void) PURE;
	STDMETHOD_(long,End)(void) PURE;

	unsigned long ApplyFilters(int, int, sDamage*);
	unsigned long ApplyResult(int victim, int culprit, unsigned long result, sChainedEvent* pEvent);
	unsigned long cBaseDamageModel::SendMessage(const sDamageMsg* pDmgMsg);
	int AlreadySentMsg(unsigned long P1, int object);

protected:
	DWORD x04, x08, x0c, x10;
	IObjectSystem* m_pObjSys; // 0x14
	cDynArray<DamageListenerEntry> m_DamageListeners;
	cDynArray<DamageFilterEntry> m_DamageFilters;
	IIntProperty* m_pDeathStageProp; // 0x28
};

// Size = 0x40
class cSimpleDamageModel : public cBaseDamageModel
{
public:
	STDMETHOD_(unsigned long,HandleImpact)(int,int,struct sImpact*,sChainedEvent*);
	STDMETHOD_(unsigned long,DamageObject)(int,int,sDamage*,sChainedEvent*,int);
	STDMETHOD_(unsigned long,SlayObject)(int,int,sChainedEvent*);
	STDMETHOD_(unsigned long,TerminateObject)(int,sChainedEvent*);
	STDMETHOD_(unsigned long,ResurrectObject)(int, int, sChainedEvent*);
	STDMETHOD_(long,Filter)(DamageFilter, void*);
	STDMETHOD_(long,Listen)(unsigned long, DamageListener, void*);
	STDMETHOD_(long,Init)(void);
	STDMETHOD_(long,End)(void);
};

interface IStructDescTools : public IUnknown
{
	 STDMETHOD_(const sFieldDesc*,GetFieldNamed)(const sStructDesc*,const char*) PURE;
	 STDMETHOD_(long,ParseField)(const sFieldDesc*,const char*,void*) PURE;
	 STDMETHOD_(long,UnparseField)(const sFieldDesc*,const void*,char*,int) PURE;
	 STDMETHOD_(int,IsSimple)(const sStructDesc*) PURE;
	 STDMETHOD_(int,ParseSimple)(const sStructDesc*,const char*,void*) PURE;
	 STDMETHOD_(int,UnparseSimple)(const sStructDesc*,const void*,char*,int) PURE;
	 STDMETHOD_(int,ParseFull)(const sStructDesc*,const char*,void*) PURE;
	 STDMETHOD_(int,UnparseFull)(const sStructDesc*,const void*,char*,int) PURE;
	 STDMETHOD_(long,Dump)(const sStructDesc*,const void * pStruct) PURE;
	 STDMETHOD_(long,SetIntegral)(const sFieldDesc*,long,void*) PURE;
	 STDMETHOD_(long,GetIntegral)(const sFieldDesc*,const void*,long*) PURE;
	 STDMETHOD_(const sStructDesc*,Lookup)(const char*) PURE;
	 STDMETHOD_(long,Register)(const sStructDesc*) PURE;
	 STDMETHOD_(long,ClearRegistry)(void) PURE;
};

typedef int (__cdecl *ScrSaveLoadFunc)(void*,void*,unsigned int);

interface IScriptMan : public IUnknown
{
	STDMETHOD_(long,GameInit)(void) PURE;
	STDMETHOD_(long,GameEnd)(void) PURE;
	STDMETHOD_(long,BeginScripts)(void) PURE;
	STDMETHOD_(long,EndScripts)(void) PURE;
	STDMETHOD_(long,SetModuleDatapath)(const struct Datapath *) PURE;
	STDMETHOD_(long,AddModule)(const char *) PURE;
	STDMETHOD_(long,RemoveModule)(const char *) PURE;
	STDMETHOD_(long,ClearModules)(void) PURE;
	STDMETHOD_(long,ExposeService)(IUnknown *,const _GUID &) PURE;
	STDMETHOD_(IUnknown *,GetService)(const _GUID &) PURE;
	STDMETHOD_(const class sScrClassDesc*,GetFirstClass)(tScrIter*) PURE;
	STDMETHOD_(const class sScrClassDesc*,GetNextClass)(tScrIter*) PURE;
	STDMETHOD_(void,EndClassIter)(tScrIter*) PURE;
	STDMETHOD_(const sScrClassDesc*,GetClass)(const char *) PURE;
	STDMETHOD_(long,SetObjScripts)(int,const char * *,unsigned int) PURE;
	STDMETHOD_(long,ForgetObj)(int) PURE;
	STDMETHOD_(long,ForgetAllObjs)(void) PURE;
	STDMETHOD_(int,WantsMessage)(int,const char *) PURE;
	STDMETHOD_(long,SendMessageA)(sScrMsg *,sMultiParm *) PURE;
	STDMETHOD_(void,KillTimedMessage)(tScrTimer) PURE;
	STDMETHOD_(int,PumpMessages)(void) PURE;
	STDMETHOD_(void,PostMessage)(sScrMsg *) PURE;
	STDMETHOD_(tScrTimer*,SetTimedMessage)(sScrMsg *,unsigned long,enum eScrTimedMsgKind) PURE;
	STDMETHOD_(cMultiParm*,SendMessage2)(cMultiParm &,int,int,const char *,const cMultiParm &,const cMultiParm &,const cMultiParm &) PURE;
	STDMETHOD_(void,PostMessage2)(int,int,const char *,const cMultiParm &,const cMultiParm &,const cMultiParm &) PURE;
	STDMETHOD_(int,SetTimedMessage2)(int,const char *,unsigned long,enum eScrTimedMsgKind,const cMultiParm &) PURE;
	STDMETHOD_(int,IsScriptDataSet)(const struct sScrDatumTag *) PURE;
	STDMETHOD_(long,GetScriptData)(const struct sScrDatumTag *,sMultiParm *) PURE;
	STDMETHOD_(long,SetScriptData)(const struct sScrDatumTag *,const sMultiParm *) PURE;
	STDMETHOD_(long,ClearScriptData)(const struct sScrDatumTag *,sMultiParm *) PURE;
	STDMETHOD_(long,AddTrace)(int,char *,enum eScrTraceAction,int) PURE;
	STDMETHOD_(long,RemoveTrace)(int,char *) PURE;
	STDMETHOD_(int,GetTraceLine)(int) PURE;
	STDMETHOD_(void,SetTraceLine)(int,int) PURE;
	STDMETHOD_(int,GetTraceLineMask)(void) PURE;
	STDMETHOD_(void,SetTraceLineMask)(int) PURE;
	STDMETHOD_(const class cScrTrace*,GetFirstTrace)(tScrIter*) PURE;
	STDMETHOD_(const class cScrTrace*,GetNextTrace)(tScrIter*) PURE;
	STDMETHOD_(void,EndTraceIter)(tScrIter*) PURE;
	STDMETHOD_(long,SaveLoad)(ScrSaveLoadFunc,void *,int) PURE;
	STDMETHOD_(void,PostLoad)(void) PURE;
};

interface ISoundScrSrv : public IScriptServiceBase
{
	STDMETHOD_(true_bool,PlayAtLocation)(object,const cScrStr &,const cScrVec &,enum eSoundSpecial,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlayAtObject)(object,const cScrStr &,object,enum eSoundSpecial ,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,Play)(object,const cScrStr &,enum eSoundSpecial,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlayAmbient)(object,const cScrStr &,enum eSoundSpecial, enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlaySchemaAtLocation)(object,object,const cScrVec &,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlaySchemaAtObject)(object,object,object,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlaySchema)(object,object,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlaySchemaAmbient)(object,object,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlayEnvSchema)(object,const cScrStr &,object,object,enum eEnvSoundLoc,enum eSoundNetwork) PURE;
	STDMETHOD_(true_bool,PlayVoiceOver)(object,object) PURE;
	STDMETHOD_(int,Halt)(object,const cScrStr &,object) PURE;
	STDMETHOD_(true_bool,HaltSchema)(object,const cScrStr &,object) PURE;
	STDMETHOD_(long,HaltSpeech)(object) PURE;
	STDMETHOD_(true_bool,PreLoad)(const cScrStr &) PURE;
};

interface IBowSrv : public IScriptServiceBase
{
	STDMETHOD_(long,Equip)(void) PURE;
	STDMETHOD_(long,UnEquip)(void) PURE;
	STDMETHOD_(int,IsEquipped)(void) PURE;
	STDMETHOD_(long,StartAttack)(void) PURE;
	STDMETHOD_(long,FinishAttack)(void) PURE;
	STDMETHOD_(long,AbortAttack)(void) PURE;
	STDMETHOD_(int,SetArrow)(object) PURE;
};

interface IGameStrings : public IUnknown
{
	STDMETHOD_(cAnsiStr,FetchString)(const char*,const char*,const char*,const char*) PURE;
	STDMETHOD_(long,RegisterProp)(const char*,IStringProperty*) PURE;
	STDMETHOD_(cAnsiStr,FetchObjString)(int, const char*) PURE;
	STDMETHOD_(long,DumpProp)(const char*) PURE;
	STDMETHOD_(long,LoadProp)(const char*) PURE;
	STDMETHOD_(long,StripProp)(const char*) PURE;
	STDMETHOD_(long,ModernizeProp)(const char*) PURE;
};

interface IMotionSet
{
	virtual ~IMotionSet();
	virtual int NeckIsFixed(int) PURE;
	virtual int BlendLength(int) PURE;
	virtual Label* GetName(int) PURE;
	// ...
};

#pragma pack(pop)