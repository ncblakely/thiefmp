#pragma once

#pragma pack(push, 1)

typedef ushort mxs_ang;

typedef enum {
	// external flags
	kGhostCfAI         = (0x01),  // im an AI
	kGhostCfIsObj      = (0x02),  // im an object
	kGhostCfPlayer     = (0x03),  // im a player
	kGhostCfNoType     = (0),     // ok, this indicates an error, nothing should be "NoType"
	kGhostCfTypeMask   = (0x03),  // mask for type

	kGhostCfNoCret     = (1<<2),  // im not a creature, so don't do mocaps
	kGhostCfNew        = (1<<3),  // im new
	kGhostCfCreate     = (1<<4),  // create me
	kGhostCfForce      = (1<<5),  // force an update
	kGhostCfObjPos     = (1<<6),  // use object pos, not physics pos

	// internal flags
	kGhostCfNoPacket   = (1<<16), // have not yet even seen a packet
	kGhostCfLoaded     = (1<<17), // ive just been loaded from a file
	kGhostCfNoPhys     = (1<<18), // savetime only: has no physics
	kGhostCfDontRun    = (1<<19), // although i have physics, dont run me on rcv
	kGhostCfDisable    = (1<<20), // do nothing, period, ever, just shut up
	kGhostCfFirstTPort = (1<<21), // set until we have processed first packet
} eGhostConfigFlags;

#define _GhostGetType(flags)     ((flags)&kGhostCfTypeMask)
#define _GhostIsType(flags,type) (_GhostGetType(flags)==kGhostCf##type##)

// how to interpret certain heartbeat fields
typedef enum {
	kGhostHBUseG       = (1<<0),  // use guaranteed packets
	kGhostHBFullAngs   = (1<<1),  // use full angles, not the tz,dtz,p format
	kGhostHBObjRel     = (1<<2),  // this has a new RelObj object attached
	kGhostHBAbsVel     = (1<<3),  // this vel is abs, not a target control vel
	kGhostHBHaveMode   = (1<<4),  // i have a valid mode set
	kGhostHBStartSw    = (1<<5),  // start a swing
	kGhostHBGravity    = (1<<6),  // ghost should have gravity
	kGhostHBTeleport   = (1<<7),  // ghost should be teleported
	kGhostHBDead       = (1<<8),  // ghost is dead    - move these two into Mode!
	kGhostHBWeap       = (1<<9),  // this packet has weap data
	kGhostHBOnObj      = (1<<10), // this is relative to an obj, not absolute
	kGhostHBHaveCap    = (1<<11), // this packet has a capture attached
	kGhostHBSlowDown   = (1<<12), // i am slowing down
	kGhostHBAxisRot    = (1<<13), // rotate on y and x as well, for fun
	kGhostHBAngOnly    = (1<<14), // actually filled in on receive...
	kGhostHBNoZPos     = (1<<15), //  as is this, in minipacket setups
} eGhostHeartBeatFlags;

typedef enum {
	kGhostStDying      = (1<<0),
	kGhostStWounded    = (1<<1),
	kGhostStSwinging   = (1<<2),
	kGhostStFiring     = (1<<3),
	kGhostStSleep      = (1<<4),
	kGhostStRevive     = (1<<5),
	kGhostStDead       = (1<<6),  // internal only - for between dying and revive
	kGhostStMax        = kGhostStDead,
	kGhostStLastPerFrame = kGhostStFiring,
	kGhostStIrqChecks  = kGhostStDying|kGhostStWounded|kGhostStSwinging|kGhostStFiring,
} eGhostState;

// motion modes, initial hack
typedef enum {
	kGhostModeStand,        // first group is mirror of kPM_
	kGhostModeCrouch,
	kGhostModeSwim,
	kGhostModeClimb,
	kGhostModeCarry,        // umm, ok, well, whatever
	kGhostModeSlide,        // speaking of whatever...
	kGhostModeJumpBase,
	kGhostModeDead,         // secretly, this is in both plyrmode and in our secret set

	kGhostModeLeanLeft,     // for non PlayerGetMode player modes
	kGhostModeLeanRight,

	kGhostModeJumpStart,    // now the "internal" ghost state stuff
	kGhostModeJumping,

	kGhostModeRevive,       // implies stand, secretly resets ghost as well
	kGhostModeSleep,

	kGhostModeMax=kGhostModeJumping+1,          // for tests and sanity checking
	kGhostModeNone=kGhostModeMax,
} eGhostMotionModes;

// speeds
typedef enum {
	kGhostSpeedNone,
	kGhostSpeedNorm,
	kGhostSpeedFast
} eGhostMotionSpeed;

typedef enum {
	kGhostRcvDidWeap = (1<<0), // have we set our weapon state bits...
	kGhostRcvCustom  = (1<<1), // are we playing a custom capture
	kGhostRcvNoInt   = (1<<2), // 
	kGhostRcvHasDied = (1<<3), // 
	kGhostRcvStand   = (1<<4), // is our tag based motion a stand
	kGhostRcvLoco    = (1<<5), // is our tag based motion a loco
	kGhostRcvDieNext = (1<<6), // should we die after playing this motion
} eGhostRecvState;

// or'd in above
typedef enum {
	kGhostSendStBase       = (kGhostStMax<<1),
	kGhostSendStSleeping   = (kGhostSendStBase<<0),
} eGhostSendState;

typedef enum {
	kGhostMotSchemaNoCustom = -1,
	kGhostMotSchemaGLoco    = -2,
} eGhostMotSchema;

////////////////////////////
// actual ghost structures

typedef struct {
	float      detail;      // detail "slider" - 0.0 means minimal, 2.0 means tons???? dumb!
	int        flags;       // global config flags
} sGhostConfig;

//////////////////////////////
// basic position/time data structure

struct sGhostHeartbeat
{
	mxs_vector pos;         // 12 - position
	mxs_vector vel;         // 12 - current or control velocity (check flags)
	union {
		struct {             // interpret as heading, heading velocity, and pitch
			mxs_ang p;
			short   dtz;      // in wack units, since isnt a float
			mxs_ang tz;       // use tz last, so same place as in fac - ideally
		};
		mxs_angvec fac;      // interpret it as a full facing
	} angle_info;           // 6 bytes
	ushort     flags;       // 2  - control flags 
	uchar      mode;        // 1  - motion mode, for relevant cases
	uchar      weap;        // 1  - weapon index
};         // 34 bytes


typedef struct {
	sGhostHeartbeat pos;     // 34 - core positional data
	ObjID           rel_obj; // 4  - relative object, if we need one (could be 2bytes)
	int             time;    // 4  - local time at which this packet is "fresh"
} sGhostPos;                // 42 bytes - ack

// Size = 0x2A

//////////////////////////////
// in case we need this for bandwidth reduction later

typedef struct {
	mxs_vector pos;         // 12 - position
	float      velx,vely;   // 8  - x,y current or control vel
	mxs_ang    tz;          // 2  - heading (tz)
	ushort     flags;       // 2  - control flags
} sGhostMiniHeartbeat;     // 24 bytes

// hmmm, something like this, maybe?
typedef struct {
	float      posx,posy;   // 8  - position
	float      velx,vely;   // 8  - x,y current or control vel
	mxs_ang    tz,dtz;      // 4  - heading (tz), and dtz for vel
} sGhostAIMiniHB;          // 20 bytes

// for small Obj stuff
typedef struct {
	mxs_vector pos;         // 12 - position
	mxs_vector vel;         // 12 - current or control velocity
} sGhostObjMiniHB;         // 24 bytes

typedef struct {
	union {
		struct {             // interpret as heading, heading velocity, and pitch
			mxs_ang p;
			short   dtz;      // in wack units, since isnt a float
			mxs_ang tz;       // use tz last, so same place as in fac - ideally
		};
		mxs_angvec fac;      // interpret it as a full facing
	} angle_info;           // 6 bytes
} sGhostRotMiniHB;

//////////////////////////////
// total position info for send and recv

typedef struct {
	mxs_ang        s_tz;
	short          s_dtz;
	int            s_time;
} sGhostDtzData;

#define NUM_DTZ_SAMPLES  (8)
#define MASK_DTZ_SAMPLES (NUM_DTZ_SAMPLES-1)
typedef struct {
	sGhostDtzData  samples[NUM_DTZ_SAMPLES];
	int            s_ptr;   // where to put next sample
	int            s_cnt;   // how many valid samples we have
	int            streak;  // streak, + means cons non-zero, - means consec zero
	int            dtz_sum; // dtz_sum for valid samples
} sGhostDtzInfo;

typedef struct {
	sGhostPos      last;    // where we last sent info about it
	sGhostPos      pred;    // predicted position info only
	ushort         seq_id;  // current sequence ID for packets
	ushort         pad;     // pad to dword
	ObjID          rel_obj; // current relative object - for edge trigger detect
	sGhostDtzInfo  dtz_dat; // data about dtz handling
} sGhostSendInfo;

typedef struct {
	sGhostPos      last;    // where we last heard about it
	sGhostPos      pred;    // where our predition model currently takes it
	//   sGhostRating   rating;  // rating for how well we are doing
	ushort         seq_id;  // current sequence ID for packets
	ushort         last_g_seq; // last guaranteed packet
} sGhostRecvInfo;

//////////////////////////////
// the peril of creatures/motion system attach

// this is very "in progress", but hey
struct sGhostMoCap
{
	short schema_idx;  // which schema
	short motion_num;  // which motion within the schema
};

#define MAX_MOCAP_STR_LEN (96)  // who knows, whatever, etc
struct _sGhostCreature {
	struct IMotionCoordinator *pMotCoord;
	sGhostMoCap         cur_mocap;
	uchar               mmode;
	uchar               speed;
	uchar               r_state;                        // eGhostRcvState, really
	uchar               stand_cnt;                      // number of stands in a row
	char                mocap_name[MAX_MOCAP_STR_LEN];
};  // @TBD: should convert to TagSet, i suppose, for now, im just gonna use a string
typedef struct _sGhostCreature sGhostCreature;  // in case we ever modularize/fdeclare

typedef struct {
	ObjID          weaponObj;
	char           nWeapon;
	char           pad[3];
} sGhostRecvWeap;

//////////////////////////////
// the actual elements in the hash
//  i.e. full info for a ghost of the right type

typedef struct {
	ObjID          obj;     // my obj ID
	char           nWeapon; // looked up index of local weapon number
	char           pad[3];  // pad, woo woo
	sGhostConfig   cfg;     // my setup
	sGhostSendInfo info;    // where am I
	sGhostMoCap    playing; // what mocap am i playing
	int            state;   // state im tracking about events, eGhostState
	int            last_fr; // time of last frame we ran
} sGhostLocal;

BOOST_STATIC_ASSERT(offsetof(sGhostLocal, info.seq_id) == 0x64);
BOOST_STATIC_ASSERT(sizeof(sGhostLocal) == 0xC8);

struct sGhostRemote 
{
	ObjID          obj;     // my obj ID
	sGhostConfig   cfg;     // my setup
	sGhostRecvInfo info;    // where am I
	sGhostCreature critter;
	sGhostRecvWeap weap;
};

BOOST_STATIC_ASSERT(offsetof(sGhostRemote, critter.r_state) == 0x6E);
BOOST_STATIC_ASSERT(sizeof(sGhostRemote) == 0xD8);

enum ePropertyNetworkingFlags_
{
	kPropertyChangeLocally     = 0x0001, // Hosts & proxies make changes locally (no net msgs).
	kPropertyNonGuaranteedMsgs = 0x0002, // Change broadcasts don't need guaranteed messaging.
	kPropertyProxyChangable    = 0x0004, // Proxy machines can initiate changes to this property.
};

#pragma pack(pop)