#include "WoW/Source/DB/Rec/LightSkyboxRec.h"

// STATIC: 0x00C27FB8/0x140A1E698
const DBCMeta LightSkyboxRec::s_meta = { "DBFilesClient\\LightSkybox.dbc", 3, 12, LightSkyboxRec::FixupStrings, 0 };

// PROVEN: 63d7ca2a IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void LightSkyboxRec::FixupStrings(void* record, const char* strings) {
    LightSkyboxRec* rec = static_cast<LightSkyboxRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
