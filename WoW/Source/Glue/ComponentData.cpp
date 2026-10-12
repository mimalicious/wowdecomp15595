#include "ComponentData.h"

// OFFSET: 0x00481DC0/0x1400A0AB0
ComponentData::ComponentData() {
    this->m_flags &= ~0x3u;
    this->m_preferences.m_raceID = 0;
    this->m_preferences.m_sexID = 0;
    this->m_preferences.m_classId = 0;
    this->m_preferences.m_hairColorId = 0;
    this->m_preferences.m_skinID = 0;
    this->m_preferences.m_faceId = 0;
    this->m_preferences.m_facialHairId = 0;
    this->m_preferences.m_hairStyleId = 0;
    this->m_npcSkinTexture[0] = 0;
    this->m_model = NULL;
    this->m_geosets[0] = 1;
    this->m_geosets[1] = 101;
    this->m_geosets[2] = 201;
    this->m_geosets[3] = 301;
    this->m_geosets[7] = 702;
    this->m_geosets[4] = 401;
    this->m_geosets[5] = 501;
    this->m_geosets[6] = 601;
    this->m_geosets[8] = 801;
    this->m_geosets[9] = 901;
    this->m_geosets[10] = 1001;
    this->m_geosets[11] = 1101;
    this->m_geosets[12] = 1201;
    this->m_geosets[13] = 1301;
    this->m_geosets[14] = 1401;
    this->m_geosets[15] = 1501;
    this->m_geosets[16] = 1601;
    this->m_geosets[17] = 1701;
    this->m_geosets[18] = 1801;
}

// OFFSET: 0x006F9CC0
void ComponentData::GetPreferences(CharacterPreferences* preferences) {
    if (!preferences) {
        return;
    }

    *preferences = this->m_preferences;
}

// OFFSET: 0x006F9C80
void ComponentData::SetPreferences(const CharacterPreferences* preferences) {
    if (!preferences) {
        return;
    }

    this->m_preferences.m_raceID = preferences->m_raceID;
    this->m_preferences.m_sexID = preferences->m_sexID;
    this->m_preferences.m_classId = preferences->m_classId;
    this->m_preferences.m_hairColorId = preferences->m_hairColorId;
    this->m_preferences.m_skinID = preferences->m_skinID;
    this->m_preferences.m_faceId = preferences->m_faceId;
    this->m_preferences.m_facialHairId = preferences->m_facialHairId;
    this->m_preferences.m_hairStyleId = preferences->m_hairStyleId;
}
