#include "Utils.h"

namespace Data
{

    static void* TSLFont = nullptr;
    
    uintptr_t GName = 0;
    uintptr_t GEngine = 0;
    uintptr_t Actor = 0;
    uintptr_t Count = 0;
    uintptr_t MySelf = 0;
    uintptr_t PlayerController = 0;
    uintptr_t PlayerCameraManager = 0;
    
    std::string GetFromFName(const int index) {
        char buffer[256];
        static char ClassCache[256] = {};

        uintptr_t A = Read<uintptr_t>(GName + (index / 0x4000) * 0x8);
        uintptr_t B = Read<uintptr_t>(A + (index % 0x4000) * 0x8);
        uintptr_t data = B + 0xc;
        if (B <= 0 || index <= 0) return "None";
        
        memmove(buffer, (void*)(data), sizeof(buffer));
        buffer[255] = '\0';
        memmove(ClassCache, buffer, sizeof(buffer));
        return std::string(ClassCache);
    }

    static bool BoneToScreen(Vector3A WorldLocation, bool bPlayerViewportRelative, Vector2A *pOut) {
        if (pOut && ProjectWorldLocationToScreen != nullptr) {
            if (ProjectWorldLocationToScreen(reinterpret_cast<void *>(PlayerController), WorldLocation, *pOut, bPlayerViewportRelative)) {
                return true;
            }
        }
        return false;
    }
    
    static bool GetLineOfSightTo(Vector3A Object) {
        if (LineOfSightTo != nullptr) {
            if (LineOfSightTo(reinterpret_cast<void *>(PlayerController), reinterpret_cast<void *>(PlayerCameraManager), Object, false)) {
                return true;
            }
        }
        return false;
    }

    bool TempLockVisble = false;
    Vector3A TempLockPos;
    static bool BoneVisble(Vector3A Object) {
        float is = GetLineOfSightTo(Object) ? 1.0f : 0.0f;
        if(is == 1.0f && !TempLockVisble) {
            TempLockPos = Object;
            TempLockVisble = true;
        }
        return true;
    }
    
    Vector3A GetBoneLoc(uintptr_t Mesh, const int Id)
    {
        uintptr_t BoneActor = Read<uintptr_t>(Mesh + Off::Bone);
        FTransform lpFTransform = Read<FTransform>(BoneActor + Id * 0x30);
        FTransform ComponentToWorld = Read<FTransform>(Mesh + Off::Human);
        D3DXMATRIX Matrix = FTransform::MatrixMultiplication(lpFTransform.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
        return Vector3A(Matrix._41, Matrix._42, Matrix._43);
    }
    
    BoneData GetBoneIndex(uintptr_t Mesh) {
        BoneData Id = {};
        int BoneMaxCount = Read<int>(Mesh + Off::Bone + 0x8);
        if (BoneMaxCount == 68) {
//            Id = {6, 3, 1, 11, 33, 12, 34, 13, 35, 55, 59, 56, 60, 57, 61};
            Id = {6, 5, 1, 34, 12, 35, 13, 66, 67, 60, 56, 61, 57, 62, 58};
        } else
        if (BoneMaxCount == 65) {
            Id = {6, 5, 1, 12, 33, 13, 34, 64, 63, 53, 57, 54, 58, 55, 59};
        } else
        if (BoneMaxCount == 67) { // 训练场人机
            Id = {6, 5, 1, 35, 14, 36, 15, 65, 66, 59, 55, 60, 56, 61, 57};
        } else 
        if (BoneMaxCount == 29) { // 地铁人机
            Id = {6, 5, 1, 14, 8, 7, 9, 27, 28, 22, 19, 23, 20, 24, 21};
        } else 
        {
            Id = {6, 3, 1, 11, 33, 12, 34, 13, 35, 55, 59, 56, 60, 57, 61};
        }
        return Id;
    }
 
    uintptr_t LockObj;
    
    struct Function
    {
        bool DrawLine = true;
        bool DrawBone = true;
        bool DrawInfo = true;
        bool DrawBox = true;
        int DrawBoxStyle = 0;
        bool DrawRadar = true;
        float DrawRadarSize = 100;
        float DrawRadarX = 0;
        float DrawRadarY = 0;
        bool G_DrawBox = true;
        bool G_DrawProj = true;
        
        bool BulletTracking = true;
        int AimRange = 100;
        int ShootMode = 0;
        int HitMode = 0;
        
        bool TestFunc = false;
        int TestStyle = 0;;
    }F;
    
    bool GetDATA() {
        GEngine = Read<uintptr_t>(libUE4 + Off::GEngine);
        TSLFont = Read<uintptr_t *>(GEngine + Off::LargeFont);
        GName = Read<uintptr_t>(Read<uintptr_t>(libUE4 + Off::GName) + 0x110);

        if (GEngine) {
            uintptr_t GWorld = Read<uintptr_t>(Read<uintptr_t>(GEngine + Off::GameViewport) + Off::World);
            if (GWorld) {
            uintptr_t NetConnection = Read<uintptr_t>(Read<uintptr_t>(GWorld + Off::NetDriver) + Off::ServerConnection);
            if (NetConnection) {
                MySelf = Read<uintptr_t>(Read<uintptr_t>(NetConnection + Off::NetConnectionClassName) + Off::STExtraBaseCharacter);
                PlayerController = Read<uintptr_t>(MySelf + Off::STPlayerController);
                PlayerCameraManager = Read<uintptr_t>(PlayerController + Off::PlayerCameraManager);
        		auto ActorsArray = DecryptActorsArray(Read<uintptr_t>(GWorld + Off::PersistentLevel), 0xA0, 0x448);
                Actor = Read<uintptr_t>(ActorsArray);
                Count = Read<int>(ActorsArray + sizeof(uintptr_t));
                return true;
                }
            }
        }
        return false;
    }

    void DrawData(void* Canvas) {
        Vector2A Screen;
        Screen.X = glWidth / 2;
        Screen.Y = glHeight / 2;

        uintptr_t TempObj = 0;
        float TempDis = F.AimRange * 2;
        
        int PlayerCount = 0, BotCount = 0;

        for (int i = 0; i < Count; i++) {
            auto Obj = Read<uintptr_t>(Actor + i * 0x8);
            if (!Obj) continue;
            
            int ObjectId = Read<int>(Obj + 0x18);
            auto ClassName = GetFromFName(ObjectId);
            
            auto O_RootComp = Read<uintptr_t>(Obj + Off::RootComponent);
            auto M_RootComp = Read<uintptr_t>(MySelf + Off::RootComponent);
            if(!O_RootComp || !M_RootComp)
                continue;
            
            
            Vector2A O_Screen;
            auto O_Pos = Read<Vector3A>(O_RootComp + Off::RelativeLocation);
            auto M_Pos = Read<Vector3A>(M_RootComp + Off::RelativeLocation);
            BoneToScreen(O_Pos, false, &O_Screen);

            TempLockVisble = false;
            auto PartHitComponent = *(uintptr_t*)(MySelf + 0x1048);
            if (PartHitComponent) {
            *(int *) (PartHitComponent + 0x198 + sizeof(uintptr_t)) = 0;
        	}
            float O_Dis = GetDistance(M_Pos, O_Pos, 100.0f);
            
            int State = Read<int>(Obj + Off::State);
            
            Vector3A POVLoc = Read<Vector3A>(PlayerCameraManager + Off::CameraCache + 0x10 + 0x0);
            Vector3A ViewRotation = Read<Vector3A>(PlayerCameraManager + Off::CameraCache + 0x10 + 0x18 );
            
            if (State == 1048592 || State == 1048576 || State == 262144  || State == 262152 || State == 0)
                continue;
            if (Read<int>(Obj + Off::TeamID) == Read<int>(MySelf + Off::TeamID))
                continue;
            if (Read<float>(Obj + Off::HighWalkSpeed) == 479.5) {
        
                auto Mesh = Read<uintptr_t>(Obj + Off::Mesh);
                
                bool blsAI = Read<bool>(Obj + Off::bIsAI);
                
                if (blsAI) {
                    BotCount++;
                } else {
                    PlayerCount++;
                }
                
                BoneData Id = GetBoneIndex(Mesh);
                int BoneList[][2]
                {
                    {Id.Head, Id.Chest}, {Id.Chest, Id.Pelvis},
                    {Id.Chest, Id.Shoulder_L}, {Id.Shoulder_L, Id.Elbow_L}, {Id.Elbow_L, Id.Wrist_L},
                    {Id.Chest, Id.Shoulder_R}, {Id.Shoulder_R, Id.Elbow_R}, {Id.Elbow_R, Id.Wrist_R},
                    {Id.Pelvis, Id.Thigh_L}, {Id.Thigh_L, Id.Knee_L}, {Id.Knee_L, Id.Ankle_L},
                    {Id.Pelvis, Id.Thigh_R}, {Id.Thigh_R, Id.Knee_R}, {Id.Knee_R, Id.Ankle_R},
                };
                static int BoneCount = sizeof(BoneList) / sizeof(int[2]);
                
                Vector2A rect_0 = Vector2A();
                Vector2A rect_1 = Vector2A();
                
                std::set<float> BoneX, BoneY, BoneZ;
                
                for (int i = 0; i < BoneCount; ++i)
                {
                    auto Start = GetBoneLoc(Mesh, BoneList[i][0]);
                    auto End = GetBoneLoc(Mesh, BoneList[i][1]);
                    
                    BoneX.insert(Start.X);
                    BoneY.insert(Start.Y);
                    BoneZ.insert(Start.Z);
                    BoneX.insert(End.X);
                    BoneY.insert(End.Y);
                    BoneZ.insert(End.Z);
                    
                    if (BoneToScreen(Start, false, &rect_0) &&
                       BoneToScreen(End, false, &rect_1)) {
                       
                       if (BoneVisble(Start) && BoneVisble(End)) {
                           bool isVisible = GetLineOfSightTo(Start) && GetLineOfSightTo(End);
                           
                           FLinearColor BoneColor;
                           if (blsAI) {
                               BoneColor = DecideColor(isVisible, ZeroGUI::Colors::白色, ZeroGUI::Colors::灰色);
                           } else {
                               BoneColor = DecideColor(isVisible, ZeroGUI::Colors::绿色, ZeroGUI::Colors::红色);
                           }
                           
                           if (F.DrawBone) {
                                ZeroGUI::Draw_Line(rect_0, rect_1, 0.1f, BoneColor);
                           }
                       }
                    }
                }
                
                // 计算 8 个顶点位置
                float MinX = *min_element(BoneX.begin(), BoneX.end()) - 5.0f;
                float MinY = *min_element(BoneY.begin(), BoneY.end()) - 5.0f;
                float MinZ = *min_element(BoneZ.begin(), BoneZ.end()) - 10.0f;
                float MaxX = *max_element(BoneX.begin(), BoneX.end()) + 5.0f;
                float MaxY = *max_element(BoneY.begin(), BoneY.end()) + 5.0f;
                float MaxZ = *max_element(BoneZ.begin(), BoneZ.end()) + 15.0f;
                auto TOP_A_POS = Vector3A(MinX, MinY, MaxZ);
                auto TOP_B_POS = Vector3A(MaxX, MinY, MaxZ);
                auto TOP_C_POS = Vector3A(MinX, MaxY, MaxZ);
                auto TOP_D_POS = Vector3A(MaxX, MaxY, MaxZ);
                auto BOT_A_POS = Vector3A(MinX, MinY, MinZ);
                auto BOT_B_POS = Vector3A(MaxX, MinY, MinZ);
                auto BOT_C_POS = Vector3A(MinX, MaxY, MinZ);
                auto BOT_D_POS = Vector3A(MaxX, MaxY, MinZ);
                
                Vector2A TOP_A, TOP_B, TOP_C, TOP_D, BOT_A, BOT_B, BOT_C, BOT_D;
                
                bool ShowAllPoint = true;
                ShowAllPoint &= BoneToScreen(TOP_A_POS, false, &TOP_A);
                ShowAllPoint &= BoneToScreen(TOP_B_POS, false, &TOP_B);
                ShowAllPoint &= BoneToScreen(TOP_C_POS, false, &TOP_C);
                ShowAllPoint &= BoneToScreen(TOP_D_POS, false, &TOP_D);
                ShowAllPoint &= BoneToScreen(BOT_A_POS, false, &BOT_A);
                ShowAllPoint &= BoneToScreen(BOT_B_POS, false, &BOT_B);
                ShowAllPoint &= BoneToScreen(BOT_C_POS, false, &BOT_C);
                ShowAllPoint &= BoneToScreen(BOT_D_POS, false, &BOT_D);
                
        		auto HeadLoc = GetBoneLoc(Mesh, Id.Head);
        		auto RootLoc = GetBoneLoc(Mesh, 0);
        		Vector2A Head;
        		BoneToScreen(HeadLoc, false, &Head);
        		
                bool isVisible = GetLineOfSightTo(HeadLoc) && GetLineOfSightTo(RootLoc);
                FLinearColor DrawColor;
                if (blsAI) {
                    DrawColor = DecideColor(isVisible, ZeroGUI::Colors::白色, ZeroGUI::Colors::灰色);
                } else {
                    DrawColor = DecideColor(isVisible, ZeroGUI::Colors::绿色, ZeroGUI::Colors::红色);
                }

                if(F.DrawBox){
                    if (ShowAllPoint) {
                    if (F.DrawBoxStyle == 0) {
                    } else {
                        ZeroGUI::Draw_Line({TOP_A.X, TOP_A.Y}, {BOT_A.X, BOT_A.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_B.X, TOP_B.Y}, {BOT_B.X, BOT_B.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_C.X, TOP_C.Y}, {BOT_C.X, BOT_C.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_D.X, TOP_D.Y}, {BOT_D.X, BOT_D.Y}, 0.1f, DrawColor);
        
                        ZeroGUI::Draw_Line({TOP_A.X, TOP_A.Y}, {TOP_B.X, TOP_B.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_C.X, TOP_C.Y}, {TOP_D.X, TOP_D.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_A.X, TOP_A.Y}, {TOP_C.X, TOP_C.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({TOP_B.X, TOP_B.Y}, {TOP_D.X, TOP_D.Y}, 0.1f, DrawColor);
        
                        ZeroGUI::Draw_Line({BOT_A.X, BOT_A.Y}, {BOT_B.X, BOT_B.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({BOT_C.X, BOT_C.Y}, {BOT_D.X, BOT_D.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({BOT_A.X, BOT_A.Y}, {BOT_C.X, BOT_C.Y}, 0.1f, DrawColor);
                        ZeroGUI::Draw_Line({BOT_B.X, BOT_B.Y}, {BOT_D.X, BOT_D.Y}, 0.1f, DrawColor);
                    }
                    }
                }

                if (F.DrawLine) {
                    ZeroGUI::Draw_Line({Screen.X, 0}, {Head.X, Head.Y - 65}, 0.1f, DrawColor);
                }
                
                if (F.DrawInfo) {
                    int BoneMaxCount = Read<int>(Mesh + Off::Bone + 0x8);
                    int TeamID = Read<int>(Obj + Off::TeamID);
                    std::string Xi;
                    char PlayerName[64];
                    if (blsAI) {
                        Xi = to_string((int)TeamID) + " : Bot";
                    } else {
                        getUTF8(PlayerName, Read<uintptr_t>(Obj + Off::PlayerName));
//                        Xi = to_string((int)TeamID) + " : " + PlayerName;
                        Xi = to_string((int)TeamID) + " : " + GetPlayerName(Obj);
                    }
                    ZeroGUI::TextOutlined(Xi.c_str(), {Head.X, Head.Y - 40}, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);

                    float Health = Read<float>(Obj + Off::Health);
                    float HealthMax = Read<float>(Obj + Off::HealthMax);

                    auto mWidth = 64;
                    auto mHeight = mWidth * 0.07f;
                    
                    Head.X -= (mWidth / (2 * 5)) * 5;
                    Head.Y -= (mHeight * 1.55f);
                    
                    ZeroGUI::DrawFilledRect({Head.X, Head.Y - 50}, (Health * mWidth / HealthMax), mHeight, ZeroGUI::Colors::红色);
                    
                    for (int i = 0; i < 5; ++i) {
                        ZeroGUI::DrawRectangle({Head.X + 16 * i, Head.Y - 50}, mWidth - 16 * i, mHeight, 1.3f, {0, 0, 0, 1.f});
                    }
                }
                
                if (F.DrawRadar) {
                    Vector2A RadarSize = {F.DrawRadarSize, F.DrawRadarSize};
                    Vector3A RadarPionts = WorldToRadar(ViewRotation.Y,O_Pos, M_Pos, F.DrawRadarX, F.DrawRadarY, F.DrawRadarSize);

                    ZeroGUI::Draw_Line({F.DrawRadarX - 90.0f, F.DrawRadarY}, Vector2A(F.DrawRadarX + 90.0f, F.DrawRadarY), 1.0f, ZeroGUI::Colors::白色);
                    ZeroGUI::Draw_Line({F.DrawRadarX, F.DrawRadarY - 90.0f}, Vector2A(F.DrawRadarX, F.DrawRadarY + 90.0f), 1.0f, ZeroGUI::Colors::白色);
                    ZeroGUI::DrawRectangle(RadarSize, F.DrawRadarX, F.DrawRadarY, 0.1, ZeroGUI::Colors::白色);
                    
                    ZeroGUI::DrawFilledRect({ F.DrawRadarX, F.DrawRadarY }, 5, 5, ZeroGUI::Colors::白色);
                    ZeroGUI::DrawFilledRect({ RadarPionts.X, RadarPionts.Y }, 5, 5, DrawColor);
                }
                
                if (F.BulletTracking) {
                    auto Mesh = Read<uintptr_t>(Obj + Off::Mesh);
                    Vector2A AimScreen;
                    Vector3A AimPos;
                    if (F.HitMode == 0) {
                        Vector3A HeadPos = GetBoneLoc(Mesh, 6);
                        if (GetLineOfSightTo(HeadPos)) {
                            AimPos = HeadPos;
                        } else {
                            AimPos = TempLockVisble ? TempLockPos : HeadPos;
                        }
                    } else {
                        Vector3A ChestPos = GetBoneLoc(Mesh, 4);
                        if (GetLineOfSightTo(ChestPos)) {
                            AimPos = ChestPos;
                        } else {
                            AimPos = TempLockVisble ? TempLockPos : ChestPos;
                        }
                    }

                    uintptr_t WeaponEntityComp = Read<uintptr_t>(Read<uintptr_t>(MySelf + Off::AnimStatusKeyList) + Off::WeaponEntity);
                    float BulletFireSpeed = Read<float>(WeaponEntityComp + Off::BulletFireSpeed);
                    float secFlyTime = GetDistance(POVLoc, AimPos, BulletFireSpeed) * 1.2;
                    
                    Vector3A Velocity;
                    long int CurrentVehicle = Read<uintptr_t>(Obj + Off::CurrentVehicle);
                    long int Parachute = Read<uintptr_t>(Obj + Off::bParachute);
                    if (Parachute) {
                        Vector3A ParachuteVelocity = Read<Vector3A>(Read<uintptr_t>(Obj + Off::ParachuteComponent) + Off::ParachuteVelocity);
                        Velocity = ParachuteVelocity;
                    } else if (CurrentVehicle){
                        Vector3A LinearVelocity = Read<Vector3A>(CurrentVehicle + Off::RepMovement);
                        Velocity = LinearVelocity;
                    }else{
                        Vector3A ComponentVelocity = Read<Vector3A>(Read<uintptr_t>(Obj + Off::RootComponent) + Off::ComponentVelocity);
                        Velocity = ComponentVelocity;
                    }
                    if (F.ShootMode == 0) {
                    AimPos.X += Velocity.X * secFlyTime;
                    AimPos.Y += Velocity.Y * secFlyTime;
                    AimPos.Z += Velocity.Z * secFlyTime;
                    }
                    
                    BoneToScreen(AimPos, false, &AimScreen);
                    bool isVisible = GetLineOfSightTo(AimPos);
                    
                    float XDis = abs(Screen.X - AimScreen.X);
	                float YDis = abs(Screen.Y - AimScreen.Y);
            
	                if(XDis <= F.AimRange && YDis <= F.AimRange){
	                    float Dis = (XDis + YDis) / 2;
	                    if(Dis < TempDis){
	                        TempDis = Dis;
	                        TempObj = Obj;
	                    }
	                }
            
	                if(LockObj == Obj && F.BulletTracking){
	                    if (isVisible && AimPos != Vector3A()) {
	                    TargetPos = AimPos;
	                    ZeroGUI::Draw_Line(Screen, AimScreen, 0.1f, FLinearColor(1.f, 1.f, 1.f, 1.f));
	                    }
	                }
                }
            } else {
                if (O_Dis < 151) {
                if (F.G_DrawBox) {
                    std::string G_BoxName;
                    if (strstr(ClassName.c_str(), "Box") || strstr(ClassName.c_str(), "PickUpListWrapperActor")) {
                    G_BoxName = "盒子 (" + std::to_string((int)O_Dis) + "m)";
                    ZeroGUI::TextOutlined(G_BoxName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    }
                }
                if (F.G_DrawProj) {
                    std::string G_ProjName;
                    if (strstr(ClassName.c_str(), "BP_Grenade_Shoulei_C")) {
                    G_ProjName = "手雷 (" + std::to_string((int)O_Dis) + "m)";
                    ZeroGUI::TextOutlined(G_ProjName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    }
                    if (strstr(ClassName.c_str(), "BP_Grenade_Burn_C")) {
                    G_ProjName = "燃烧瓶 (" + std::to_string((int)O_Dis) + "m)";
                    ZeroGUI::TextOutlined(G_ProjName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    }
                    if (strstr(ClassName.c_str(), "BP_Grenade_Smoke_C")) {
                    G_ProjName = "烟雾弹 (" + std::to_string((int)O_Dis) + "m)";
                    ZeroGUI::TextOutlined(G_ProjName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    }
                    if (strstr(ClassName.c_str(), "BP_Grenade_Tun_Weapon_Wrapper_C")) {
                    G_ProjName = "闪光弹 (" + std::to_string((int)O_Dis) + "m)";
                    ZeroGUI::TextOutlined(G_ProjName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    }
                }
                }
            }
            
            if (F.TestFunc) {
                std::string Xi;
                Vector2A TestScreen;
//                if (Read<float>(Obj + Off::HighWalkSpeed) == 479.5) {
                    auto Mesh = Read<uintptr_t>(Obj + Off::Mesh);
                    Vector3A TestPos = GetBoneLoc(Mesh, 6);
                    int BoneMaxCount = Read<int>(Mesh + Off::Bone + 0x8);
                    Xi = "当前人物骨骼数量: " + to_string((int)BoneMaxCount);
                    BoneToScreen(TestPos, false, &TestScreen);
 //               }
                if (F.TestStyle == 0) {

                    ZeroGUI::TextOutlined(Xi.c_str(), {TestScreen.X - 20, TestScreen.Y - 40}, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                    ZeroGUI::TextOutlined(ClassName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                }
                if (F.TestStyle == 1) {
                    ZeroGUI::TextOutlined(Xi.c_str(), {TestScreen.X - 20, TestScreen.Y - 40}, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                }
                if (F.TestStyle == 2) {
                ZeroGUI::TextOutlined(ClassName.c_str(), O_Screen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                }
                if (F.TestStyle == 3) {
                int Boneid[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
                40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 
                59, 60, 61, 62, 63, 64, 65, 66, 67, 68};

                std::string id;
                auto Mesh = Read<uintptr_t>(Obj + Off::Mesh);
                for (int i = 0; i < 68; ++i)
                {
                    Vector2A BoneidScreen;
                    auto BoneIdPOS = GetBoneLoc(Mesh, Boneid[i]);
                    BoneToScreen(BoneIdPOS, false, &BoneidScreen);
                    id = to_string((int)Boneid[i]);
                    ZeroGUI::TextOutlined(id.c_str(), BoneidScreen, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
                }
                }
            }
        } // For ++Count
        
       ZeroGUI::DrawCircle(Screen, F.AimRange, 10, ZeroGUI::Colors::白色);
        
       string ACount = "P:" + to_string((int)PlayerCount) + " " + "B:" + to_string((int)BotCount);
       ZeroGUI::TextOutlined(ACount.c_str(), {Screen.X , Screen.Y / 8}, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
       
       LockObj = TempObj;

        // string sb = "FPS: " + to_string((int)(Read<int>(libUE4 + Off::GFPS)));
        // ZeroGUI::TextOutlined(sb.c_str(), {Screen.X - 380, Screen.Y / 18}, FLinearColor(1.f, 1.f, 1.f, 1.f), FLinearColor(0, 0, 0, 1.f), true);
    }
    
    void (*orig_PostRender)(void* Viewport, void* Canvas);
    void new_PostRender(void* Viewport, void* Canvas) {
    
        if (Canvas) {
            glWidth = Read<int>((uintptr_t)Canvas + 0x40);
            glHeight = Read<int>((uintptr_t)Canvas + 0x44);
            Data::GetDATA();
            
            if (TSLFont) {
            
                ZeroGUI::SetupCanvas(Canvas, TSLFont);
                Data::DrawData(Canvas);
                
                static Vector2A Pos = { 300, 100 };
                static bool IsOpen = false;
                static float TempValue = 0;
                static int test_number = 0;
            
                if (ZeroGUI::Window("EngineEsp", &Pos, { 400.0f, 500.0f }, IsOpen, TempValue)) {
                    static int tab = 0;
                    if (ZeroGUI::ButtonTab("绘制", Vector2A{ 100, 35 }, tab == 0)) tab = 0;
                    if (ZeroGUI::ButtonTab("追踪", Vector2A{ 100, 35 }, tab == 1)) tab = 1;
                    if (ZeroGUI::ButtonTab("测试", Vector2A{ 100, 35 }, tab == 2)) tab = 2;
                    ZeroGUI::NextColumn(110.0f);
                    
                    if (tab == 0) {                   
                        ZeroGUI::Text("esp");
                        ZeroGUI::Checkbox("射线", &F.DrawLine);
                        ZeroGUI::SameLine();
                        ZeroGUI::Checkbox("骨骼", &F.DrawBone);
                        ZeroGUI::Checkbox("信息", &F.DrawInfo);
                        ZeroGUI::SameLine();
                        ZeroGUI::Checkbox("方框", &F.DrawBox);
                        ZeroGUI::Checkbox("盒子", &F.G_DrawBox);
                        ZeroGUI::SameLine();
                        ZeroGUI::Checkbox("投掷物", &F.G_DrawProj);
                        ZeroGUI::Checkbox("雷达", &F.DrawRadar);
                		ZeroGUI::PushNextElementY(20.0f);
                		ZeroGUI::Combobox("方框样式", Vector2A{ 80, 25 }, &F.DrawBoxStyle, "方框2D[超薄]", "方框 3D", NULL);
                        ZeroGUI::SliderFloat("雷达大小", &F.DrawRadarSize, 20, 500);
                        ZeroGUI::SliderFloat("雷达X轴", &F.DrawRadarX, 0, glHeight);
                        ZeroGUI::SliderFloat("雷达Y轴", &F.DrawRadarY, 0, glWidth);
                        
                    }
                    if (tab == 1) {
                        ZeroGUI::Text("测试");
                        ZeroGUI::Checkbox("追踪", &F.BulletTracking);
                        ZeroGUI::SliderInt("AimRange", &F.AimRange, 20, 1024);
                		ZeroGUI::Combobox("射击方式", Vector2A{ 80, 25 }, &F.ShootMode, "弹道", "坐标", NULL);
                        ZeroGUI::SameLine();		
                		ZeroGUI::Combobox("打击部位", Vector2A{ 80, 25 }, &F.HitMode, "头", "胸", NULL);
                    }
                    if (tab == 2) {
                        ZeroGUI::Text("测试");
                        ZeroGUI::Checkbox("测试功能", &F.TestFunc);
                        ZeroGUI::SameLine();
                		ZeroGUI::Combobox("选项", Vector2A{ 80, 25 }, &F.TestStyle, "全部", "骨骼数量", "类名", "骨骼id", NULL);
                    }
                }
            }
        }
        return orig_PostRender(Viewport, Canvas);
    }
    
    void (*orig_ShootEvent)(void *thiz, Vector3A start, FRotator rot, void *weapon, int unk1) = nullptr;
    void ShootEvent(void *thiz, Vector3A start, FRotator rot, void *weapon, int unk1) {
        if (F.BulletTracking && LockObj && TargetPos != Vector3A()) {
            rot = ToRotator(start, TargetPos);
            if (F.ShootMode != 0) {
            start = TargetPos;
            }
        }
        return orig_ShootEvent(thiz, start, rot, weapon, unk1);
    }
    
    
    void LoadFunc(uintptr_t Module) {
        // Get Function Ptr
        // UCanvas *Canvas
        *(uintptr_t *) &K2_DrawLine = Module + Off::K2_DrawLine;
        *(uintptr_t *) &K2_DrawText = Module + Off::K2_DrawText;
        *(uintptr_t *) &K2_DrawTexture = Module + Off::K2_DrawTexture;
        *(uintptr_t *) &ProjectWorldLocationToScreen = Module + Off::ProjectWorldLocation;
        *(uintptr_t *) &LineOfSightTo = Module + Off::LineOfSightTo;
        
        A64HookFunction((void*)(Module + Off::GPostRender), (void*)new_PostRender, (void**)&orig_PostRender);
        A64HookFunction((void*)(Module + Off::ShootEvent), (void*)ShootEvent, (void**)&orig_ShootEvent);
    }
    
};