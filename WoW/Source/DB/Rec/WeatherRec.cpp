#include "WoW/Source/DB/Rec/WeatherRec.h"

// STATIC: 0x00C27A84/0x140A1E038
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 28 };

// STATIC: 0x00C27A9C/0x140A1E050
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 12, 4 };

// STATIC: 0x00C27AB4/0x140A1E068
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 3, 3, 2 };

// STATIC: 0x00C27ACC/0x140A1E080
const DBCMeta WeatherRec::s_meta = { "DBFilesClient\\Weather.dbc", 6, 32, WeatherRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0f0d9524 IDENTICAL/DIFFERENT
// OFFSET: 0x00A3B700/0x1407E8500
void WeatherRec::FixupStrings(void* record, const char* strings) {
    WeatherRec* rec = static_cast<WeatherRec*>(record);
    DBCFixupString(rec->m_effectTexture, strings);
}
