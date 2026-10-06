#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"

// STATIC: 0x00C25F30/0x140A1BD10
const DBCMeta CurrencyTypesRec::s_meta = { "DBFilesClient\\CurrencyTypes.dbc", 10, 44, CurrencyTypesRec::FixupStrings, 0 };

// PROVEN: ea9cc3f2 IDENTICAL/NONE
// OFFSET: 0x00A2EC00/0x1407D84D0
void CurrencyTypesRec::FixupStrings(void* record, const char* strings) {
    CurrencyTypesRec* rec = static_cast<CurrencyTypesRec*>(record);
    DBCFixupString(rec->unk_0008, strings);
    DBCFixupString(rec->unk_000C, strings);
    DBCFixupString(rec->unk_0010, strings);
    DBCFixupString(rec->unk_0028, strings);
}
