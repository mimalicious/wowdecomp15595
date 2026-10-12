#include "WoW/Source/DB/Rec/SoundFilterRec.h"

// STATIC: 0x00C1DF00/0x140A11FD0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1DF08/0x140A11FD8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1DF10/0x140A11FE0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1DF18/0x140A11FE8
const DBCMeta SoundFilterRec::s_meta = { "DBFilesClient\\SoundFilter.dbc", 2, 8, SoundFilterRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7e7ff3ec IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SoundFilterRec::FixupStrings(void* record, const char* strings) {
    SoundFilterRec* rec = static_cast<SoundFilterRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
