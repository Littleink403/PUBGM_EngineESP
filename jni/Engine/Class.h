#pragma once


namespace Off {
    int GameViewport = 0x810; // GameViewportClient* GameViewport;
    int World = 0x78; // Class: GameViewportClient.ScriptViewportClient.Object -> World* World;
    int PersistentLevel = 0x30; // Class: World.Object -> Level* PersistentLevel;
    
    int NetDriver = PersistentLevel + 0x8; // Class: World.Object -> NetDriver* NetDriver;
    int ServerConnection = 0x78;// NetConnection* ServerConnection;
    int NetConnectionClassName = 0x30; // FString NetConnectionClassName;
    int STExtraBaseCharacter = 0x2698; // STExtraBaseCharacter* STExtraBaseCharacter;
    int STPlayerController = 0x47e8; // STExtraPlayerController* STPlayerController;
    int PlayerCameraManager = 0x4d0; // PlayerCameraManager* PlayerCameraManager;
	int CameraCache = 0x4b0; // CameraCacheEntry CameraCache
	
    int Mesh = 0x498; // SkeletalMeshComponent* Mesh
    int Human = 0x1b0; // Vector RelativeScale3D; + 0x3C
    int Bone = 0x878; // Class: StaticMeshComponent.MeshComponent.PrimitiveComponent.SceneComponent.ActorComponent.Object -> StaticMesh* StaticMesh
    
	int RootComponent = 0x1b0; // SceneComponent* RootComponent
	int RelativeLocation = 0x1c0;// Vector K2_GetActorLocation();
	int ComponentVelocity = 0x260; // Vector ComponentVelocity;

	int CurrentVehicle = 0xdf8; // STExtraVehicleBase* CurrentVehicle
	int RepMovement = 0xb0; // RepMovement ReplicatedMovement
	int bParachute = 0x3028; // bool bReplicatedIsStartParachute;
    int ParachuteComponent = 0x1670; // CharacterParachuteComponent* ParachuteComponent;
	int ParachuteVelocity = 0x350; // Vector GetParachuteLandingVelocity();
	
    int HighWalkSpeed = 0x2970; // float HighWalkSpeed;
    int bIsAI = 0x9d9; // 和平 bool bIsAI; | 国际服 bool bEnsure;
    int TeamID = 0x928; // Class: UAECharacter.Character.Pawn.Actor.Object -> int TeamID;
    int State = 0xfa0; // Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> 和平 ReplicatedPawnStates PawnStateRepSyncData; | 国际服 uint64 CurrentStates;
    int Health = 0xdb0; // Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> float Health;
    int HealthMax = Health + 0x4; // Class: STExtraCharacter.UAECharacter.Character.Pawn.Actor.Object -> float HealthMax;
    int PlayerName = 0x888; // Class: UAECharacter.Character.Pawn.Actor.Object -> FString PlayerName;
    int bIsWeaponFiring = 0x1688; // bool bIsWeaponFiring;
    
	int STExtraWeapon = 0x20; // STExtraWeapon* EquipWeapon;
    int AnimStatusKeyList = 0x28d8 + STExtraWeapon; // AnimStatusKeyList LastUpdateStatusKeyList;
	int WeaponEntity = 0x840; // WeaponEntity* WeaponEntityComp;
	int BulletFireSpeed = 0x508; // float BulletFireSpeed;
	
    int LargeFont = 0xb0; // Font* LargeFont;
 
    int GEngine = 0xDE49DE8;
    int GName = 0xD66B2C0;
    int ProjectWorldLocation = 0x9B822B0;
    int LineOfSightTo = 0x9756EA8;
    int K2_DrawLine = 0x9E25AC4;
    int K2_DrawText = 0x9E25D54;
    int K2_DrawTexture = 0x9E25B48;
    int GPostRender = 0x985D218;
    int GNativeApp = 0xD876FD0;
    int ShootEvent = 0x6111528;
};

