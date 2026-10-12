#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"

// STATIC: 0x00C1D8D4/0x140A11898
static const int32_t s_fieldOffsets[15] = { 0, 4, 8, 12, 16, 20, 24, 36, 40, 44, 48, 52, 56, 60, 64 };

// STATIC: 0x00C1D910/0x140A118D8
static const int32_t s_fieldSizes[15] = { 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1D94C/0x140A11918
static const int32_t s_fieldTypes[15] = { 0, 0, 0, 0, 3, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1D988/0x140A11958
const DBCMeta CreatureDisplayInfoRec::s_meta = { "DBFilesClient\\CreatureDisplayInfo.dbc", 15, 68, CreatureDisplayInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E850/0x1407D7970
void CreatureDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoRec* rec = static_cast<CreatureDisplayInfoRec*>(record);
    DBCFixupString(rec->m_textureVariation[0], strings);
    DBCFixupString(rec->m_textureVariation[1], strings);
    DBCFixupString(rec->m_textureVariation[2], strings);
    DBCFixupString(rec->m_portraitTextureName, strings);
}
