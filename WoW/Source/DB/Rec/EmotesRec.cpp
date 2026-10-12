#include "WoW/Source/DB/Rec/EmotesRec.h"

// STATIC: 0x00C258B4/0x140A1B558
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C258D4/0x140A1B578
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C258F4/0x140A1B598
static const int32_t s_fieldTypes[8] = { 0, 2, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C25914/0x140A1B5B8
const DBCMeta EmotesRec::s_meta = { "DBFilesClient\\Emotes.dbc", 8, 32, EmotesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesRec::FixupStrings(void* record, const char* strings) {
    EmotesRec* rec = static_cast<EmotesRec*>(record);
    DBCFixupString(rec->m_emoteSlashCommand, strings);
}
