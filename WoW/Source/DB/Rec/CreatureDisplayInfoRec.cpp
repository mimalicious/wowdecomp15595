#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"

// STATIC: 0x00C1D988/0x140A11958
const DBCMeta CreatureDisplayInfoRec::s_meta = { "DBFilesClient\\CreatureDisplayInfo.dbc", 15, 68, CreatureDisplayInfoRec::FixupStrings, 0 };

// PROVEN: 5f35d268 IDENTICAL/NONE
// OFFSET: 0x00A2E850/0x1407D7970
void CreatureDisplayInfoRec::FixupStrings(void* record, const char* strings) {
    CreatureDisplayInfoRec* rec = static_cast<CreatureDisplayInfoRec*>(record);
    DBCFixupString(rec->m_textureVariation[0], strings);
    DBCFixupString(rec->m_textureVariation[1], strings);
    DBCFixupString(rec->m_textureVariation[2], strings);
    DBCFixupString(rec->m_portraitTextureName, strings);
}
