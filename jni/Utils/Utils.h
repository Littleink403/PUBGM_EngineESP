#include "MemoryTools.h"
#include "Hook64.h"
#include "Engine.h"
#include "NativeApp.h"
#include "ZeroGUI.h"

float glWidth, glHeight;

Vector2A GetGameScreen()
{
    Vector2A Size;
    Size.X = ANativeWindow_getWidth(g_App->window);
    Size.Y = ANativeWindow_getHeight(g_App->window);
    return Size;
}


int (*orig_onInputEvent)(struct android_app *app, AInputEvent *inputEvent);
int onInputEvent(struct android_app *app, AInputEvent *inputEvent)
{
    static Vector2A CSize = GetGameScreen();
    ZeroGUI::onEvent(inputEvent, {(float)CSize.X / (float)glWidth, (float)CSize.Y / (float)glHeight}); 
    return orig_onInputEvent(app, inputEvent);
}

typedef char UTF8;
typedef unsigned short UTF16;

void getUTF8(UTF8 * buf, unsigned long namepy)
{
	UTF16 buf16[16] = { 0 };
	memoryRead(namepy, buf16, 28);
	UTF16 *pTempUTF16 = buf16;
	UTF8 *pTempUTF8 = buf;
	UTF8 *pUTF8End = pTempUTF8 + 32;
	while (pTempUTF16 < pTempUTF16 + 28)
	{
		if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
		{
			*pTempUTF8++ = (UTF8) * pTempUTF16;
		}
		else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
		{
			*pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
		}
		else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
		{
    			*pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
			*pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
			*pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
		}
		else
		{
			break;
		}
		pTempUTF16++;
	}
}


//数组解密
struct Actors {
    uint64_t Enc_1, Enc_2;
    uint64_t Enc_3, Enc_4;
};
struct Chunk {
    uint32_t val_1, val_2, val_3, val_4;
    uint32_t val_5, val_6, val_7, val_8;
};
uint64_t DecryptActorsArray(uint64_t uLevel, int Actors_Offset, int EncryptedActors_Offset) {
    if (uLevel < 0x10000000)
        return 0;
 
    if (Read<uint64_t>(uLevel + Actors_Offset) > 0)
		return uLevel + Actors_Offset;
 
    if (Read<uint64_t>(uLevel + EncryptedActors_Offset) > 0)
		return uLevel + EncryptedActors_Offset;
 
    auto AActors = Read<Actors>(uLevel + EncryptedActors_Offset + 0x10);
 
    if (AActors.Enc_1 > 0) {
        auto Enc = Read<Chunk>(AActors.Enc_1 + 0x80);
        return (((Read<uint8_t>(AActors.Enc_1 + Enc.val_1)
            | (Read<uint8_t>(AActors.Enc_1 + Enc.val_2) << 8))
            | (Read<uint8_t>(AActors.Enc_1 + Enc.val_3) << 0x10)) & 0xFFFFFF
            | ((uint64_t)Read<uint8_t>(AActors.Enc_1 + Enc.val_4) << 0x18)
            | ((uint64_t)Read<uint8_t>(AActors.Enc_1 + Enc.val_5) << 0x20)) & 0xFFFF00FFFFFFFFFF
            | ((uint64_t)Read<uint8_t>(AActors.Enc_1 + Enc.val_6) << 0x28)
            | ((uint64_t)Read<uint8_t>(AActors.Enc_1 + Enc.val_7) << 0x30)
            | ((uint64_t)Read<uint8_t>(AActors.Enc_1 + Enc.val_8) << 0x38);
    }
    else if (AActors.Enc_2 > 0) {
        auto Lost_Actors = Read<uint64_t>(AActors.Enc_2);
        if (Lost_Actors > 0) {
            return (uint16_t)(Lost_Actors - 0x400) & 0xFF00
                | (uint8_t)(Lost_Actors - 0x04)
                | (Lost_Actors + 0xFC0000) & 0xFF0000
                | (Lost_Actors - 0x4000000) & 0xFF000000
                | (Lost_Actors + 0xFC00000000) & 0xFF00000000
                | (Lost_Actors + 0xFC0000000000) & 0xFF0000000000
                | (Lost_Actors + 0xFC000000000000) & 0xFF000000000000
                | (Lost_Actors - 0x400000000000000) & 0xFF00000000000000;
        }
    }
    else if (AActors.Enc_3 > 0) {
        auto Lost_Actors = Read<uint64_t>(AActors.Enc_3);
        if (Lost_Actors > 0) {
            return (Lost_Actors >> 0x38) | (Lost_Actors << (64 - 0x38));
		}
    }
    else if (AActors.Enc_4 > 0) {
        auto Lost_Actors = Read<uint64_t>(AActors.Enc_4);
        if (Lost_Actors > 0) {
            return Lost_Actors ^ 0xCDCD00;
		}
    }
    return 0;
}
