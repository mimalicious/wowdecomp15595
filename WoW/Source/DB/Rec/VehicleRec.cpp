#include "WoW/Source/DB/Rec/VehicleRec.h"

// STATIC: %0x00B44768
const DBCMeta VehicleRec::s_meta = { "DBFilesClient\\Vehicle.dbc", 29, 160, VehicleRec::FixupStrings, 0 };

// OFFSET: %0x0097C150
void VehicleRec::FixupStrings(void* record, const char* strings) {
    VehicleRec* rec = static_cast<VehicleRec*>(record);
    rec->m_msslTrgtArcTexture = &strings[reinterpret_cast<uintptr_t>(rec->m_msslTrgtArcTexture)];
    rec->m_msslTrgtImpactTexture = &strings[reinterpret_cast<uintptr_t>(rec->m_msslTrgtImpactTexture)];
    rec->m_msslTrgtImpactModel[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_msslTrgtImpactModel[0])];
    rec->m_msslTrgtImpactModel[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_msslTrgtImpactModel[1])];
}

#if defined(WOWDECOMP_ARCH_64)
void VehicleRec::ConvertRow(VehicleRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_turnSpeed, &row[0x0008], sizeof(rec->m_turnSpeed));
    memcpy(&rec->m_pitchSpeed, &row[0x000C], sizeof(rec->m_pitchSpeed));
    memcpy(&rec->m_pitchMin, &row[0x0010], sizeof(rec->m_pitchMin));
    memcpy(&rec->m_pitchMax, &row[0x0014], sizeof(rec->m_pitchMax));
    memcpy(rec->m_seatID, &row[0x0018], sizeof(rec->m_seatID));
    memcpy(&rec->m_mouseLookOffsetPitch, &row[0x0038], sizeof(rec->m_mouseLookOffsetPitch));
    memcpy(&rec->m_cameraFadeDistScalarMin, &row[0x003C], sizeof(rec->m_cameraFadeDistScalarMin));
    memcpy(&rec->m_cameraFadeDistScalarMax, &row[0x0040], sizeof(rec->m_cameraFadeDistScalarMax));
    memcpy(&rec->m_cameraPitchOffset, &row[0x0044], sizeof(rec->m_cameraPitchOffset));
    memcpy(&rec->m_facingLimitRight, &row[0x0048], sizeof(rec->m_facingLimitRight));
    memcpy(&rec->m_facingLimitLeft, &row[0x004C], sizeof(rec->m_facingLimitLeft));
    memcpy(&rec->m_msslTrgtTurnLingering, &row[0x0050], sizeof(rec->m_msslTrgtTurnLingering));
    memcpy(&rec->m_msslTrgtPitchLingering, &row[0x0054], sizeof(rec->m_msslTrgtPitchLingering));
    memcpy(&rec->m_msslTrgtMouseLingering, &row[0x0058], sizeof(rec->m_msslTrgtMouseLingering));
    memcpy(&rec->m_msslTrgtEndOpacity, &row[0x005C], sizeof(rec->m_msslTrgtEndOpacity));
    memcpy(&rec->m_msslTrgtArcSpeed, &row[0x0060], sizeof(rec->m_msslTrgtArcSpeed));
    memcpy(&rec->m_msslTrgtArcRepeat, &row[0x0064], sizeof(rec->m_msslTrgtArcRepeat));
    memcpy(&rec->m_msslTrgtArcWidth, &row[0x0068], sizeof(rec->m_msslTrgtArcWidth));
    memcpy(rec->m_msslTrgtImpactRadius, &row[0x006C], sizeof(rec->m_msslTrgtImpactRadius));
    rec->m_msslTrgtArcTexture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0074])];
    rec->m_msslTrgtImpactTexture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0078])];
    rec->m_msslTrgtImpactModel[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x007C])];
    rec->m_msslTrgtImpactModel[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0080])];
    memcpy(&rec->m_cameraYawOffset, &row[0x0084], sizeof(rec->m_cameraYawOffset));
    memcpy(&rec->m_uiLocomotionType, &row[0x0088], sizeof(rec->m_uiLocomotionType));
    memcpy(&rec->m_msslTrgtImpactTexRadius, &row[0x008C], sizeof(rec->m_msslTrgtImpactTexRadius));
    memcpy(&rec->m_vehicleUiindicatorID, &row[0x0090], sizeof(rec->m_vehicleUiindicatorID));
    memcpy(rec->m_powerDisplayID, &row[0x0094], sizeof(rec->m_powerDisplayID));
}
#endif
