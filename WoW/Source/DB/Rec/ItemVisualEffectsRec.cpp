#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"

// STATIC: 0x00C1D150/0x140A10FB0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1D158/0x140A10FB8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1D160/0x140A10FC0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1D168/0x140A10FC8
const DBCMeta ItemVisualEffectsRec::s_meta = { "DBFilesClient\\ItemVisualEffects.dbc", 2, 8, ItemVisualEffectsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemVisualEffectsRec::FixupStrings(void* record, const char* strings) {
    ItemVisualEffectsRec* rec = static_cast<ItemVisualEffectsRec*>(record);
    DBCFixupString(rec->m_model, strings);
}
