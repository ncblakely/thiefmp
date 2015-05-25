#pragma once

#pragma pack(push, 1)

#include "BaseTypes.h"
#include "Defines_LG.h" // 12/19/10
#include "Damage.h"
#include "DarkStim.h"
#include "DynArray.h"
#include "StructDesc.h"

class IUnknownImpl : public IUnknown
{
	STDMETHOD_(HRESULT,QueryInterface)(REFIID riid, void**ppvObject);
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();

protected:
	int m_refCount;
};

// ----------------------------------------------
// Packet definitions
// Names for these structures are unknown

struct PacketHeader
{
	BYTE msgHandlerID; // ID returned by a call to NetMsgHandlerID
	BYTE x02; // unknown, appears to be a byte in all messages so is likely part of header data
};

struct LoginResponse : public PacketHeader
{
	// x02 == 0
	BYTE playerNum; // is this really the player number? appears to be NetManager->m_numPlayers on the host's end?
};

struct sNetMsg_CreatePlayer : public PacketHeader
{
	// x02 == 1
	short avatarArchetype; // 0x2
	short localPlayerObj; // 0x4
	BOOL bSessionHost;
	mxs_vector position;
	short bank; // position-> 0x14
	BYTE playerNum; // cNetManager + 1E1
};

struct sAILaunchAction
{
	DWORD handlerData; 
	BYTE unk[0x20];
	short projArch;
};

// ----------------------------------------------

struct _IB_var
{
	char name[32];
	DWORD x32;
	BYTE unk_x36[64];
	void* cmdFunc;
	BYTE unk_2[39];
};

class cAvatar : public IUnknownImpl
{
public:
	cAvatar(int objId, unsigned long playerNum);

	STDMETHOD_(unsigned long, PlayerNum)();
	STDMETHOD_(int, PlayerObjID)();
	STDMETHOD_(void, WriteTagInfo)(interface ITagFile* pTag);

	unsigned long m_playerNum; // cNetManager + 1E1 - 0x8
	int m_playerObjId; // 0xC
};

/////////////


struct PackedDword
{
	BYTE b1: 8;
	BYTE b2: 8;
	BYTE b3: 8;
	BYTE b4: 8;
};

struct sfx_parm
{
	DWORD dw1;			// 0x0
	DWORD flag;		// 0x4
	DWORD dw2;
	short	initialVol;	// 0xC
	short	s4;	// 0x10
	int   initialDelay;	// 0x14
	int	fade;	// 0x18
	DWORD dw7;	// 0x1C
	DWORD dw8;	// 0x20
	DWORD dw9;	// 0x24
	int	schema;	// 0x28
	DWORD dw11; // 0x2C
	DWORD dw12; // 0x30
	DWORD dw13; // 0x34
};

struct ImageResource
{
	BYTE unk[0x5];
	BYTE palette; // 0x5
	short x06;
	short width; // 0x8
	short height; // 0xA
};

enum eMultiParmType
{
	kMT_Undef,
	kMT_Int,
	kMT_Float,
	kMT_String,
	kMT_Vector,
	kMT_Boolean
};

struct sMultiParm
{
	sMultiParm() { _type = kMT_Undef; }
	sMultiParm(char* s) { _type = kMT_String; _str = s; }
	sMultiParm(int i) { _type = kMT_Int; _int = i; }
	sMultiParm(float f) { _type = kMT_Float; _flt = f; }
	sMultiParm(mxs_vector* v) { _type = kMT_Vector; _vec = v; }

	union 
	{
		char* _str;
		int _int; // doubles as BOOL
		float _flt;
		mxs_vector* _vec;
	};

	eMultiParmType _type;
};

class cMultiParm : public sMultiParm
{
public:
	~cMultiParm() { }
};

class cTag
{
	BYTE unk[0x8];
};

class cTagSet
{
	DWORD P1;
	int numTags;
};

class cTagDBInput
{
	cTagSet tagSet;
};

struct WeaponAttachment
{
	int attachJoint;
	DWORD P2;
	mxs_trans trans;
	DWORD P3;
};

typedef void (*_SpeechEndCallback)(int, int, void*);

// These were reconstructed from struct desc, data types may be incorrect
struct sSchemaPlayParams
{
	DWORD flags;
	int volume;
	int initialDelay;
	int pan;
	int fade;
};

struct sSchemaLoopParams
{
	DWORD flags;
	int maxSamples;
	int intervalMin;
	int intervalMax;
};

// Size = 0x1100 - confirmed from dromed.00585FDB
struct _LGadTextBox
{
	BYTE unk[0x96];
	char* text; // 96
	DWORD x9a;
	int cursorPos; // 9E 
	BYTE unk2[0x105E];
};

struct MsgDescription
{
	DWORD P1;
	char msgDesc[16];
};

struct sNetMsg_Generic
{
	DWORD P1;
	DWORD P2;
	MsgDescription* desc;
};

struct sFrobActivate
{
	int msdown; // 0
	int objectID;	// 4
	DWORD P2;		// 8
	int frobType;		// C
	DWORD P4;		// 10
	int frobberID; // 14
	DWORD P5;		// 18
};

struct sFrobInfo
{
	DWORD worldActionFlags;
	DWORD invActionFlags;
	DWORD toolActionFlags;
};

struct grs_bitmap
{
};

struct GUIsetupStruct
{
	BYTE unk[0x68];
};

// Size = 0x50
struct guiStyle
{
	DWORD x00;
	struct Colors
	{
		int colorFg; // 0x4
		int colorBg; // 0x8
		int colorText; // 0xc
		int colorHilite; // 0x10
		int colorBright; // 0x14
		int colorDim; // 0x18
		int colorFg2; // 0x1c
		int colorBg2; // 0x20
		int colorBorder; // 0x24
		int colorWhite; // 0x28
		int colorBlack; // 0x2c
		int colorXor; // 0x30
		int colorBevelLight; // 0x34
		int colorBevelDark; // 0x38
	} colors;

	interface IRes* fontNormal;
	interface IRes* fontTitle;

	// These are initialized in uiGameLoadStyle. No idea what they're for
	int zero;
	int one;
	int two;
};

// Returned by a Lock() on a cFontResourceType
struct grs_font
{
};

class cFontResourceType
{
};

// Size = 0x54
struct grd_canvas
{
	DWORD x00, x04; 
	short width; // 0x8
	short height; // 0xA
	DWORD x0c;
	int	color; // 0x10
	DWORD P5; // 0x14
	grs_font* fontResource; // 0x18
	DWORD x1c, x20, x24, x28, x2c, x30, x34, x38, x3c, x40, x44, x48, x4c, x50;
};

struct sScreenMode
{
	DWORD validParameters; // 0x0
	int width; // 0x4
	int height;  // 0x8
	int bitDepth; // 0xC
	DWORD flags; // 0x10
};

struct grd_screen
{
	DWORD P1;	// 0
	DWORD P2;	// 4
	short width;
	short height;
	DWORD P5;	// 10
	DWORD P6;	// 14
	DWORD P7;	// 18
	DWORD P8;	// 1C
	grd_canvas* grd_canvas;
};

struct sPickState;
struct sPickCfg;

struct sContainIter
{
	BOOL bDone;
	uint P2;
	uint P3;
	int object;
};

struct sLink
{
	int sourceObj; // 0x0
	int destObj; // 0x4
	short flavorID; // 0x8
};

struct sMissionData
{
	int missNumber;
	char pathname[9];
};

class cFastMutex
{
	DWORD dwThreadId;

};

struct grs_mode_info
{
	DWORD flags;
	short modeWidth;
	short modeHeight;
	unsigned char modeDepth;
	unsigned char P1;
};

struct sLoot 
{
	int gold;
	int gems;
	int art;
	int special;
};

struct sPropertyListenMsg
{
	int	eventID;
	DWORD flags;
	int	objectID;
	int	value;
	// incomplete
};

// Used in DirectPlay4's GetPlayerData and SetPlayerData calls (real struct name unknown)
struct DP4Data
{
	DWORD P1;
	int playerObject;
};

// Size = 0xC
struct sPlayerInfoEntry
{
	char* name;
	char* address;
	int object;
};

class cPlayerMsgBundle
{
	BYTE unk[0x1c];
};

struct sGlobalObjID
{
	short objIDhostNum;
	char unk;
};

struct Command
{
	char* command;
	eArgType argType;
	void*	func;
	char* commandHelp;
	DWORD state;	// ???
};

struct _DrawElement
{
	BYTE unk[0x1C];
	char* elementText;
};

// Size = 0x24
struct sDarkPanelDesc
{
	const char* panelName; // 0x0 / 0x10
	int num_buttons; // 0x4 / 0x14
	int num_rects; // 0x8 // 0x18
	int num_button_strings; // 0xc // 0x1c
	const char** uiStrings; // 0x10 // 0x20
	DWORD x14, x18, x1c, x20; // 0x24 - 30
};

struct tSpringInfo
{
	DWORD unk;
	int arraySize;
	BYTE unk2[0x18];
	int someIndex;
};

struct SpeedScale
{
	char scaleName[32];
	float transScale; // 0x20
	float rotScale; // 0x24
};

class cPlayerMode
{
public:
	int m_jumpState;				// 0x0		0 - not jumping : 1 - jumping
	int m_speedToggleOn;		// 0x4
	int m_toggleMeansFaster; // 0x8
	int m_leaningState;				// 0xC		 0 - lean off : 1 - lean on
	int m_slideOn;						// 0x10
	float m_forwardSpeed;	// 0x14
	float m_sidestepSpeed;	// 0x18
	float m_rotateSpeed;		// 0x1C
	DWORD unk;	// 0x20
	int m_modeID;		// 0x24 - ePlayerMode
	cDynArray<SpeedScale> m_SpeedScales; // 0x28 - 0x2c
	DWORD x30;
	DWORD x34;
	int tmFadeStart; // 0x38
	float fadeTime; // 0x3C
	BYTE fadeR; // 0x40
	BYTE fadeG; // 0x41
	BYTE fadeB; // 0x42

	float GetTransSpeedScale();
};

struct sAIVisibility
{
	int level;
	int lightRating;
	int movementRating;
	int exposureRating;
	int lastUpdate;
};

struct sAIFrustrated
{
	int frustratedAt;
	int aiObj;
};

struct PlayerCamera
{
	int mode;
	float zoomLevel;
	mxs_vector location;
	DWORD x14, x18, x1c;
	int attachedObj; // 0x20 - the object the camera is attached to
};

class cScrStr
{
public:
	char* m_string;
};

class cScrVec : public mxs_vector
{
};

struct sScrDatumTag
{
	int object;
	const char* scriptName;
	const char* dataName;
};

struct sPropertyObjIter
{
	DWORD x00, x04, x08, x0c, x10;
};

// sScrMsg flags
#define SSCRMSG_ISBROADCAST 0x4
#define SSCRMSG_ISPOSTMESSAGE 0x8 // set in cLinkSrv::BroadcastOnAllLinks immediately before calling PostMessage - will be posted to the owner of the object if it's hosted remotely
// Size = 0x38
struct sScrMsg
{
	sScrMsg() { ZeroMemory(this, sizeof(sScrMsg)); }

	DWORD sScrMsgvtbl; // 0x0
	int refCount; // 0x4
	DWORD sPersistentvtbl; // 0x8
	int source; // 0xC
	int dest; // 0x10
	const char* msgName; // 0x14
	int time; // 0x18
	DWORD flags; // 0x1C
	cMultiParm parm1, parm2, parm3; // 0x20 - 0x34
};

struct sSlayMsg
{
	sSlayMsg() { ZeroMemory(this, sizeof(sSlayMsg)); }

	int culprit; // 0x38
};

struct sFrobMsg : public sScrMsg
{
	sFrobMsg() { ZeroMemory(this, sizeof(sFrobMsg)); }

	int frobSrc;
	int frobDest;
	int frobber; // 0x40
	DWORD x44, x48;
};

struct sAIAlertnessMsg : public sScrMsg
{
	sAIAlertnessMsg() { ZeroMemory(this, sizeof(sAIAlertnessMsg)); }

	eAIAlertLevel currentLevel;
	eAIAlertLevel prevLevel;
};

struct sSimMsg : public sScrMsg
{
	sSimMsg() { ZeroMemory(this, sizeof(sSimMsg)); }

	BOOL bEntering; // 0x38
};

struct sMovingTerrainMsg : public sScrMsg
{
	sMovingTerrainMsg() { ZeroMemory(this, sizeof(sMovingTerrainMsg)); }

	BOOL bActive; // 0x38
};

struct sContainedMsg : public sScrMsg
{
	sContainedMsg() { ZeroMemory(this, sizeof(sContainedMsg)); }

	int event; // 0x38
	int containedBy; // 0x3C
};

struct sPhysMsg: public sScrMsg
{
	sPhysMsg() { ZeroMemory(this, sizeof(sPhysMsg)); }

	DWORD x38, x3c;
	int collideWith; // 0x40
};

struct sPhysListenMsg
{

};

struct IPhysAttrProperty
{
	float gravity;
	float mass;
	float density;
	float elasticity;
};

class cAnsiStr
{
public:
	cAnsiStr() { m_pString = NULL; m_dataLength = NULL; m_allocLength = NULL; }
	//~cAnsiStr();

	//operator bool () { return m_pString[0] != ChNil; }
	operator const char* () const 
	{ 
		// 7/31/10 - game's cAnsiStrs do not return NULL if empty

		//if (m_pString[0] == ChNil) 
		//	return NULL; 
		//else 
			return m_pString;
	}

	cAnsiStr& operator+= (const char* rhs)
	{
		Append((int)strlen(rhs), rhs);
		return *this;
	}

	cAnsiStr& operator+= (const cAnsiStr& rhs)
	{
		Append(rhs.m_dataLength, rhs.m_pString);
		return *this;
	}

	void Append(int length, const char* str);
	void Assign(int dataLength, const char* str);
	void FreeStr(char* str);
	int Length() { return m_dataLength; }
	char* ReallocStr(char *, int);

	void Destroy() 
	{ 
		if (m_pString && m_pString[0] != ChNil)
		{
			FreeStr(m_pString); 
			m_pString = NULL;
			m_dataLength = NULL;
			m_allocLength = NULL;
		}
	}

	char* m_pString;
	int m_dataLength, m_allocLength;

	static char ChNil;
};

class cHashSet
{
	DWORD x00;
};

struct tHashSetHandle // 0x24 bytes or 8?
{
	DWORD x00;
	DWORD x04;
	DWORD x08;
};	

struct tHashSetKey__
{
};

struct tHashSetNode__
{
	DWORD data1;
};

// Data written to the GHOSTREM section during a tag file write op
struct SavedGhostRem
{
	int objectID; // 0x0
	float x04;
	DWORD flags; // 0x8
	DWORD x0c;
	int weaponObj; // 0x10
	int weaponID; // 0x14
};

struct _TagFileTag
{
	char tag[16];
};

struct VersionNum
{
	unsigned int minor;
	unsigned int major;
};

struct IEsndTagProperty
{
	DWORD x00;
	char name[32]; // length unknown
};

class cRoom;
class cRoomPortal;

class cPropSndInstHigh
{
public:
	DWORD vtable;
	DWORD x04, x08, x0c, x10;
	int attachedObject; // 0x14 - object the sound is attached to or associated with
	mxs_vector position; // 0x18 - 0x20 - position the sound is played at
	const char* sndName; // 0x24 - name of the sound file
	DWORD x28;
	int sndSchema; // 0x2C
};

class cPhysModels
{
public:
	BYTE unk[0x1C];
	cHashSet pHashSet;
};

class cPhysDynData
{
	DWORD x00, x04, x08, x0c, x10, x14, x18;
	mxs_vector velocity; // 0x1C - 0x24
	DWORD x28, x2c, x30, x34, x38, x3c, x40;
	mxs_vector x44;
};

#define PHYSMODEL_ISPLAYER 0x8
#define PHYSMODEL_ISMOVEABLE 0x2
class cPhysModel
{
public:
	DWORD vtable;
	DWORD x04, x08;
	int m_objID;
	int m_numSubmodels; // 0x10
	DWORD x14, x18, x1c;
	DWORD m_flagState; // 0x20
	DWORD x24, x28, x2c, x30;
	mxs_vector m_location; // 0x34 - 3C (location of the whole phys model)
	DWORD x40;
	mxs_angvec m_angle;
	short x4a;
	DWORD x4c, x50, x54, x58, x5c, x60, x64, x68, x6c, x70, x74, x78, x7c, x80, x84, x88, x8c, x90, x94, x98, x9c, xa0, xa4, xa8,
		xac, xb0, xb4, xb8, xbc, xc0, xc4, xc8, xcc, xd0, xd4, xd8, xd8c, xe0, xe4, xe8, xec, xf0, xf4, xf8, xfc, x100, x104, x108;
	cPhysDynData* m_pDynData; // 0x10C;
	DWORD x110, x114, x118, x11c, x120, x124, x128, x12c, x130, x134, x138, x13c, x140, x144, x148, x14c, x150, x154, x158, x15c, x160, x164;
	float m_mass; // 0x168
	float m_elasticity; // 0x16C
	DWORD x170, x174, x178, x17c, x180, x184, x188, x18c, x190, x194, x198, x19c, x1a0, x1a4, x1a8;
	mxs_vector m_velocity; // used for player ghost velocity calculations
};

struct Rect
{
	const Rect& operator += (const Rect& rhs)
	{
		this->left += rhs.left;
		this->top += rhs.top;
		this->right += rhs.right;
		this->bottom += rhs.bottom;

		return *this;
	}

	void Clear(){ left = right = top = bottom = 0; } 
	int Width() const { return right - left; }
	int Height() const { return bottom - top; }
	void Offset(int x, int y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	short left;
	short top;
	short right;
	short bottom;
};

struct Point
{
	Point() { }
	Point(short inX, short inY) { x = inX; y = inY; }

	short x;
	short y;
};

struct _Region
{
	DWORD x00, x04, x08, x0c, x10;
	void* pfnExposeFunc; // 0x14
	DWORD x18, x1c;
	DWORD flags; // 0x20
};

typedef void (*_LGadTextboxCallback)(_LGadTextBox *,_LGadTextBoxEvent,int,void *);
struct LGadTextboxCreateParams
{
	Rect rect;
	char* pTextBuffer; // 0x8
	int maxChars; // 0xC
	DWORD x10;
	void* pfnCB; // 0x14
	void* data; // 0x18 - data passed to callback function
	DWORD x1c;
};

// Appears to either inherit from or contain _Region.
// Since gadget source files are .c, containment is more likely
struct _LGadRoot : public _Region
{
	_Region region;
};

struct LGadSlider
{

};

struct TextboxState
{
	char data[568];
};

struct sAIImpulse
{
	mxs_vector vel; // 0x0
};

struct sAIMoveEnactorx10
{
	eAISpeed m_AISpeed; // 0x0
};

struct sAIMoveGoal
{
	mxs_vector goalPosition; // 0x0

};

struct sAIProp
{
	char behaviorSet[32];
};

struct sReactionDesc
{
	char name[32];
	char description[64];
	char unknown[32];
};

class SchemaPlayList
{
public:
	struct sSchemaPlay* next;
	struct sSchemaPlay* prev;
};

// size is 0x30
struct sSchemaCallParams
{
	DWORD callFlags;			// 0x0 - 0xB0 for AI speech
	DWORD P2;					// 0x4 - unknown, -1 in almost all cases
	int object1;					// 0x8
	Label* conceptName;	// 0xC
	DWORD P5;					// 0x10
	_SpeechEndCallback pfnEndSampleCB;	// end sample callback - 0x14
	DWORD P6;
	int object2;			// identical to object1
	DWORD P7;
	DWORD P8;
	DWORD P9;
	DWORD P10;
};

// 5C bytes
struct sSchemaPlay
{
	sSchemaPlay* next;
	sSchemaPlay* prev;
	int schemaID; // 0x8
	DWORD callFlags; // 0xc - sSchemaCallParams.callFlags
	int object; // 0x10-  sSchemaCallParams.object1
	float x14, x18, x1c;
	DWORD x20; // sSchemaCallParams.P5
	DWORD x24, x28, x2c;
	_SpeechEndCallback EndCallback; // 0x30
	DWORD x34, x38, x3c, x40, x44, x48, x4c, x50, x54, x58;
};

struct sSpeech
{
	DWORD flags; // 0x0
	uint time; // 0x4
	int schemaID; // 0x8
	int concept; // 0xc
	int schemaHandle; // 0x10
	sSchemaPlay* pSchemaPlay; // 0x14
};

class cNameMap
{
public:
	int IDFromName(const Label* pLabel);
};

class cSpeechDomain : public cNameMap
{
public:
	int ConceptIndex(const Label* pLabel) { return IDFromName(pLabel); }
};

struct sAINetTransfer
{
	DWORD c; // 0x0 - always 0xc
	mxs_vector* objPos;
};

class cAINonCombatAbility
{
	DWORD vtbl;
	BYTE unk[0x1C];
	int m_alertnessBroadcastDelay; // 0x20 - 2000
};

union uPropListenerValue;

struct sPropertyDesc
{
	char name[16];
	DWORD x10, x14, x18, x1c, x20;
	const char* parentMenuName; // 0x24
	const char* propertyName; // 0x28
	DWORD netFlags; // 0x2c
};

class cStoredProperty
{
	DWORD vtable;
	sPropertyDesc desc; // 0x4 - 0x30
};

// size = 8
struct sAIAlertness
{
	eAIAwareLevel level;
	eAIAwareLevel peak;
};

struct ColorInfo
{
	float hue;
	float saturation;
};

struct sLightProp
{
	float brightness;
	mxs_vector offsetFromObj;
	float radius;
	float innerRadius;
	BOOL bQuadLit;
};

struct sFlinder
{
	int count;
	float impulse;
	BOOL bScatter;
	mxs_vector offset;
};

struct sPropagateSourceScale
{
	BOOL bPropagateSourceScale;
};

struct sAIAwareDelay
{
	int reactTimeToTwo;
	int reactTimeToThree;
	int retriggerTwoDelay;
	int retriggerThreeDelay;
	int ignoreDelayRange;
};

// size is off a bit
struct sAIAwareness 
{
	int  object; // 0x0 - the object that this sAIAwareness is linked to (i.e the object that the AI is aware of). not listed in struct desc, for some reason
	int flags; // 0x4 - 1 indicates AI can see target
	int level; // 0x8
	int peakLevel; // 0xC
	int levelEnterTime; // 0x10
	int timeLastContact; // 0x14
	mxs_vector posLastContact;
	int lastPulseLevel;
	int visionCone;
	int timeLastUpdate;
	int timeLastUpdateLOS;
	int lastTrueContact;
	int freshness;
};

struct sAutomapProperty
{
	int page;
	int location;
};

struct sRGBA
{
	int r;
	int g;
	int b;
	int a;
};

struct PCXHEAD
{
  char id;
  char version;
  char encoding;
  char bpp;
  short x1;
  short y1;
  short x2;
  short y2;
  short hRes;
  short vRes;
  char clrMap[16*3];
  char reserved1;
  char numPlanes;
  short bpl;
  short pal_t;
  char filler[58];
};

struct sSingleRemap;

class object
{
public:
	operator int () const
		{ return id; }
	object& operator= (int i)
		{ id = i; return *this; }
	bool operator== (const object & o)
		{ return id == o.id; }
	bool operator== (int i)
		{ return id == i; }
	bool operator!= (const object & o)
		{ return id != o.id; }
	bool operator!= (int i)
		{ return id != i; }
	object (int iId = 0) : id(iId) { }
	int id;
};

struct true_bool
{
	unsigned int f;

	true_bool() : f(0) { }
	true_bool(int v) { f = v; }
	
	bool operator== (int v) const
	{
		return (f) ? (v != 0) : (v == 0);
	}
	bool operator!= (int v) const
	{
		return (f) ? (v == 0) : (v != 0);
	}
	bool operator! () const
	{
		return f == 0;
	}

	true_bool& operator= (int v)
	{
		f = v;
		return *this;
	}
	
	operator int () const
	{
		return (f) ? 1 : 0;
	}
	operator bool () const
	{
		return f != 0;
	}
};

enum eScrTimedMsgKind 
{
	kSTM_OneShot,
	kSTM_Periodic
};

class linkkind
{
public:
	/*
	linkkind (linkkind);
	*/
	linkkind (int iId = 0) : id(iId) { }
	bool operator== (const linkkind & f)
		{ return id == f.id; }
	bool operator!= (const linkkind & f)
		{ return id != f.id; }
	operator long () const
		{ return id; }
	long id;
};

enum ePropertyEvent
{
	kPropertyChange = 1,
	kPropertyAdd    = 2,
	kPropertyDelete = 4,
	kPropertyFull   = 15,
	kPropertyRebuild = 0x30,
};

typedef void (__stdcall *PropListenFunc)(sPropertyListenMsg*,PropListenerData);

struct _kbs_event
{
	char scanCode; // 0 - http://msdn.microsoft.com/en-us/library/ms894073.aspx
	char transitionState; // 1
};


#pragma pack(pop)