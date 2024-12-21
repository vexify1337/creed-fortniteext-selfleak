#pragma once
namespace gamesdk {
#define UWORLD 0x13743D18 //UWORLD/GWORLD
#define GAME_INSTANCE 0x1f0 //GAMEINSTANCE
#define LOCAL_PLAYERS 0x38 //LOCALPLAYERS
#define PLAYER_CONTROLLER 0x30 //PLAYERCONTROLLER
#define LOCAL_PAWN 0x350 //LOCALPAWN //AACTOR
#define PAWN_PRIVATE 0x320 //PAWNPRIVATE /ACCTOR I THINK
#define ROOT_COMPONENT 0x1B0 //ROOTC /ROOTCOMPONENT //0x1B0
#define RELATIVE_LOCATION 0x138 //RELATIVELOCATION //0x120
#define PLAYER_STATE 0x2C8 //PLAYERSTATE //DONTREMEBER WHERE IT IS IN SDK //0x2B0
#define TEAM_INDEX 0x1261 //DONTREMEBER WHERE IT IS IN SDK //0x1239
#define GAME_STATE 0x178 //UPD
#define PLAYER_ARRAY 0x2C0 //PLAYERARRAY
#define MESH 0x328 //UPD
#define COMPONENT_TO_WORLD 0x1E0 //230
#define BONE_ARRAY 0x5A8 //UPD	
#define BONE_ARRAY_CACHE 0x590 //DONTREMEBER WHERE IT IS IN SDK
#define LAST_SUBMIT_TIME 0x2E8 //DONTREMEBER WHERE IT IS IN SDK //0x2EC //0x2E0  //0x2E8
#define LAST_SUBMIT_TIME_ON_SCREEN 0x2E0 //DONTREMEBER WHERE IT IS IN SDK //0X2E8
#define VELOCITY 0x168 //UPD

}
//ALL OFFSETS HERE!
//THATS ALL THE OFFSETS YOU NEED FOR THAT SIMPLE ASS BASE
//IF YOU ADD MORE STUFF KEEP IN MIND THAT YOU MIGHT NEED MORE OFFSETS !
//LAST UPDATED WAS 6/12/2024 (TODAY)




namespace Offsets {
	uintptr_t GameWorld = 0x13743D18; // (UWorld) // fresh dump
	uintptr_t PersistentLevel = 0x40;
	uintptr_t OwningWorld = 0xc0;
	uintptr_t HabaneroComponent = 0x9E8;// fresh dump
	uintptr_t GObjects = 0x12E4DF88;  // fresh dump
	uintptr_t GNames = 0x12EFC740;// fresh dump
	uintptr_t FName = 0x124DE7C0 + 8;
	uintptr_t StaticFindObject = 0x1DE185C;
	uintptr_t GetBoneMatrix = 0x1E4E8E6;
	uintptr_t LineOfSightTo = 0x627FB84;
	uintptr_t ProjectileSpeed = 0x1F30;
	uintptr_t ProjectileGravity = 0x1F34;
	uintptr_t PlayerName = 0xAF8;
	uintptr_t Viewpoint = 0x10F90A10;
	uintptr_t DrawTransition = 0x71;
	uintptr_t ProcessEvent = 0x4D;
	uintptr_t Fire = 0x285D0DC;
	uintptr_t Malloc = 0xC4663D;
	uintptr_t  platform = 0x3F0;
	uintptr_t Free = 0xC4668D;
	uintptr_t LocalPawn = 0x350;
	uintptr_t PlayerController = 0x30;
	uintptr_t LocalPlayers = 0x38;
	uintptr_t PlayerState = 0x2C8;
	uintptr_t GameState = 0x178;
	uintptr_t GameInstance = 0x1f0;
	uintptr_t PlayerArray = 0x2C0;
	uintptr_t TeamIndex = 0x1261;
	uintptr_t RootC = 0x1b0;
	uintptr_t CameraManager = 0x348;
	uintptr_t CachedActor = 0x308;
	uintptr_t RootComponent = 0x1b0;
	uintptr_t ActorCount = 0xA8;
	uintptr_t BoneArray = 0x5A8;
	uintptr_t BoneCache = 0x590;// fresh dump
	uintptr_t C2W = 0x1c0;
	uintptr_t Actor = 0xA0; // sdk offset
	uintptr_t ActorMesh = 0x328; // sdk offset
	uintptr_t AActor = 0xA8;// sdk offset

	uintptr_t RelativeLocation = 0x138;// sdk offset
	uintptr_t DisplayName = 0x90;// sdk offset
	uintptr_t ComponetToWorld = 0x1E0;// sdk offset
	uintptr_t PawnPrivate = 0x320;// sdk offset
	uintptr_t LastRenderTimeOnScreen = 0x2E0; // sdk offset
	uintptr_t LastSubmitTime = 0x2E8; // sdk offset
	uintptr_t Velocity = 0x168;// sdk offset
	uintptr_t DefaultFOV = 0x2a4;// sdk offset
	uintptr_t CurrentWeapons = 0x9f8;// sdk offset
	uintptr_t bIsReloadingWeapon = 0x399;// sdk offset
	uintptr_t bIsEquippingWeapon = 0x350; 
	uintptr_t bIsChargingWeapon = 0x369;// sdk offset
	uintptr_t WeaponData = 0x568;// sdk offset
	uintptr_t AmmoCount = 0xef4;// sdk offset
	uintptr_t bIsTargeting = 0x581;// sdk offset
	uintptr_t GunName = 0x98;// sdk offset
	uintptr_t Tier = 0x9A;// sdk offset
	uintptr_t bIsCrouched = 0x45C;// sdk offset
	uintptr_t bIsDying = 0x758;// sdk offset
	uintptr_t bIsHiddenForDeath = 0x758;// sdk offset
	uintptr_t bIsKnockedback = 0x758;// sdk offset
	uintptr_t bIsStaggered = 0x759; // sdk offset
	uintptr_t bIsInvulnerable = 0x75A;// sdk offset
	uintptr_t bSpotted = 0x75A;// sdk offset
	uintptr_t bIsSliding = 0x75B;// sdk offset
	uintptr_t bWeaponHolstered = 0x939;// sdk offset
	uintptr_t bIsDBNO = 0x93A;
	uintptr_t LastRevivedFromDBNOTime = 0x4C30;
	uintptr_t bIsJumping = 0xA60;
	uintptr_t bIsABot = 0x29A;

	uintptr_t GlobalAnimRateScale = 0xA18;
	uintptr_t CustomTimeDilation = 0x68;
}