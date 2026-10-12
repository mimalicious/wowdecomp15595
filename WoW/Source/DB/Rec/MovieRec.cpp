#include "WoW/Source/DB/Rec/MovieRec.h"

// STATIC: 0x00C1E14C/0x140A122D8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1E15C/0x140A122E8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1E16C/0x140A122F8
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 0 };

// STATIC: 0x00C1E17C/0x140A12308
const DBCMeta MovieRec::s_meta = { "DBFilesClient\\Movie.dbc", 4, 16, MovieRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f0c30ca8 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void MovieRec::FixupStrings(void* record, const char* strings) {
    MovieRec* rec = static_cast<MovieRec*>(record);
    DBCFixupString(rec->m_filename, strings);
}
