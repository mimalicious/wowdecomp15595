#include "WoW/Source/DB/Rec/gtOCTHpPerStaminaRec.h"

// STATIC: 0x00C24CB8/0x140A1A5F0
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24CBC/0x140A1A5F4
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24CC0/0x140A1A5F8
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24CC4/0x140A1A600
const DBCMeta gtOCTHpPerStaminaRec::s_meta = { "DBFilesClient\\gtOCTHpPerStamina.dbc", 1, 8, gtOCTHpPerStaminaRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtOCTHpPerStaminaRec::FixupStrings(void* record, const char* strings) {
}
