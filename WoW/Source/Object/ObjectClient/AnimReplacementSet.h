#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMREPLACEMENTSET_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMREPLACEMENTSET_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class AnimReplacementRec;
class AnimReplacementSetRec;

class CAnimReplacementSetDef {
    public:
    static TSGrowableArray<CAnimReplacementSetDef*> s_defs;

    static CAnimReplacementSetDef* GetDef(uint32_t id);
    static void InitSystem();
    static void UninitSystem();

    /* 0000 */ const AnimReplacementSetRec* m_rec;
    /* 0004 */ uint16_t m_rangeCount;
    /* 0006 */ uint16_t unk_0006;
    /* 0008 */ union {
        uint16_t m_count;
        uint16_t m_base;
    };
    /* 000A */ uint16_t m_minAnim;
    /* 000C */ union {
        uint16_t m_maxAnim;
        uint16_t* m_range;
        const AnimReplacementRec** m_list;
    };

    uint32_t FilterAnim(uint32_t anim) const;
};

#endif
