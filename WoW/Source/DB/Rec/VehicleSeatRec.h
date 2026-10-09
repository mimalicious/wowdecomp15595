#ifndef WOW_SOURCE_DB_REC_VEHICLESEATREC_H
#define WOW_SOURCE_DB_REC_VEHICLESEATREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class VehicleSeatRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags;
    /* 0008 */ int32_t m_attachmentID;
    /* 000C */ float m_attachmentOffset [3];
    /* 0018 */ float m_enterPreDelay;
    /* 001C */ float m_enterSpeed;
    /* 0020 */ float m_enterGravity;
    /* 0024 */ float m_enterMinDuration;
    /* 0028 */ float m_enterMaxDuration;
    /* 002C */ float m_enterMinArcHeight;
    /* 0030 */ float m_enterMaxArcHeight;
    /* 0034 */ int32_t m_enterAnimStart;
    /* 0038 */ int32_t m_enterAnimLoop;
    /* 003C */ int32_t m_rideAnimStart;
    /* 0040 */ int32_t m_rideAnimLoop;
    /* 0044 */ int32_t m_rideUpperAnimStart;
    /* 0048 */ int32_t m_rideUpperAnimLoop;
    /* 004C */ float m_exitPreDelay;
    /* 0050 */ float m_exitSpeed;
    /* 0054 */ float m_exitGravity;
    /* 0058 */ float m_exitMinDuration;
    /* 005C */ float m_exitMaxDuration;
    /* 0060 */ float m_exitMinArcHeight;
    /* 0064 */ float m_exitMaxArcHeight;
    /* 0068 */ int32_t m_exitAnimStart;
    /* 006C */ int32_t m_exitAnimLoop;
    /* 0070 */ int32_t m_exitAnimEnd;
    /* 0074 */ float m_passengerYaw;
    /* 0078 */ float m_passengerPitch;
    /* 007C */ float m_passengerRoll;
    /* 0080 */ int32_t m_passengerAttachmentID;
    /* 0084 */ int32_t m_vehicleEnterAnim;
    /* 0088 */ int32_t m_vehicleExitAnim;
    /* 008C */ int32_t m_vehicleRideAnimLoop;
    /* 0090 */ int32_t m_vehicleEnterAnimBone;
    /* 0094 */ int32_t m_vehicleExitAnimBone;
    /* 0098 */ int32_t m_vehicleRideAnimLoopBone;
    /* 009C */ float m_vehicleEnterAnimDelay;
    /* 00A0 */ float m_vehicleExitAnimDelay;
    /* 00A4 */ int32_t m_vehicleAbilityDisplay;
    /* 00A8 */ int32_t m_enterUisoundID;
    /* 00AC */ int32_t m_exitUisoundID;
    /* 00B0 */ int32_t m_uiSkin;
    /* 00B4 */ int32_t m_flagsB;
    /* 00B8 */ float m_cameraEnteringDelay;
    /* 00BC */ float m_cameraEnteringDuration;
    /* 00C0 */ float m_cameraExitingDelay;
    /* 00C4 */ float m_cameraExitingDuration;
    /* 00C8 */ float m_cameraOffsetX;
    /* 00CC */ float m_cameraOffsetY;
    /* 00D0 */ float m_cameraOffsetZ;
    /* 00D4 */ float m_cameraPosChaseRate;
    /* 00D8 */ float m_cameraFacingChaseRate;
    /* 00DC */ float m_cameraEnteringZoom;
    /* 00E0 */ float m_cameraSeatZoomMin;
    /* 00E4 */ float m_cameraSeatZoomMax;
    /* 00E8 */ int32_t m_enterAnimKitID;
    /* 00EC */ int32_t m_rideAnimKitID;
    /* 00F0 */ int32_t m_exitAnimKitID;
    /* 00F4 */ int32_t m_vehicleEnterAnimKitID;
    /* 00F8 */ int32_t m_vehicleRideAnimKitID;
    /* 00FC */ int32_t m_vehicleExitAnimKitID;
    /* 0100 */ int32_t m_cameraModeID;
    /* 0104 */ int32_t m_flagsC;
};

#endif
