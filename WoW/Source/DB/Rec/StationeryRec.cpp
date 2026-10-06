#include "WoW/Source/DB/Rec/StationeryRec.h"

// STATIC: 0x00C2055C/0x140A14DC0
const DBCMeta StationeryRec::s_meta = { "DBFilesClient\\Stationery.dbc", 4, 16, StationeryRec::FixupStrings, 0 };

// PROVEN: 899224cc IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void StationeryRec::FixupStrings(void* record, const char* strings) {
    StationeryRec* rec = static_cast<StationeryRec*>(record);
    DBCFixupString(rec->m_texture, strings);
}
