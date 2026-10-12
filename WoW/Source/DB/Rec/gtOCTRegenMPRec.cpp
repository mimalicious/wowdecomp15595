#include "WoW/Source/DB/Rec/gtOCTRegenMPRec.h"

// STATIC: 0x00C24C68/0x140A1A580
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24C6C/0x140A1A584
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24C70/0x140A1A588
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24C74/0x140A1A590
const DBCMeta gtOCTRegenMPRec::s_meta = { "DBFilesClient\\gtOCTRegenMP.dbc", 1, 8, gtOCTRegenMPRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtOCTRegenMPRec::FixupStrings(void* record, const char* strings) {
}
