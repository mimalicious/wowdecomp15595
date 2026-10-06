#include "WoW/Source/DB/Rec/UnitPowerBarRec.h"

// STATIC: 0x00C1FD34/0x140A143B8
const DBCMeta UnitPowerBarRec::s_meta = { "DBFilesClient\\UnitPowerBar.dbc", 17, 108, UnitPowerBarRec::FixupStrings, 0 };

// OFFSET: 0x00A2E9E0/0x1407D7F10
void UnitPowerBarRec::FixupStrings(void* record, const char* strings) {
    UnitPowerBarRec* rec = static_cast<UnitPowerBarRec*>(record);
    DBCFixupString(rec->unk_0054, strings);
    DBCFixupString(rec->unk_0058, strings);
    DBCFixupString(rec->unk_005C, strings);
    DBCFixupString(rec->unk_0060, strings);
}
