#include "Class.h"
#include "Struct.h"

uintptr_t libUE4;

using namespace std;

Vector3A TargetPos;
    
void (*K2_DrawLine)(void* Canvas, Vector2A ScreenPositionA, Vector2A ScreenPositionB, float Thickness, FLinearColor RenderColor);
void (*K2_DrawText)(void* Canvas, void* RenderFont, const FString & Text, Vector2A ScreenPosition, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, Vector2A ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor);
void (*K2_DrawTexture)(void* Canvas, void* RenderTexture, Vector2A ScreenPosition, Vector2A ScreenSize, Vector2A CoordinatePosition, Vector2A CoordinateSize, FLinearColor RenderColor, int BlendMode, float Rotation, Vector2A PivotPoint);
static bool (*ProjectWorldLocationToScreen)(void *PlayerController, Vector3A WorldLocation,Vector2A &ScreenLocation, bool bPlayerViewportRelative) = nullptr;
static bool (*LineOfSightTo)(void *Controller, void *Actor, Vector3A ViewPoint, bool bAlternateChecks) = nullptr;

std::string GetPlayerName(uintptr_t Pawn) {
    uintptr_t nameAddress = Read<uintptr_t>(Pawn + Off::PlayerName);
    unsigned short buf16[64] = {0};
    memoryRead(nameAddress, buf16, sizeof(buf16));

    // 将 UTF-16 转换为 UTF-8
    std::string playerName;
    for (int i = 0; i < 64 && buf16[i] != 0; ++i) {
        if (buf16[i] <= 0x007F) {
            playerName += static_cast<char>(buf16[i]);
        } else if (buf16[i] <= 0x07FF) {
            playerName += static_cast<char>(0xC0 | (buf16[i] >> 6));
            playerName += static_cast<char>(0x80 | (buf16[i] & 0x3F));
        } else {
            playerName += static_cast<char>(0xE0 | (buf16[i] >> 12));
            playerName += static_cast<char>(0x80 | ((buf16[i] >> 6) & 0x3F));
            playerName += static_cast<char>(0x80 | (buf16[i] & 0x3F));
        }
    }
    return playerName;
}


float GetDistance(Vector3A self, Vector3A object, float divice) {
    Vector3A xyz;
    xyz.X = self.X - object.X;
    xyz.Y = self.Y - object.Y;
    xyz.Z = self.Z - object.Z;
    return sqrt(pow(xyz.X, 2) + pow(xyz.Y, 2) + pow(xyz.Z, 2)) / divice;
}

FLinearColor DecideColor(bool Conditional, FLinearColor AColor, FLinearColor BColor) {
    if (Conditional) {
        return AColor;
    } else {
        return BColor;
    }
    return AColor;
}

static FRotator ToRotator(Vector3A local, Vector3A target) {
    Vector3A rotation;
    rotation.X = local.X - target.X;
    rotation.Y = local.Y - target.Y;
    rotation.Z = local.Z - target.Z;
    FRotator newViewAngle = {0,0,0};

    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);

    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Roll = (float)0.f;

    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;

    return newViewAngle;
}


Vector3A WorldToRadar(float Yaw, Vector3A Origin, Vector3A LocalOrigin, float PosX, float PosY, float DrawSize) {
    Vector2A Size = {DrawSize, DrawSize};
    bool flag = false;
    bool outbuff = false;

    double radianYaw = Yaw * 0.017453292519943295; // 将角度转为弧度
    float cosYaw = std::cos(radianYaw);
    float sinYaw = std::sin(radianYaw);

    // 计算玩家和敌人之间的相对位置
    float deltaX = Origin.X - LocalOrigin.X;
    float deltaY = Origin.Y - LocalOrigin.Y;

    // 计算在雷达上的相对位置
    Vector3A Xector;
    Xector.X = (deltaY * cosYaw - deltaX * sinYaw) / 150.0f;
    Xector.Y = (deltaX * cosYaw + deltaY * sinYaw) / 150.0f;

    const float halfSizeX = Size.X * 0.5f;
    const float halfSizeY = Size.Y * 0.5f;

    // 将计算得出的相对位置转换到雷达上的实际位置
    Vector3A Xector2;
    Xector2.X = std::clamp(Xector.X + PosX + halfSizeX, PosX, PosX + Size.X);
    Xector2.Y = std::clamp(-Xector.Y + PosY + halfSizeY, PosY, PosY + Size.Y);

    flag = (Xector2.X == PosX || Xector2.X == PosX + Size.X || 
            Xector2.Y == PosY || Xector2.Y == PosY + Size.Y);

    outbuff = flag;

    // 创建一个 Vector3A 实例并赋值
    Vector3A radarPosition;
    radarPosition.X = Xector2.X;
    radarPosition.Y = Xector2.Y;
    radarPosition.Z = 0.0f; // 或根据需要设置

    return radarPosition;
}


