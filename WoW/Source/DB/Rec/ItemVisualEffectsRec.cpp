#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"

// STATIC: 0x00C1D168/0x140A10FC8
const DBCMeta ItemVisualEffectsRec::s_meta = { "DBFilesClient\\ItemVisualEffects.dbc", 2, 8, ItemVisualEffectsRec::FixupStrings, 0 };

// PROVEN: cfd1f086 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemVisualEffectsRec::FixupStrings(void* record, const char* strings) {
    ItemVisualEffectsRec* rec = static_cast<ItemVisualEffectsRec*>(record);
    DBCFixupString(rec->m_model, strings);
}
