#include "WoW/Source/DB/Rec/LightSkyboxRec.h"

// STATIC: 0x00C27F94/0x140A1E668
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C27FA0/0x140A1E678
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C27FAC/0x140A1E688
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C27FB8/0x140A1E698
const DBCMeta LightSkyboxRec::s_meta = { "DBFilesClient\\LightSkybox.dbc", 3, 12, LightSkyboxRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void LightSkyboxRec::FixupStrings(void* record, const char* strings) {
    LightSkyboxRec* rec = static_cast<LightSkyboxRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
