#ifndef WOW_SOURCE_DB_REC_VEHICLEUIINDSEATREC_H
#define WOW_SOURCE_DB_REC_VEHICLEUIINDSEATREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class VehicleUIIndSeatRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_vehicleUiindicatorID;
    /* 0008 */ int32_t m_virtualSeatIndex;
    /* 000C */ float m_xpos;
    /* 0010 */ float m_ypos;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(VehicleUIIndSeatRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
