#include "WoW/Source/DB/Rec/FootprintTexturesRec.h"

// STATIC: 0x00C281F8/0x140A1E970
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C28200/0x140A1E978
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C28208/0x140A1E980
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C28210/0x140A1E988
const DBCMeta FootprintTexturesRec::s_meta = { "DBFilesClient\\FootprintTextures.dbc", 2, 8, FootprintTexturesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void FootprintTexturesRec::FixupStrings(void* record, const char* strings) {
    FootprintTexturesRec* rec = static_cast<FootprintTexturesRec*>(record);
    DBCFixupString(rec->m_footstepFilename, strings);
}
