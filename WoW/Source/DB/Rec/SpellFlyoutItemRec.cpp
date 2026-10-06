#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"

// STATIC: 0x00C213C0/0x140A15F10
const DBCMeta SpellFlyoutItemRec::s_meta = { "DBFilesClient\\SpellFlyoutItem.dbc", 4, 16, SpellFlyoutItemRec::FixupStrings, 0 };

// PROVEN: 164e5513 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellFlyoutItemRec::FixupStrings(void* record, const char* strings) {
}
