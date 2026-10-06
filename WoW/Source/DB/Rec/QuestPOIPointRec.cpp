#include "WoW/Source/DB/Rec/QuestPOIPointRec.h"

// STATIC: 0x00C22CDC/0x140A17D40
const DBCMeta QuestPOIPointRec::s_meta = { "DBFilesClient\\QuestPOIPoint.dbc", 4, 16, QuestPOIPointRec::FixupStrings, 0 };

// PROVEN: 252a672a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestPOIPointRec::FixupStrings(void* record, const char* strings) {
}
