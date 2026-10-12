#include "GameObjectDef.h"

static const int32_t s_props0[8] = { 1, 4, 3, 57, 89, 90, 117, 85 };
static const int32_t s_props1[10] = { 1, 4, 3, 30, 57, 48, 89, 90, 55, 85 };
static const int32_t s_props2[11] = { 4, 5, 17, 38, 22, 57, 89, 55, 91, 48, 85 };
static const int32_t s_props3[18] = { 4, 6, 7, 8, 25, 26, 13, 30, 20, 31, 55, 56, 62, 66, 89, 92, 19, 85 };
static const int32_t s_props5[7] = { 19, 18, 37, 48, 63, 20, 85 };
static const int32_t s_props6[16] = { 4, 31, 24, 10, 9, 23, 3, 32, 37, 65, 48, 58, 89, 90, 123, 85 };
static const int32_t s_props7[5] = { 11, 12, 94, 13, 85 };
static const int32_t s_props8[9] = { 14, 24, 30, 37, 20, 48, 19, 63, 85 };
static const int32_t s_props9[5] = { 15, 16, 17, 91, 85 };
static const int32_t s_props10[23] = { 4, 20, 21, 3, 22, 8, 23, 15, 16, 17, 10, 57, 30, 48, 89, 90, 55, 91, 19, 38, 127, 63, 85 };
static const int32_t s_props11[6] = { 93, 1, 3, 125, 126, 87 };
static const int32_t s_props12[8] = { 4, 24, 27, 28, 29, 3, 89, 90 };
static const int32_t s_props13[5] = { 4, 33, 21, 89, 85 };
static const int32_t s_props15[9] = { 35, 36, 43, 44, 45, 99, 87, 67, 128 };
static const int32_t s_props18[9] = { 34, 10, 39, 40, 49, 50, 54, 46, 85 };
static const int32_t s_props19[1] = { 85 };
static const int32_t s_props21[2] = { 42, 9 };
static const int32_t s_props22[6] = { 10, 9, 47, 91, 48, 85 };
static const int32_t s_props23[3] = { 51, 52, 53 };
static const int32_t s_props24[9] = { 4, 59, 24, 60, 61, 57, 89, 55, 85 };
static const int32_t s_props25[5] = { 24, 6, 25, 26, 4 };
static const int32_t s_props26[5] = { 4, 21, 59, 57, 89 };
static const int32_t s_props27[1] = { 64 };
static const int32_t s_props29[22] = { 24, 10, 67, 68, 70, 71, 72, 73, 74, 75, 76, 77, 78, 69, 79, 80, 81, 82, 48, 18, 118, 119 };
static const int32_t s_props30[7] = { 1, 24, 83, 85, 84, 86, 37 };
static const int32_t s_props31[2] = { 87, 88 };
static const int32_t s_props32[2] = { 12, 121 };
static const int32_t s_props33[24] = { 95, 97, 103, 109, 103, 96, 103, 103, 103, 110, 103, 103, 103, 103, 111, 103, 112, 103, 124, 114, 103, 103, 120, 103 };
static const int32_t s_props34[1] = { 85 };
static const int32_t s_props35[3] = { 93, 1, 3 };

// STATIC: 0x00CA74C0/0x140B46D40
const CGameObjectDef::TypeInfo CGameObjectDef::s_typeInfo[36] = {
    { "door", 8, s_props0, NULL },
    { "button", 10, s_props1, NULL },
    { "questgiver", 11, s_props2, NULL },
    { "chest", 18, s_props3, NULL },
    { "binder", 0, NULL, NULL },
    { "generic", 7, s_props5, NULL },
    { "trap", 16, s_props6, NULL },
    { "chair", 5, s_props7, NULL },
    { "spellFocus", 9, s_props8, NULL },
    { "text", 5, s_props9, NULL },
    { "goober", 23, s_props10, NULL },
    { "transport", 6, s_props11, NULL },
    { "areaDamage", 8, s_props12, NULL },
    { "camera", 5, s_props13, NULL },
    { "mapobject", 0, NULL, NULL },
    { "moTransport", 9, s_props15, NULL },
    { "duelFlag", 0, NULL, NULL },
    { "fishingNode", 0, NULL, NULL },
    { "ritual", 9, s_props18, NULL },
    { "mailbox", 1, s_props19, NULL },
    { "DONOTUSE", 0, NULL, NULL },
    { "guardPost", 2, s_props21, NULL },
    { "spellCaster", 6, s_props22, NULL },
    { "meetingStone", 3, s_props23, NULL },
    { "flagStand", 9, s_props24, NULL },
    { "fishingHole", 5, s_props25, NULL },
    { "flagDrop", 5, s_props26, NULL },
    { "minigame", 1, s_props27, NULL },
    { "DONOTUSE", 0, NULL, NULL },
    { "capturePoint", 22, s_props29, NULL },
    { "auraGenerator", 7, s_props30, NULL },
    { "dungeonDifficulty", 2, s_props31, NULL },
    { "barberChair", 2, s_props32, NULL },
    { "destructibleBuilding", 24, s_props33, NULL },
    { "guild bank", 1, s_props34, NULL },
    { "trapdoor", 3, s_props35, NULL },
};

// PROVEN: d1af7773 IDENTICAL/IDENTICAL
// OFFSET: 0x005E6B10/0x140267470
const char* CGameObjectDef::NameFromTypeId(int32_t typeId) {
    if (static_cast<uint32_t>(typeId) <= 35) {
        return CGameObjectDef::s_typeInfo[typeId].m_name;
    }

    return NULL;
}

// OFFSET: 0x005E6B30/0x140267490
int32_t CGameObjectDef::GetPropNum(int32_t typeId, int32_t prop) {
    if (typeId >= 36 || prop >= 129) {
        return -1;
    }

    int32_t count = CGameObjectDef::s_typeInfo[typeId].m_numProps;

    for (int32_t i = 0; i < count; i++) {
        if (CGameObjectDef::s_typeInfo[typeId].m_props[i] == prop) {
            return i;
        }
    }

    return -1;
}
