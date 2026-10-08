#ifndef WOW_SOURCE_OBJECT_OBJECT_H
#define WOW_SOURCE_OBJECT_OBJECT_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "WoW/Source/Object/ObjectClient/Types.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/Object/WGUID.h"

class CClientObjCreate;
class CGObjectData;

class __declspec(novtable) CGObject {
    public:
    static const uint32_t s_savedFields[3];

    static int32_t GetSavedIndexFromDataIndex(uint32_t index);
    static int32_t TotalFieldsSaved();

    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t m_heapIndex;
    /* 000C */ CGObjectData* m_obj;
    /* 0010 */ void* m_objMirror;
    /* 0014 */ OBJECT_TYPE_ID m_typeId;

    virtual void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) = 0;
};

class CGObjectData {
    public:
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ WOWGUID m_data;
    /* 0010 */ uint16_t m_type;
    /* 0012 */ uint16_t m_dataType;
    /* 0014 */ uint32_t m_entryID;
    /* 0018 */ float m_scale;
    /* 001C */ uint32_t m_pad;
};

#endif
