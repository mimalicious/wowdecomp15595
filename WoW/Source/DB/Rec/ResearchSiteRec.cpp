#include "WoW/Source/DB/Rec/ResearchSiteRec.h"

// STATIC: 0x00C229E8/0x140A17988
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C229FC/0x140A179A0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C22A10/0x140A179B8
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 2, 0 };

// STATIC: 0x00C22A24/0x140A179D0
const DBCMeta ResearchSiteRec::s_meta = { "DBFilesClient\\ResearchSite.dbc", 5, 20, ResearchSiteRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a8f77e39 IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void ResearchSiteRec::FixupStrings(void* record, const char* strings) {
    ResearchSiteRec* rec = static_cast<ResearchSiteRec*>(record);
    DBCFixupString(rec->unk_000C, strings);
}
