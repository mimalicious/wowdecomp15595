#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_VOIDSTORAGE_C_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct VoidStorageItem {
    /* 0000 */ uint32_t unk_0000[6];
    /* 0018 */ WOWGUID m_guid;
    /* 0020 */ int32_t m_entryID;
};

class CGVoidStorage_C {
    public:
    VoidStorageItem* GetItem(WOWGUID guid);
};

void VoidStorage_C_Initialize();

#endif
