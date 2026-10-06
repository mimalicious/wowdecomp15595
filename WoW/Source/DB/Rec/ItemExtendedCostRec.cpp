#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"

// STATIC: %0x00B460C4
const DBCMeta ItemExtendedCostRec::s_meta = { "DBFilesClient\\ItemExtendedCost.dbc", 15, 124, ItemExtendedCostRec::FixupStrings, 0, NULL, NULL, NULL };

// PROVEN: e503d6ee IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemExtendedCostRec::FixupStrings(void* record, const char* strings) {
}
