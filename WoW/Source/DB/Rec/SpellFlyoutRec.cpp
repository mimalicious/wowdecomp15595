#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"

// STATIC: 0x00C21454/0x140A15FC8
const DBCMeta SpellFlyoutRec::s_meta = { "DBFilesClient\\SpellFlyout.dbc", 7, 28, SpellFlyoutRec::FixupStrings, 0 };

// PROVEN: 9eff182c IDENTICAL/NONE
// OFFSET: 0x00A2EB30/0x1407D7E90
void SpellFlyoutRec::FixupStrings(void* record, const char* strings) {
    SpellFlyoutRec* rec = static_cast<SpellFlyoutRec*>(record);
    DBCFixupString(rec->unk_0014, strings);
    DBCFixupString(rec->unk_0018, strings);
}
