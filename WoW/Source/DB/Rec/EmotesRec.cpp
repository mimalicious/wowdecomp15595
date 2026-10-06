#include "WoW/Source/DB/Rec/EmotesRec.h"

// STATIC: 0x00C25914/0x140A1B5B8
const DBCMeta EmotesRec::s_meta = { "DBFilesClient\\Emotes.dbc", 8, 32, EmotesRec::FixupStrings, 0 };

// PROVEN: 3e3d8966 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void EmotesRec::FixupStrings(void* record, const char* strings) {
    EmotesRec* rec = static_cast<EmotesRec*>(record);
    DBCFixupString(rec->m_emoteSlashCommand, strings);
}
