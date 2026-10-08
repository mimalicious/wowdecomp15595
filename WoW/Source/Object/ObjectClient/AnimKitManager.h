#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H

#include <cstddef>
#include <cstdint>

class CM2Model;

class CAnimKitManager {
    public:
    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint16_t unk_0002;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ uint8_t unk_0008 [0xE8];

    CAnimKitManager();
};

#endif
