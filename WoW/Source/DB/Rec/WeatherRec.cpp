#include "WoW/Source/DB/Rec/WeatherRec.h"

// STATIC: 0x00C27ACC/0x140A1E080
const DBCMeta WeatherRec::s_meta = { "DBFilesClient\\Weather.dbc", 6, 32, WeatherRec::FixupStrings, 0 };

// PROVEN: 0f0d9524 IDENTICAL/NONE
// OFFSET: 0x00A3B700/0x1407E8500
void WeatherRec::FixupStrings(void* record, const char* strings) {
    WeatherRec* rec = static_cast<WeatherRec*>(record);
    DBCFixupString(rec->m_effectTexture, strings);
}
