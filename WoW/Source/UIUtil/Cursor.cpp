#include "Cursor.h"
#include <cstring>
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/LootFrame.h"

// STATIC: #0x00AD2808
static const char* s_cursorNames[54] = {
    NULL,
    "Point",
    "Cast",
    "Buy",
    "Attack",
    "Interact",
    "Speak",
    "Inspect",
    "Pickup",
    "Taxi",
    "Trainer",
    "Mine",
    "Skin",
    "GatherHerbs",
    "PickLock",
    "Mail",
    "LootAll",
    "Repair",
    "RepairNPC",
    "Item",
    "SkinHorde",
    "SkinAlliance",
    "Innkeeper",
    "Quest",
    "QuestRepeatable",
    "QuestTurnIn",
    "vehichleCursor",
    "UnablePoint",
    "UnableCast",
    "UnableBuy",
    "UnableAttack",
    "UnableInteract",
    "UnableSpeak",
    "UnableInspect",
    "UnablePickup",
    "UnableTaxi",
    "UnableTrainer",
    "UnableMine",
    "UnableSkin",
    "UnableGatherHerbs",
    "UnablePickLock",
    "UnableMail",
    "UnableLootAll",
    "UnableRepair",
    "UnableRepairNPC",
    "UnableItem",
    "UnableSkinHorde",
    "UnableSkinAlliance",
    "UnableInnkeeper",
    "UnableQuest",
    "UnableQuestRepeatable",
    "UnableQuestTurnIn",
    "UnablevehichleCursor",
    NULL,
};

// STATIC: #0x00C25CD0
static char s_cursorFile[260];

// STATIC: #0x00C25DD4
static uint32_t s_cursorItemMipsHeight;

// STATIC: #0x00C25DD8
static uint32_t s_cursorItemMipsWidth;

// STATIC: #0x00C25DDC
static MipBits* s_cursorItemMips;

// STATIC: #0x00C25DE0
static uint32_t s_cursorItemType;

// STATIC: #0x00C25DE4
static uint32_t s_cursorResetMode;

// STATIC: #0x00C25DE8
static uint32_t s_cursorItem[1024];

// STATIC: #0x00C26DE8
static uint32_t s_cursorMode;

// STATIC: #0x00C26DF0
static uint32_t s_cursorImages[54][1024];

// PROVEN: 80f3c29c IDENTICAL/NONE
// OFFSET: 0x00486580/0x1400A74C0
void CursorDropItem() {
    if (s_cursorItemType) {
        s_cursorItemType = 0;
        UpdateCursor();
    }
}

// PROVEN: c792b4e5 IDENTICAL/NONE
// OFFSET: 0x00486000/0x1400A6D20
uint32_t CursorGetResetMode() {
    return s_cursorResetMode;
}

// PROVEN: 5d9b37c6 IDENTICAL/NONE
// OFFSET: 0x00486010/0x1400A6D30
void CursorSetResetMode(uint32_t mode) {
    s_cursorResetMode = mode;
}

// OFFSET: #0x006160B0
int32_t CopyCursorImage(uint32_t* image, MipBits* mipImages, uint32_t width, uint32_t height) {
    if (mipImages) {
        if (width == 32) {
            if (height == 32) {
                memcpy(image, mipImages->m_mip[0], 4096);
                return 1;
            }
        } else if (width == 64 && height == 64) {
            const uint32_t* source = reinterpret_cast<const uint32_t*>(mipImages->m_mip[0]);

            for (uint32_t* row = image; row < image + 1024; row += 32) {
                const uint32_t* top = source;
                const uint32_t* bottom = source + 64;

                for (uint32_t* pixel = row; pixel < row + 32; pixel++) {
                    uint32_t redBlue = ((top[0] & 0xFF00FF) + (top[1] & 0xFF00FF) + (bottom[0] & 0xFF00FF) + (bottom[1] & 0xFF00FF) + 0x20002) >> 2;
                    uint32_t green = ((top[0] >> 2) & 0x3FC0) + ((top[1] >> 2) & 0x3FC0) + ((bottom[0] >> 2) & 0x3FC0) + ((bottom[1] >> 2) & 0x3FC0) + 0x80;
                    *pixel = (redBlue & 0xFFFF00FF) | (green & 0xFF00) | 0xFF000000;

                    top += 2;
                    bottom += 2;
                }

                source += 128;
            }

            return 1;
        }
    }

    memset(image, 0, 4096);
    return 0;
}

// OFFSET: #0x006162C0
void UpdateCursor() {
    uint32_t* pixels = g_theGxDevicePtr->CursorLock();

    if (s_cursorItemType) {
        if (s_cursorItemMips) {
            memcpy(pixels, s_cursorImages[19], 4096);
        } else {
            for (uint32_t i = 0; i < 1024; i++) {
                uint32_t pixel = s_cursorImages[19][i];

                if (!(pixel & 0xFF000000)) {
                    pixel = s_cursorItem[i];
                }

                pixels[i] = pixel;
            }
        }
    } else {
        memcpy(pixels, s_cursorImages[s_cursorMode], 4096);
    }

    g_theGxDevicePtr->CursorUnlock(0, 0);
}

// OFFSET: #0x006163B0
void CursorInitialize() {
    s_cursorMode = 1;
    s_cursorResetMode = 1;
    s_cursorItemType = 0;
    s_cursorItemMips = NULL;
    s_cursorItemMipsWidth = 0;
    s_cursorItemMipsHeight = 0;

    for (uint32_t i = 0; i < 54; i++) {
        MipBits* images = NULL;
        uint32_t width;
        uint32_t height;
        PIXEL_FORMAT format;

        if (s_cursorNames[i]) {
            char fileName[260];
            SStrPrintf(fileName, sizeof(fileName), "Interface\\Cursor\\%s.blp", s_cursorNames[i]);

            width = 0;
            height = 0;
            format = PIXEL_ARGB8888;
            images = TextureLoadImage(fileName, &width, &height, &format, NULL, NULL, NULL, 0);
        }

        if (images) {
            CopyCursorImage(s_cursorImages[i], images, width, height);
            TextureFreeMippedImg(images, format, width, height);
        } else {
            memset(s_cursorImages[i], 0, sizeof(s_cursorImages[i]));
        }
    }

    s_cursorFile[0] = '\0';
    UpdateCursor();
}

// PROVEN: 69c795fc IDENTICAL/NONE
// OFFSET: 0x00486020/0x1400A6D40
uint32_t CursorGetModeFromName(const char* name) {
    for (uint32_t i = 1; i < 53; i++) {
        if (!SStrCmpI(name, s_cursorNames[i], STORM_MAX_STR)) {
            return i;
        }
    }

    return 0;
}

// OFFSET: #0x00616800
void CursorSetMode(uint32_t mode) {
    if (mode != 53 && s_cursorMode != mode) {
        s_cursorMode = mode;
        UpdateCursor();
        FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
    }
}

// OFFSET: %0x0048E8F0
void CursorGrabVirtualItem(int32_t displayId) {
    if (s_cursorItemMips) {
        TextureFreeMippedImg(s_cursorItemMips);
        s_cursorItemMips = NULL;
        s_cursorItemMipsWidth = 0;
        s_cursorItemMipsHeight = 0;
    }

    if (displayId) {
        const char* path = ClientDBStringLookup(3);
        const char* separator = "\\";

        if (!*path) {
            separator = "";
        }

        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(displayId));
        PIXEL_FORMAT format = PIXEL_ARGB8888;
        s_cursorItemMips = TextureLoadImage(fileName, &s_cursorItemMipsWidth, &s_cursorItemMipsHeight, &format, NULL, NULL, NULL, 0);
    }

    s_cursorItemType = 1;
    CursorReleaseItemMips();
}

// OFFSET: #0x00616830
int32_t CursorSetFromName(const char* name) {
    int32_t result = !SStrCmpI(s_cursorFile, name, STORM_MAX_STR);
    int32_t changed = !result || s_cursorMode != 53;

    if (!result) {
        uint32_t width = 0;
        uint32_t height = 0;
        PIXEL_FORMAT format = PIXEL_ARGB8888;
        MipBits* images = TextureLoadImage(name, &width, &height, &format, NULL, NULL, NULL, 0);

        if (images) {
            if (CopyCursorImage(s_cursorImages[53], images, width, height)) {
                result = 1;
            }

            TextureFreeMippedImg(images, format, width, height);
        } else {
            memset(s_cursorImages[53], 0, sizeof(s_cursorImages[53]));
        }

        SStrCopy(s_cursorFile, name, sizeof(s_cursorFile));
    }

    if (changed) {
        s_cursorMode = 53;
        UpdateCursor();
        FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
    }

    return result;
}

// OFFSET: #0x00616920
void CursorResetMode() {
    if (s_cursorResetMode != 53 && s_cursorMode != s_cursorResetMode) {
        s_cursorMode = s_cursorResetMode;
        UpdateCursor();
        FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
    }
}

// OFFSET: #0x006164B0
void CursorReleaseItemMips() {
    MipBits* mips = s_cursorItemMips;

    if (!mips) {
        return;
    }

    if (s_cursorItemType) {
        CopyCursorImage(s_cursorItem, mips, s_cursorItemMipsWidth, s_cursorItemMipsHeight);
        TextureFreeMippedImg(mips);
    } else {
        TextureFreeMippedImg(s_cursorItemMips);
    }

    s_cursorItemMipsHeight = 0;
    s_cursorItemMipsWidth = 0;
    s_cursorItemMips = NULL;
    UpdateCursor();
}

// OFFSET: #0x00616630
void CursorSetHeldItem(WOWGUID item) {
    if (s_cursorItemMips) {
        TextureFreeMippedImg(s_cursorItemMips);
        s_cursorItemMips = NULL;
        s_cursorItemMipsWidth = 0;
        s_cursorItemMipsHeight = 0;
    }

    CGItem_C* itemObject = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (itemObject) {
        const char* path = ClientDBStringLookup(3);
        const char* art = itemObject->GetInventoryArt();
        const char* separator = "\\";

        if (!path || !*path || !art || !*art) {
            separator = "";
        }

        char fileName[260];
        SStrPrintf(fileName, sizeof(fileName), "%s%s%s", path, separator, art);
        PIXEL_FORMAT format = PIXEL_ARGB8888;
        s_cursorItemMips = TextureLoadImage(fileName, &s_cursorItemMipsWidth, &s_cursorItemMipsHeight, &format, NULL, NULL, NULL, 0);
    }

    s_cursorItemType = 1;
    CursorReleaseItemMips();
}

// OFFSET: 0x004862B0/0x1400A7030
void CursorGrabMoney(uint64_t amount) {
    if (s_cursorItemMips) {
        TextureFreeMippedImg(s_cursorItemMips);
        s_cursorItemMips = NULL;
        s_cursorItemMipsWidth = 0;
        s_cursorItemMipsHeight = 0;
    }

    char fileName[260];
    GetCurrencyIcon(amount, fileName, sizeof(fileName));
    PIXEL_FORMAT format = PIXEL_ARGB8888;
    s_cursorItemMips = TextureLoadImage(fileName, &s_cursorItemMipsWidth, &s_cursorItemMipsHeight, &format, NULL, NULL, NULL, 0);
    s_cursorItemType = 2;
    CursorReleaseItemMips();
}

// OFFSET: %0x0048E780
void CursorGrabSpell(const char* texture) {
    if (s_cursorItemMips) {
        TextureFreeMippedImg(s_cursorItemMips);
        s_cursorItemMips = NULL;
        s_cursorItemMipsWidth = 0;
        s_cursorItemMipsHeight = 0;
    }

    if (texture) {
        PIXEL_FORMAT format = PIXEL_ARGB8888;
        s_cursorItemMips = TextureLoadImage(texture, &s_cursorItemMipsWidth, &s_cursorItemMipsHeight, &format, NULL, NULL, NULL, 0);
    }

    s_cursorItemType = 3;
    CursorReleaseItemMips();
}

// PROVEN: e9dc305e IDENTICAL/NONE
// OFFSET: 0x00485FC0/0x1400A6CE0
void CursorDestroy() {
    if (!s_cursorItemMips) {
        return;
    }

    TextureFreeMippedImg(s_cursorItemMips);
    s_cursorItemMips = NULL;
    s_cursorItemMipsWidth = 0;
    s_cursorItemMipsHeight = 0;
}
