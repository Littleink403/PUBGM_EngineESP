#pragma once

#include <string>
#include <codecvt>

struct Vector2A {
    float X;
    float Y;

    Vector2A() {
        this->X = 0;
        this->Y = 0;
    }

    Vector2A(float x, float y) {
        this->X = x;
        this->Y = y;
    }
    
    bool operator!=(const Vector2A& other) const {
        return X != other.X || Y != other.Y;
    }
};

struct Vector3A {
    float X;
    float Y;
    float Z;

    Vector3A() {
        this->X = 0;
        this->Y = 0;
        this->Z = 0;
    }

    Vector3A(float x, float y, float z) {
        this->X = x;
        this->Y = y;
        this->Z = z;
    }
    
    Vector3A operator-(const Vector3A& other) const {
        return Vector3A(X - other.X, Y - other.Y, Z - other.Z);
    }
    
    bool operator!=(const Vector3A& other) const {
        return X != other.X || Y != other.Y || X != other.Z;
    }

};

struct Vector4A {
    float X;
    float Y;
    float Z;
    float W;
    
    Vector4A() {
        this->X = 0;
        this->Y = 0;
        this->Z = 0;
        this->W = 0;        
    }

    Vector4A(float x, float y, float z, float w) {
        this->X = x;
        this->Y = y;
        this->Z = z;
        this->W = w;        
    }    
};

struct FLinearColor
{
	float R;
	float G;
	float B;
	float A;

    FLinearColor() {
        this->R = 0;
        this->G = 0;
        this->B = 0;
        this->A = 0;        
    }

    FLinearColor(float r, float g, float b, float a) {
        this->R = r;
        this->G = g;
        this->B = b;
        this->A = a;        
    }			
}; 

struct FRotator
{
    float Pitch;
    float Yaw;
    float Roll;
};

template<typename T>
struct TArray {
	struct FString;
	T * Data;
	int32_t Count;
	int32_t Max;
	T & operator[](int idx) {
		return Data[idx];
	}
	T & operator[](int idx) const {
		return Data[idx];
	}
};


struct FString : private TArray<unsigned short>
{
    FString() {
    }

    FString(const std::wstring s)
    {
        Max = Count = !s.empty() ? (s.length() * 2) + 1 : 0;
        if (Count)
        {
            Data = (unsigned short *)(s.data());
        }
    }

    FString(const wchar_t *s) : FString(std::wstring(s)) {
    }

    FString(const wchar_t *s, int len) : FString(std::wstring(s, s + len)) {
    }

    FString(const std::string s) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring ws = converter.from_bytes(s);

        Max = Count = !ws.empty() ? (ws.length() * 2) + 1 : 0;
        if (Count)
        {
            Data = (unsigned short *)(ws.data());
        }
    }

    FString(const char *s) : FString(std::string(s)) {
    }

    FString(const char *s, int len) : FString(std::string(s, s + len)) {
    }

    bool IsValid() const
    {
        return Data != nullptr;
    }

    const wchar_t* ToWString() const
    {
        wchar_t *output = new wchar_t[Count + 1];

        for (int i = 0; i < Count; i++) {
            const char16_t uc = Data[i];
            if (uc - 0xd800u >= 2048u) {
                output[i] = uc;
            } else {
                if ((uc & 0xfffffc00) == 0xd800 && (uc & 0xfffffc00) == 0xdc00)
                    output[i] = (uc << 10) + Data[i] - 0x35fdc00;
                else
                    output[i] = L'?';
            }
        }

        output[Count] = 0;
        return output;
    }

   const char* ToString() const
    {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
        return convert.to_bytes(std::u16string(Data, Data + Count)).c_str();
    }
};


struct D3DXMATRIX
{
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};
    
struct FTransform
{
	Vector4A rot;
	Vector3A translation;
	float timestamp;
	Vector3A scale;
	
	Vector4A Rotation;
	Vector3A Translation;
	float chunk;
	Vector3A Scale3D;
	D3DXMATRIX ToMatrixWithScale()
	{
		D3DXMATRIX m;
		m._41 = translation.X;
		m._42 = translation.Y;
		m._43 = translation.Z;

		float x2 = rot.X + rot.X;
		float y2 = rot.Y + rot.Y;
		float z2 = rot.Z + rot.Z;

		float xx2 = rot.X * x2;
		float yy2 = rot.Y * y2;
		float zz2 = rot.Z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.X;
		m._22 = (1.0f - (xx2 + zz2)) * scale.Y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.Z;

		float yz2 = rot.Y * z2;
		float wx2 = rot.W * x2;
		m._32 = (yz2 - wx2) * scale.Z;
		m._23 = (yz2 + wx2) * scale.Y;

		float xy2 = rot.X * y2;
		float wz2 = rot.W * z2;
		m._21 = (xy2 - wz2) * scale.Y;
		m._12 = (xy2 + wz2) * scale.X;

		float xz2 = rot.X * z2;
		float wy2 = rot.W * y2;
		m._31 = (xz2 + wy2) * scale.Z;
		m._13 = (xz2 - wy2) * scale.X;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
	static D3DXMATRIX MatrixMultiplication(D3DXMATRIX pM1, D3DXMATRIX pM2) {

		D3DXMATRIX pOut;
		pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
		pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
		pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
		pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
		pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
		pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
		pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
		pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
		pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
		pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
		pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
		pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
		pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
		pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
		pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
		pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

		return pOut;
	}
};

struct BoneData {
    int Head;           // Õ∑≤ø
    int Chest;           // –ÿ≤ø
    int Pelvis;           // ≈Ëπ«
    int Shoulder_L;      // ◊ÛºÁ∞Ú
    int Shoulder_R;      // ”“ºÁ∞Ú
    int Elbow_L;         // ◊Û ÷÷‚
    int Elbow_R;         // ”“ ÷÷‚
    int Wrist_L;          // ◊Û ÷ÕÛ
    int Wrist_R;          // ”“ ÷ÕÛ
    int Thigh_L;          // ◊Û¥ÛÕ»
    int Thigh_R;          // ”“¥ÛÕ»
    int Knee_L;           // ◊Ûœ•∏«
    int Knee_R;           // ”“œ•∏«
    int Ankle_L;           // ◊ÛΩ≈ÕÛ
    int Ankle_R;           // ”“Ω≈ÕÛ
};


//6, 5, 1, 14, 15, 8, 9, 27, 28, 22, 19, 23, 20, 24, 21
