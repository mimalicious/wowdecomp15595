#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"

// STATIC: 0x00C28190/0x140A1E8E0
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C2819C/0x140A1E8F0
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C281A8/0x140A1E900
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C281B4/0x140A1E910
const DBCMeta GroundEffectDoodadRec::s_meta = { "DBFilesClient\\GroundEffectDoodad.dbc", 3, 12, GroundEffectDoodadRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void GroundEffectDoodadRec::FixupStrings(void* record, const char* strings) {
    GroundEffectDoodadRec* rec = static_cast<GroundEffectDoodadRec*>(record);
    DBCFixupString(rec->m_doodadpath, strings);
}
