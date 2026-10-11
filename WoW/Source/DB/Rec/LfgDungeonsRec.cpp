#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsGroupingMapRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"

// STATIC: 0x00C236A8/0x140A189D0
static const int32_t s_fieldOffsets[21] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80 };

// STATIC: 0x00C23700/0x140A18A30
static const int32_t s_fieldSizes[21] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23758/0x140A18A90
static const int32_t s_fieldTypes[21] = { 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 };

// STATIC: 0x00C237AC/0x140A18AE8
const DBCMeta LfgDungeonsRec::s_meta = { "DBFilesClient\\LfgDungeons.dbc", 21, 84, LfgDungeonsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: cc2f18a1 IDENTICAL/NONE
// OFFSET: 0x00A2EAD0/0x1407D81D0
void LfgDungeonsRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonsRec* rec = static_cast<LfgDungeonsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_textureFilename, strings);
    DBCFixupString(rec->m_description, strings);
}

// OFFSET: 0x00B02530
int32_t LfgDungeonsRec::GetGroupSize() const {
    return this->m_countTank + this->m_countHealer + this->m_countDamage;
}

// OFFSET: 0x00B02540
bool LfgDungeonsRec::IsHeroic() const {
    MapRec* map = g_mapDB.GetRecord(this->m_mapID);

    if (map) {
        int32_t instanceType = map->m_instanceType;

        if (instanceType == 1) {
            return this->m_difficulty > 0;
        }

        if (instanceType == 2) {
            return this->m_difficulty > 1;
        }
    }

    return false;
}

// OFFSET: 0x00B02590
bool LfgDungeonsRec::IsRaid() const {
    MapRec* map = g_mapDB.GetRecord(this->m_mapID);
    return map && map->m_instanceType == 2;
}

// OFFSET: 0x00B025D0
bool LfgDungeonsRec::IsInRandomGroup(int32_t randomID) const {
    if (this->m_randomID == randomID) {
        return true;
    }

    for (int32_t i = 0; i < g_lfgDungeonsGroupingMapDB.m_numRecords; i++) {
        LfgDungeonsGroupingMapRec* rec = g_lfgDungeonsGroupingMapDB.GetRecordByIndex(i);

        if (rec->m_lfgDungeonsID == this->m_id && rec->m_randomLfgDungeonsID == randomID) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00B02640
int32_t LfgDungeonsRec::GetTopGroupID() const {
    int32_t groupID = this->m_groupID;
    LfgDungeonGroupRec* group = g_lfgDungeonGroupDB.GetRecord(groupID);

    for (int32_t i = 0; i < g_lfgDungeonsGroupingMapDB.m_numRecords; i++) {
        LfgDungeonsGroupingMapRec* rec = g_lfgDungeonsGroupingMapDB.GetRecordByIndex(i);

        if (rec->m_lfgDungeonsID == this->m_id) {
            LfgDungeonGroupRec* other = g_lfgDungeonGroupDB.GetRecord(rec->m_groupID);

            if (other && (!group || other->m_orderIndex > group->m_orderIndex)) {
                group = other;
            }
        }
    }

    return group ? group->m_id : groupID;
}
