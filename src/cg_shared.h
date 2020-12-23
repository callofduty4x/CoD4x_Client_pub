#ifndef __CG_SHARED_H__
#define __CG_SHARED_H__

#include "q_shared.h"
#include "qcommon.h"
#include "xassets/xanim.h"
#include "xassets/sounds.h"
#include "xassets/font.h"
#include "xassets/fx.h"
#include "xassets/xmodel.h"
#include "client.h"
#include "refdef.h"

//#define cgsArray (*((cgsData_t*)(0x74a908)))
//#define cgArray (*((cgData_t*)(0x74E338)))
#define cgs (*((cgs_t*)(0x74a908)))
#define cg (*((cg_t*)(0x74E338)))
#define cgEntities ((centity_t*)(0x84F2D8))
#define cgMedia (*((cgMedia_t*)(0x84CB30)))
#define s_cameraShakeSet (*((CameraShakeSet*)(0x7380C0)))

typedef void FxEffect_t; //Too complicated

extern cvar_t* cg_zoom_sensitivity_ratio;

typedef struct{
	vec3_t kickAVel;
	vec3_t kickAngles;
}cgSafeAngles_t;


/* 775 */
typedef enum 
{
  DEMO_TYPE_NONE = 0x0,
  DEMO_TYPE_CLIENT = 0x1,
  DEMO_TYPE_SERVER = 0x2,
}DemoType;

/* 776 */
typedef enum 
{
  CUBEMAPSHOT_NONE = 0x0,
  CUBEMAPSHOT_RIGHT = 0x1,
  CUBEMAPSHOT_LEFT = 0x2,
  CUBEMAPSHOT_BACK = 0x3,
  CUBEMAPSHOT_FRONT = 0x4,
  CUBEMAPSHOT_UP = 0x5,
  CUBEMAPSHOT_DOWN = 0x6,
  CUBEMAPSHOT_COUNT = 0x7,
}CubemapShot;

/********************************************************************
ShellShock types
********************************************************************/

/* 777 */
typedef enum 
{
  SHELLSHOCK_VIEWTYPE_BLURRED = 0x0,
  SHELLSHOCK_VIEWTYPE_FLASHED = 0x1,
}ShockViewTypes;


/* 7111 */
struct shellshock_screenBlend_t
{
  int blurredFadeTime;
  int blurredEffectTime;
  int flashWhiteFadeTime;
  int flashShotFadeTime;
  ShockViewTypes type;
};

/* 7112 */
struct shellshock_view_t
{
  int fadeTime;
  float kickRate;
  float kickRadius;
};

/* 7113 */
#pragma pack(push, 4)
struct shellshock_sound_t
{
  byte affect;
  byte pad[3];
  char loop[64];
  char loopSilent[64];
  char end[64];
  char endAbort[64];
  int fadeInTime;
  int fadeOutTime;
  float drylevel;
  float wetlevel;
  char roomtype[16];
  float channelvolume[64];
  int modEndDelay;
  int loopFadeTime;
  int loopEndDelay;
};
#pragma pack(pop)

/* 7114 */
#pragma pack(push, 4)
struct shellshock_lookControl_t
{
  byte affect;
  byte pad[3];
  int fadeTime;
  float mouseSensitivity;
  float maxPitchSpeed;
  float maxYawSpeed;
};
#pragma pack(pop)

/* 7115 */
struct shellshock_movement_t
{
  byte affect;
  byte pad[3];
};

/* 7116 */
#pragma pack(push, 4)
struct shellshock_parms_t
{
  struct shellshock_screenBlend_t screenBlend;
  struct shellshock_view_t view;
  struct shellshock_sound_t sound;
  struct shellshock_lookControl_t lookControl;
  struct shellshock_movement_t movement;
};
#pragma pack(pop)


/* 7117 */
typedef struct 
{
  struct shellshock_parms_t *parms;
  int startTime;
  int duration;
  int loopEndTime;
  float sensitivity;
  float viewDelta[2];
  int hasSavedScreen;
}shellshock_t;


/* 7118 */
typedef struct
{
  int time;
  int duration;
}cgShockinfo_t;

/***************************************************************************
CG_Animation types
****************************************************************************/

/* 7119 */
#pragma pack(push, 8)
typedef struct animation_s
{
  char name[64];
  int initialLerp;
  float moveSpeed;
  int duration;
  int nameHash;
  int flags;
  int64_t movetype;
  int noteType;
}animation_t;
#pragma pack(pop)

/* 7126 */
typedef struct scr_animuabs_s
{
  uint16_t index;
  uint16_t tree;
}scr_animuabs_t;

/* 7124 */
typedef struct scr_animtree_s
{
  XAnim_s *anims;
}scr_animtree_t;

typedef union
{
  scr_animuabs_t s;
  const char *linkPointer;
}scr_animu_s;

/* 7128 */
typedef struct scr_anim_s
{
  scr_animu_s u;
}scr_anim_t;

/* 7129 */
typedef struct 
{
  scr_animtree_t tree;
  scr_anim_t torso;
  scr_anim_t legs;
  scr_anim_t turning;
}bodyAnim_t;


/* 7121 */
typedef struct 
{
  int16_t bodyPart[2];
  int16_t animIndex[2];
  int16_t animDuration[2];
  snd_alias_list_t *soundAlias;
}animScriptCommand_t;

/* 7120 */
typedef struct 
{
  int index;
  unsigned int value[2];
}animScriptCondition_t;


/* 7122 */
typedef struct 
{
  int numConditions;
  animScriptCondition_t conditions[10];
  int numCommands;
  animScriptCommand_t commands[8];
}animScriptItem_t;


/* 7123 */
typedef struct 
{
  int numItems;
  animScriptItem_t *items[128];
}animScript_t;


/* 7125 */
#pragma pack(push, 8)
typedef struct
{
  animation_t animations[512];
  unsigned int numAnimations;
  animScript_t scriptAnims[1][43];
  animScript_t scriptCannedAnims[1][43];
  animScript_t scriptStateChange[1][1];
  animScript_t scriptEvents[21];
  animScriptItem_t scriptItems[2048];
  int numScriptItems;
  scr_animtree_t animTree;
  uint16_t torsoAnim;
  uint16_t legsAnim;
  uint16_t turningAnim;
  uint16_t pad;
  snd_alias_list_t *(__cdecl *soundAlias)(const char *);
  int (__cdecl *playSoundAlias)(int, snd_alias_list_t *);
  int pad2;
}animScriptData_t;
#pragma pack(pop)

/***************************************************************************
Vision Set types
****************************************************************************/


/* 779 */
typedef enum 
{
  VISIONSETLERP_UNDEFINED = 0x0,
  VISIONSETLERP_NONE = 0x1,
  VISIONSETLERP_TO_LINEAR = 0x2,
  VISIONSETLERP_TO_SMOOTH = 0x3,
  VISIONSETLERP_BACKFORTH_LINEAR = 0x4,
  VISIONSETLERP_BACKFORTH_SMOOTH = 0x5,
}visionSetLerpStyle_t;

/* 7134 */
#pragma pack(push, 4)
typedef struct 
{
  byte glowEnable;
  byte pad[3];
  float glowBloomCutoff;
  float glowBloomDesaturation;
  float glowBloomIntensity0;
  float glowBloomIntensity1;
  float glowRadius0;
  float glowRadius1;
  float glowSkyBleedIntensity0;
  float glowSkyBleedIntensity1;
  byte filmEnable;
  byte pad2[3];
  float filmBrightness;
  float filmContrast;
  float filmDesaturation;
  byte filmInvert;
  byte pad3[3];
  float filmLightTint[3];
  float filmDarkTint[3];
}visionSetVars_t;
#pragma pack(pop)


/* 7135 */
typedef struct 
{
  int timeStart;
  int timeDuration;
  visionSetLerpStyle_t style;
}visionSetLerpData_t;


/***************************************************************************
ClientInfo type
****************************************************************************/


/* 7131 */
typedef struct lerpFrame_s
{
  float yawAngle;
  int yawing;
  float pitchAngle;
  int pitching;
  int animationNumber;
  animation_t *animation;
  int animationTime;
  float oldFramePos[3];
  float animSpeedScale;
  int oldFrameSnapshotTime;
}lerpFrame_t;


/* 7132 */
#pragma pack(push, 4)
typedef struct clientInfo_s
{
  int infoValid;
  int nextValid;
  int clientNum;
  char name[16];
  team_t team;
  team_t oldteam;
  int rank;
  int unk1;
  int unk2;
  int score;
  int location;
  int health;
  char model[64];
  char attachModelNames[6][64];
  char attachTagNames[6][64];
  lerpFrame_t legs;
  lerpFrame_t torso;
  float lerpMoveDir;
  float lerpLean;
  float playerAngles[3];
  int leftHandGun;
  int dobjDirty;
  clientControllers_t control;
  unsigned int clientConditions[10][2];
  XAnimTree_s *pXAnimTree;
  int iDObjWeapon;
  char weaponModel;
  char pad[3];
  int stanceTransitionTime;
  int turnAnimEndTime;
  char turnAnimType;
  char pad2[3];  
  int attachedVehEntNum;
  int attachedVehSlotIndex;
  byte hideWeapon;
  byte usingKnife;
  char pad3[2];
}clientInfo_t;
#pragma pack(pop)

/***************************************************************************
DObj types
****************************************************************************/

/* 6916 */
typedef struct 
{
  float quat[4];
  float trans[3];
  float transWeight;
}DObjAnimMat;

/* 7074 */
typedef struct 
{
  int anim[4];
  int control[4];
  int skel[4];
}DSkelPartBits;

/* 7075 */
typedef struct 
{
  DSkelPartBits partBits;
  int timeStamp;
  DObjAnimMat *mat;
}DSkel;

/* 7076 */
#pragma pack(push, 2)
typedef struct DObj_s
{
  XAnimTree_s *tree;
  uint16_t duplicateParts;
  uint16_t entnum;
  char duplicatePartsSize;
  char numModels;
  char numBones;
  byte pad;
  unsigned int ignoreCollision;
  volatile int locked;
  DSkel skel;
  float radius;
  int hidePartBits[4];
  XModel **models;
}DObj_t;
#pragma pack(pop)

#pragma pack(push, 2)
typedef struct DObjModel_s
{
  XModel *model;
  uint16_t boneName;
  byte ignoreCollision;
  byte pad;
}DObjModel_t;
#pragma pack(pop)



/***************************************************************************
Other types
****************************************************************************/

/* 7136 */
typedef struct
{
  float aimSpreadScale;
}cgFrameInfo_t;


/* 7110 */
typedef struct 
{
  int time;
  int duration;
  float yaw;
}viewDamage_t;


/* 7109 */
typedef struct 
{
  int client;
  int score;
  int ping;
  int deaths;
  int team;
  int kills;
  int rank;
  int assists;
  Material *hStatusIcon;
  Material *hRankIcon;
}score_t;



/* 6848 */
typedef struct 
{
  int sprintButtonUpRequired;
  int sprintDelay;
  int lastSprintStart;
  int lastSprintEnd;
  int sprintStartMaxLength;
}SprintState;

/* 7137 */
typedef struct 
{
  int lastPlayedTime;
}hudElemSoundInfo_t;

/* 7104 */
typedef struct 
{
  float fLastWeaponPosFrac;
  int bPositionToADS;
  float vPositionLastOrg[3];
  float fLastIdleFactor;
  float vLastMoveOrg[3];
  float vLastMoveAng[3];
}playerEntity_t;



typedef struct snapshot_s
{
  int snapFlags;
  int ping;
  int serverTime;
  playerState_t ps;
  int numEntities;
  int numClients;
  entityState_t entities[512];
  clientState_t clients[64];
  int serverCommandNum;
}snapshot_t;




typedef struct
{
	char name[33];
	char clantag[13];
}gameClientDataX_t;

extern gameClientDataX_t gameClientDataX[64];


/* 7844 */
typedef struct
{
  int frameSamples[128];
  int frameCount;
  int snapshotFlags[128];
  int snapshotSamples[128];
  int snapshotCount;
}lagometer_t;

/***************************************************************************
CGame Core types: bgs_t, cg_t, cgs_t
****************************************************************************/

/* 7133 */
typedef struct bgs_s
{
  animScriptData_t animScriptData;
  bodyAnim_t generic_human;
  int time;
  int latestSnapshotTime;
  int frametime;
  int anim_user;
  XModel *(__cdecl *GetXModel)(const char *);
  void (__cdecl *CreateDObj)(DObjModel_t *, uint16_t, XAnimTree_s *, int, int, clientInfo_t *);
  uint16_t (__cdecl *AttachWeapon)(DObjModel_t *, uint16_t, clientInfo_t *);
  struct DObj_s *(__cdecl *GetDObj)(int, int);
  void (__cdecl *SafeDObjFree)(int, int);
  void *(__cdecl *AllocXAnim)(int);
  clientInfo_t clientinfo[64];
}bgs_t;


typedef struct cgsData_s
{
  int field_0;
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int serverCommandSequence;
  int field_18;
  int isdefgametype;
  char gametype[32];
  char hostname[256];
  int maxclients;
  char bspfilename[64];
  char unk[14496];
}cgsData_t;


#pragma pack(push, 1)
typedef struct cgData_s
{
  int clientNum;
  int localClientNum;
  DemoType demoType;
  CubemapShot cubemapShot;
  int cubemapSize;
  int renderScreen;
  int latestSnapshotNum;
  int latestSnapshotTime;
  snapshot_t *snap;
  snapshot_t *nextSnap;
  snapshot_t activeSnapshots[2];
  float frameInterpolation;
  int frametime;
  int time;
  int oldTime;
  int physicsTime;
  char field_46124[755380];
  int field_FE7EC;
}cgData_t;

#pragma pack(pop)

#define cg_drawFPS getcvaradr(0x748634)
#define cg_drawSnapshot getcvaradr(0x74A888)
#define cg_debugInfoCornerOffset getcvaradr(0x8C63B8)


typedef struct CEntPlayerInfo_s
{
  clientControllers_t *control;
  byte tag[6];
}CEntPlayerInfo_t;

typedef struct CEntTurretAngles_s
{
  float pitch;
  float yaw;
}CEntTurretAngles_t;


typedef struct CEntTurretInfo_s
{
  union
  {
    CEntTurretAngles_t angles;
	const float *viewAngles;
  };
  
  float barrelPitch;
  byte playerUsing;
  byte tag_aim;
  byte tag_aim_animated;
  byte tag_flash;
}CEntTurretInfo_t;


typedef struct CEntVehicleInfo_s
{
  short pitch;
  short yaw;
  short roll;
  short barrelPitch;
  float barrelRoll;
  short steerYaw;
  byte pad[2];
  float time;
  unsigned short wheelFraction[4];
  byte wheelBoneIndex[4];
  byte tag_body;
  byte tag_turret;
  byte tag_barrel;
  byte pad2;
}CEntVehicleInfo_t;

typedef struct CEntFx_s
{
  int triggerTime;
  FxEffect_t *effect;
}CEntFx_t;

typedef union
{
  CEntPlayerInfo_t player;
  CEntTurretInfo_t turret;
  CEntVehicleInfo_t vehicle;
  CEntFx_t fx;
  byte badsizepad[0x30];
}CEntInfo_t;


typedef struct cpose_s
{
  unsigned short lightingHandle;
  byte eType;
  byte eTypeUnion;
  byte localClientNum;
  byte pad[3];
  int cullIn;
  byte isRagdoll;
  byte pad2[3];
  int ragdollHandle;
  int killcamRagdollHandle;
  int physObjId;
  vec3_t origin;
  vec3_t angles;
  CEntInfo_t centInfo;
}cpose_t;


typedef struct centity_s
{
  cpose_t pose;
  LerpEntityState_t currentState;
  entityState_t nextState;
  byte nextValid;
  byte bMuzzleFlash;
  byte bTrailMade;
  byte pad;
  int previousEventSequence;
  int miscTime;
  vec3_t lightingOrigin;
  XAnimTree_s *tree;
}centity_t;


/* 7255 */

typedef struct 
{
  Material *whiteMaterial;
  Material *teamStatusBar;
  Material *balloonMaterial;
  Material *connectionMaterial;
  Material *youInKillCamMaterial;
  Material *tracerMaterial;
  Material *laserMaterial;
  Material *laserLightMaterial;
  Material *lagometerMaterial;
  Material *hintMaterials[133];
  Material *stanceMaterials[4];
  Material *objectiveMaterials[1];
  Material *friendMaterials[2];
  Material *damageMaterial;
  Material *mantleHint;
  Font_t *smallDevFont;
  Font_t *bigDevFont;
  snd_alias_list_t *landDmgSound;
  snd_alias_list_t *grenadeExplodeSound[29];
  snd_alias_list_t *rocketExplodeSound[29];
  snd_alias_list_t *bulletHitSmallSound[29];
  snd_alias_list_t *bulletHitLargeSound[29];
  snd_alias_list_t *bulletHitAPSound[29];
  snd_alias_list_t *shotgunHitSound[29];
  snd_alias_list_t *bulletExitSmallSound[29];
  snd_alias_list_t *bulletExitLargeSound[29];
  snd_alias_list_t *bulletExitAPSound[29];
  snd_alias_list_t *shotgunExitSound[29];
  snd_alias_list_t *stepSprintSound[58];
  snd_alias_list_t *stepSprintSoundPlayer[58];
  snd_alias_list_t *stepRunSound[58];
  snd_alias_list_t *stepRunSoundPlayer[58];
  snd_alias_list_t *stepWalkSound[58];
  snd_alias_list_t *stepWalkSoundPlayer[58];
  snd_alias_list_t *stepProneSound[58];
  snd_alias_list_t *stepProneSoundPlayer[58];
  snd_alias_list_t *landSound[58];
  snd_alias_list_t *landSoundPlayer[58];
  snd_alias_list_t *qsprintingEquipmentSound;
  snd_alias_list_t *qsprintingEquipmentSoundPlayer;
  snd_alias_list_t *qrunningEquipmentSound;
  snd_alias_list_t *qrunningEquipmentSoundPlayer;
  snd_alias_list_t *qwalkingEquipmentSound;
  snd_alias_list_t *qwalkingEquipmentSoundPlayer;
  snd_alias_list_t *sprintingEquipmentSound;
  snd_alias_list_t *sprintingEquipmentSoundPlayer;
  snd_alias_list_t *runningEquipmentSound;
  snd_alias_list_t *runningEquipmentSoundPlayer;
  snd_alias_list_t *walkingEquipmentSound;
  snd_alias_list_t *walkingEquipmentSoundPlayer;
  snd_alias_list_t *foliageMovement;
  snd_alias_list_t *bulletWhizby;
  snd_alias_list_t *meleeSwingLarge;
  snd_alias_list_t *meleeSwingSmall;
  snd_alias_list_t *meleeHit;
  snd_alias_list_t *meleeHitOther;
  snd_alias_list_t *nightVisionOn;
  snd_alias_list_t *nightVisionOff;
  snd_alias_list_t *playerSprintGasp;
  snd_alias_list_t *playerHeartBeatSound;
  snd_alias_list_t *playerBreathInSound;
  snd_alias_list_t *playerBreathOutSound;
  snd_alias_list_t *playerBreathGaspSound;
  snd_alias_list_t *playerSwapOffhand;
  snd_alias_list_t *physCollisionSound[50][29];
  Material *compassping_friendlyfiring;
  Material *compassping_friendlyyelling;
  Material *compassping_enemy;
  Material *compassping_enemyfiring;
  Material *compassping_enemyyelling;
  Material *compassping_grenade;
  Material *compassping_explosion;
  Material *compass_radarline;
  Material *compass_helicopter_friendly;
  Material *compass_helicopter_enemy;
  Material *compass_plane_friendly;
  Material *compass_plane_enemy;
  Material *grenadeIconFrag;
  Material *grenadeIconFlash;
  Material *grenadeIconThrowBack;
  Material *grenadePointer;
  Material *offscreenObjectivePointer;
  FxImpactTable *fx;
  FxEffectDef *fxNoBloodFleshHit;
  FxEffectDef *fxKnifeBlood;
  FxEffectDef *fxKnifeNoBlood;
  FxEffectDef *heliDustEffect;
  FxEffectDef *heliWaterEffect;
  FxEffectDef *helicopterLightSmoke;
  FxEffectDef *helicopterHeavySmoke;
  FxEffectDef *helicopterOnFire;
  FxEffectDef *jetAfterburner;
  FxEffectDef *fxVehicleTireDust;
  Material *nightVisionOverlay;
  Material *hudIconNVG;
  Material *hudDpadArrow;
  Material *ammoCounterBullet;
  Material *ammoCounterBeltBullet;
  Material *ammoCounterRifleBullet;
  Material *ammoCounterRocket;
  Material *ammoCounterShotgunShell;
  Material *textDecodeCharacters;
  Material *textDecodeCharactersGlow;
}cgMedia_t;




/* 7138 */
typedef struct cg_s
{
  int clientNum;
  int localClientNum;
  DemoType demoType;
  CubemapShot cubemapShot;
  int cubemapSize;
  int renderScreen;
  int latestSnapshotNum;
  int latestSnapshotTime;
  snapshot_t *snap;
  snapshot_t *nextSnap;
  snapshot_t activeSnapshots[2];
  float frameInterpolation;
  int frametime;
  int time;
  int oldTime;
  int physicsTime;
  int mapRestart;
  int renderingThirdPerson;
  playerState_t predictedPlayerState;
  centity_t predictedPlayerEntity;
  playerEntity_t playerEntity;
  int predictedErrorTime;
  float predictedError[3];
  float landChange;
  int landTime;
  float heightToCeiling;
  struct refdef_s refdef;
  float refdefViewAngles[3];
  float lastVieworg[3];
  float swayViewAngles[3];
  float swayAngles[3];
  float swayOffset[3];
  int iEntityLastType[1024];
  XModel *pEntityLastXModel[1024];
  float zoomSensitivity;
  byte isLoading;
  char objectiveText[1024];
  char scriptMainMenu[256];
  byte pad[3];
  int scoresRequestTime;
  int numScores;
  int teamScores[4];
  int teamPings[4];
  int teamPlayers[4];
  score_t scores[64];
  int scoreLimit;
  int scoreFadeTime;
  int scoresTop;
  int scoresOffBottom;
  int scoresBottom;
  int bannerLines[1];
  int drawHud;
  int crosshairClientNum;
  int crosshairClientLastTime;
  int crosshairClientStartTime;
  int identifyClientNum;
  int cursorHintIcon;
  int cursorHintTime;
  int cursorHintFade;
  int cursorHintString;
  int lastClipFlashTime;
  int invalidCmdType;
  int invalidCmdBlinkTime;
  int lastHealthPulseTime;
  int lastHealthLerpDelay;
  int lastHealthClient;
  float lastHealth;
  float healthOverlayFromAlpha;
  float healthOverlayToAlpha;
  int healthOverlayPulseTime;
  int healthOverlayPulseDuration;
  int healthOverlayPulsePhase;
  byte healthOverlayHurt;
  byte pad2[3];
  int healthOverlayLastHitTime;
  float healthOverlayOldHealth;
  int healthOverlayPulseIndex;
  int proneBlockedEndTime;
  int lastStance;
  int lastStanceChangeTime;
  int lastStanceFlashTime;
  int voiceTime;
  unsigned int weaponSelect;
  int weaponSelectTime;
  unsigned int weaponLatestPrimaryIdx;
  int prevViewmodelWeapon;
  int equippedOffHand;
  viewDamage_t viewDamage[8];
  int damageTime;
  float damageX;
  float damageY;
  float damageValue;
  float viewFade;
  int weapIdleTime;
  int nomarks;
  int v_dmg_time;
  float v_dmg_pitch;
  float v_dmg_roll;
  float fBobCycle;
  float xyspeed;
  float kickAVel[3];
  float kickAngles[3];
  float offsetAngles[3];
  float gunPitch;
  float gunYaw;
  float gunXOfs;
  float gunYOfs;
  float gunZOfs;
  float vGunOffset[3];
  float vGunSpeed[3];
  float viewModelAxis[4][3];
  float rumbleScale;
  float compassNorthYaw;
  float compassNorth[2];
  Material *compassMapMaterial;
  float compassMapUpperLeft[2];
  float compassMapWorldSize[2];
  int compassFadeTime;
  int healthFadeTime;
  int ammoFadeTime;
  int stanceFadeTime;
  int sprintFadeTime;
  int offhandFadeTime;
  int offhandFlashTime;
  shellshock_t shellshock;
  cgShockinfo_t testShock;
  int holdBreathTime;
  int holdBreathInTime;
  int holdBreathDelay;
  float holdBreathFrac;
  float radarProgress;
  float selectedLocation[2];
  SprintState sprintStates;
  int adsViewErrorDone;
  int inKillCam;
  bgs_t bgs;
  cpose_t viewModelPose;
  visionSetVars_t visionSetPreLoaded[4];
  char visionSetPreLoadedName[4][64];
  visionSetVars_t visionSetFrom[2];
  visionSetVars_t visionSetTo[2];
  visionSetVars_t visionSetCurrent[2];
  visionSetLerpData_t visionSetLerpData[2];
  char visionNameNaked[64];
  char visionNameNight[64];
  int extraButtons;
  int lastActionSlotTime;
  byte playerTeleported;
  byte pad3[3];
  int stepViewStart;
  float stepViewChange;
  cgFrameInfo_t lastFrame;
  hudElemSoundInfo_t hudElemSound[32];
  int vehicleFrame;
}cg_t;

/* 7140 */
typedef struct cgs_s
{
  int viewX;
  int viewY;
  int viewWidth;
  int viewHeight;
  float viewAspect;
  int serverCommandSequence;
  int processedSnapshotNum;
  int localServer;
  char gametype[32];
  char szHostName[256];
  int maxclients;
  char mapname[64];
  int gameEndTime;
  int voteTime;
  int voteYes;
  int voteNo;
  char voteString[256];
  XModel *gameModels[512];
  FxEffectDef *fxs[100];
  FxEffectDef *smokeGrenadeFx;
  const struct shellshock_parms_t holdBreathParams;
  char teamChatMsgs[8][160];
  int teamChatMsgTimes[8];
  int teamChatPos;
  int teamLastChatPos;
  float compassWidth;
  float compassHeight;
  float compassY;
  clientInfo_t corpseinfo[8];
}cgs_t;


/* 7851 */
typedef struct
{
  int time;
  float scale;
  float length;
  float radius;
  float src[3];
  float size;
  float rumbleScale;
}CameraShake;

/* 7852 */
typedef struct
{
  CameraShake shakes[4];
  float phase;
}CameraShakeSet;


void CG_DrawUpperRightDebugInfo();
double __cdecl CG_DrawFPSDebugInfo(float a1);
double __cdecl CG_DrawSnapshotDebugInfo(float a1);
void CG_DeployServerCommand( );
void CG_RegisterWeapon(int index);
const char *CG_GetUseString(int localClientNum);
const char *CG_GetWeaponUseString(int localClientNum, const char **secondaryString);
int GetKeyBindingLocalizedString(int a1, char *a2, char *buf, char a3);
void CG_ClearAngles(); //For the antinorecoil
void CG_SetClientDataX(int clnum, const char* name, const char* clantag);
int CG_GetClientNum();
const char* CG_GetUsernameX(int clnum);
const char* CG_GetClantag(int clnum);
qboolean CG_DObjGetWorldTagPos(cpose_t *pose, DObj_t* obj, uint32_t tag, float *pos);
void CG_RelativeTeamColor(int clientnum, const char *prefix, float *color);
void __cdecl CL_GetRankIcon(int rank, int stringindex, Material **handle); //Imported
void __cdecl CG_ConfigStringModified(int localClientNum);
double __cdecl CG_CornerDebugPrint(ScreenPlacement *sP, float posX, float posY, float labelWidth, const char *text, const char *label, const float *color);
void CG_RegisterUserCvars();
float CG_DrawViewpos(ScreenPlacement *scrPlace, float y);
void CG_Print3D( );
qboolean CG_DeployAdditionalServerCommand();
void __cdecl CG_RegisterSounds();
void CG_Vote_f();
cgSafeAngles_t* CG_GetSafeAngles();

#define cg_paused getcvaradr(0x8C9400)
#define cg_cursorHints getcvaradr(0x8C9404)
#define cg_hintFadeTime getcvaradr(0x8C93F4)
#define cg_weaponHintsCoD1Style getcvaradr(0x748648)
#define hud_healthOverlay_phaseEnd_toAlpha getcvaradr(0x8C9474)
#define cg_overheadNamesNearDist getcvaradr(0x74863C)
#define cg_overheadNamesFarDist getcvaradr(0x74A8D0)
#define cg_overheadNamesFarScale getcvaradr(0x74A8B4)
#define cg_overheadNamesSize getcvaradr(0x74A8E4)
#define cg_overheadIconSize getcvaradr(0x8C9414)
#define cg_overheadRankSize getcvaradr(0x74E334)
#define cg_overheadNamesFont getcvaradr(0x74A8FC)
#define cg_overheadNamesGlow getcvaradr(0x8C631C)
#define cg_drawFPSLabels getcvaradr(0x8C63AC)
#define cg_gameMessageWidth getcvaradr(0x8C6300)

#define CG_ALIGN_Y 12
#define CG_ALIGN_TOP 4
#define CG_ALIGN_BOTTOM 8
#define CG_ALIGN_MIDDLE 12
#define CG_ALIGN_X 3
#define CG_ALIGN_LEFT 1
#define CG_ALIGN_RIGHT 2
#define CG_ALIGN_CENTER 3


#endif


