#include "WoW/Source/DB/Rec/QuestPOIBlobRec.h"

// STATIC: 0x00C22D4C/0x140A17DC8
const DBCMeta QuestPOIBlobRec::s_meta = { "DBFilesClient\\QuestPOIBlob.dbc", 4, 16, QuestPOIBlobRec::FixupStrings, 0 };

// PROVEN: db36192a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestPOIBlobRec::FixupStrings(void* record, const char* strings) {
}
