#include "WoW/Source/DB/Rec/VehicleSeatRec.h"

// STATIC: %0x00B44738
const DBCMeta VehicleSeatRec::s_meta = { "DBFilesClient\\VehicleSeat.dbc", 62, 264, VehicleSeatRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void VehicleSeatRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void VehicleSeatRec::ConvertRow(VehicleSeatRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_attachmentID, &row[0x0008], sizeof(rec->m_attachmentID));
    memcpy(rec->m_attachmentOffset, &row[0x000C], sizeof(rec->m_attachmentOffset));
    memcpy(&rec->m_field33512213004, &row[0x0018], sizeof(rec->m_field33512213004));
    memcpy(&rec->m_enterSpeed, &row[0x001C], sizeof(rec->m_enterSpeed));
    memcpy(&rec->m_enterGravity, &row[0x0020], sizeof(rec->m_enterGravity));
    memcpy(&rec->m_enterMinDuration, &row[0x0024], sizeof(rec->m_enterMinDuration));
    memcpy(&rec->m_enterMaxDuration, &row[0x0028], sizeof(rec->m_enterMaxDuration));
    memcpy(&rec->m_enterMinArcHeight, &row[0x002C], sizeof(rec->m_enterMinArcHeight));
    memcpy(&rec->m_enterMaxArcHeight, &row[0x0030], sizeof(rec->m_enterMaxArcHeight));
    memcpy(&rec->m_enterAnimStart, &row[0x0034], sizeof(rec->m_enterAnimStart));
    memcpy(&rec->m_enterAnimLoop, &row[0x0038], sizeof(rec->m_enterAnimLoop));
    memcpy(&rec->m_rideAnimStart, &row[0x003C], sizeof(rec->m_rideAnimStart));
    memcpy(&rec->m_rideAnimLoop, &row[0x0040], sizeof(rec->m_rideAnimLoop));
    memcpy(&rec->m_rideUpperAnimStart, &row[0x0044], sizeof(rec->m_rideUpperAnimStart));
    memcpy(&rec->m_rideUpperAnimLoop, &row[0x0048], sizeof(rec->m_rideUpperAnimLoop));
    memcpy(&rec->m_field33512213017, &row[0x004C], sizeof(rec->m_field33512213017));
    memcpy(&rec->m_exitSpeed, &row[0x0050], sizeof(rec->m_exitSpeed));
    memcpy(&rec->m_exitGravity, &row[0x0054], sizeof(rec->m_exitGravity));
    memcpy(&rec->m_exitMinDuration, &row[0x0058], sizeof(rec->m_exitMinDuration));
    memcpy(&rec->m_exitMaxDuration, &row[0x005C], sizeof(rec->m_exitMaxDuration));
    memcpy(&rec->m_exitMinArcHeight, &row[0x0060], sizeof(rec->m_exitMinArcHeight));
    memcpy(&rec->m_exitMaxArcHeight, &row[0x0064], sizeof(rec->m_exitMaxArcHeight));
    memcpy(&rec->m_exitAnimStart, &row[0x0068], sizeof(rec->m_exitAnimStart));
    memcpy(&rec->m_exitAnimLoop, &row[0x006C], sizeof(rec->m_exitAnimLoop));
    memcpy(&rec->m_exitAnimEnd, &row[0x0070], sizeof(rec->m_exitAnimEnd));
    memcpy(&rec->m_field33512213027, &row[0x0074], sizeof(rec->m_field33512213027));
    memcpy(&rec->m_passengerPitch, &row[0x0078], sizeof(rec->m_passengerPitch));
    memcpy(&rec->m_field33512213029, &row[0x007C], sizeof(rec->m_field33512213029));
    memcpy(&rec->m_passengerAttachmentID, &row[0x0080], sizeof(rec->m_passengerAttachmentID));
    memcpy(&rec->m_vehicleEnterAnim, &row[0x0084], sizeof(rec->m_vehicleEnterAnim));
    memcpy(&rec->m_vehicleExitAnim, &row[0x0088], sizeof(rec->m_vehicleExitAnim));
    memcpy(&rec->m_vehicleRideAnimLoop, &row[0x008C], sizeof(rec->m_vehicleRideAnimLoop));
    memcpy(&rec->m_field33512213034, &row[0x0090], sizeof(rec->m_field33512213034));
    memcpy(&rec->m_vehicleExitAnimBone, &row[0x0094], sizeof(rec->m_vehicleExitAnimBone));
    memcpy(&rec->m_vehicleEnterAnimBone, &row[0x0098], sizeof(rec->m_vehicleEnterAnimBone));
    memcpy(&rec->m_field33512213037, &row[0x009C], sizeof(rec->m_field33512213037));
    memcpy(&rec->m_field33512213038, &row[0x00A0], sizeof(rec->m_field33512213038));
    memcpy(&rec->m_vehicleAbilityDisplay, &row[0x00A4], sizeof(rec->m_vehicleAbilityDisplay));
    memcpy(&rec->m_enterUisoundID, &row[0x00A8], sizeof(rec->m_enterUisoundID));
    memcpy(&rec->m_field33512213041, &row[0x00AC], sizeof(rec->m_field33512213041));
    memcpy(&rec->m_uiSkin, &row[0x00B0], sizeof(rec->m_uiSkin));
    memcpy(&rec->m_flagsB, &row[0x00B4], sizeof(rec->m_flagsB));
    memcpy(&rec->m_field33512213044, &row[0x00B8], sizeof(rec->m_field33512213044));
    memcpy(&rec->m_field33512213045, &row[0x00BC], sizeof(rec->m_field33512213045));
    memcpy(&rec->m_field33512213046, &row[0x00C0], sizeof(rec->m_field33512213046));
    memcpy(&rec->m_field33512213047, &row[0x00C4], sizeof(rec->m_field33512213047));
    memcpy(&rec->m_field33512213048, &row[0x00C8], sizeof(rec->m_field33512213048));
    memcpy(&rec->m_field33512213049, &row[0x00CC], sizeof(rec->m_field33512213049));
    memcpy(&rec->m_field33512213050, &row[0x00D0], sizeof(rec->m_field33512213050));
    memcpy(&rec->m_field33512213051, &row[0x00D4], sizeof(rec->m_field33512213051));
    memcpy(&rec->m_field33512213052, &row[0x00D8], sizeof(rec->m_field33512213052));
    memcpy(&rec->m_field33512213053, &row[0x00DC], sizeof(rec->m_field33512213053));
    memcpy(&rec->unk_00E0, &row[0x00E0], sizeof(rec->unk_00E0));
    memcpy(&rec->unk_00E4, &row[0x00E4], sizeof(rec->unk_00E4));
    memcpy(&rec->unk_00E8, &row[0x00E8], sizeof(rec->unk_00E8));
    memcpy(&rec->unk_00EC, &row[0x00EC], sizeof(rec->unk_00EC));
    memcpy(&rec->unk_00F0, &row[0x00F0], sizeof(rec->unk_00F0));
    memcpy(&rec->unk_00F4, &row[0x00F4], sizeof(rec->unk_00F4));
    memcpy(&rec->unk_00F8, &row[0x00F8], sizeof(rec->unk_00F8));
    memcpy(&rec->unk_00FC, &row[0x00FC], sizeof(rec->unk_00FC));
    memcpy(&rec->unk_0100, &row[0x0100], sizeof(rec->unk_0100));
    memcpy(&rec->unk_0104, &row[0x0104], sizeof(rec->unk_0104));
}
#endif
