#include "WoW/Source/DB/Rec/PetPersonalityRec.h"

const DBCMeta PetPersonalityRec::s_meta = { "DBFilesClient\\PetPersonality.dbc", 4, 32, PetPersonalityRec::FixupStrings, 0, NULL, NULL, NULL };

// INLINED
void PetPersonalityRec::FixupStrings(void* record, const char* strings) {
    PetPersonalityRec* rec = static_cast<PetPersonalityRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
