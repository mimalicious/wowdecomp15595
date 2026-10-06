#include "WoW/Source/DB/Rec/QuestPOIBlobRec.h"

// STATIC: 0x00C22D1C/0x140A17D98
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C22D2C/0x140A17DA8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C22D3C/0x140A17DB8
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C22D4C/0x140A17DC8
const DBCMeta QuestPOIBlobRec::s_meta = { "DBFilesClient\\QuestPOIBlob.dbc", 4, 16, QuestPOIBlobRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: db36192a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestPOIBlobRec::FixupStrings(void* record, const char* strings) {
}
