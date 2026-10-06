#include "WoW/Source/DB/Rec/SoundEmittersRec.h"

// STATIC: 0x00C27C74/0x140A1E298
static const int32_t s_fieldOffsets[7] = { 0, 4, 16, 28, 32, 36, 40 };

// STATIC: 0x00C27C90/0x140A1E2B8
static const int32_t s_fieldSizes[7] = { 4, 12, 12, 4, 4, 4, 4 };

// STATIC: 0x00C27CAC/0x140A1E2D8
static const int32_t s_fieldTypes[7] = { 0, 3, 3, 0, 0, 2, 0 };

// STATIC: 0x00C27CC8/0x140A1E2F8
const DBCMeta SoundEmittersRec::s_meta = { "DBFilesClient\\SoundEmitters.dbc", 7, 44, SoundEmittersRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: dba5006f IDENTICAL/NONE
// OFFSET: 0x00A2EB70/0x1407D8310
void SoundEmittersRec::FixupStrings(void* record, const char* strings) {
    SoundEmittersRec* rec = static_cast<SoundEmittersRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
