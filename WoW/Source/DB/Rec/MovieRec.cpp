#include "WoW/Source/DB/Rec/MovieRec.h"

// STATIC: 0x00C1E17C/0x140A12308
const DBCMeta MovieRec::s_meta = { "DBFilesClient\\Movie.dbc", 4, 16, MovieRec::FixupStrings, 0 };

// PROVEN: f0c30ca8 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void MovieRec::FixupStrings(void* record, const char* strings) {
    MovieRec* rec = static_cast<MovieRec*>(record);
    DBCFixupString(rec->m_filename, strings);
}
