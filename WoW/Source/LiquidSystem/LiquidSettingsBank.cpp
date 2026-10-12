#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00E4EB90
uint32_t Liquid::CSettingsBank::s_refCount;

// STATIC: 0x00E4EB94/0x140D37F70
CDataAllocator Liquid::s_settingsAllocator(sizeof(Liquid::CMaterialSettings), 32);

// STATIC: 0x00E4EBA8/0x140D37F90
TSGrowableArray<Liquid::CMaterialSettings*> Liquid::CSettingsBank::s_settings;

// PROVEN: 2356bc85 IDENTICAL/NONE
// OFFSET: 0x0071BF50
void Liquid::CSettingsBank::Create() {
    Liquid::CSettingsBank::s_refCount++;
}

// OFFSET: 0x0071C000/0x1403F0DD0
int32_t Liquid::CSettingsBank::UpdateRecord(uint32_t liquidType, CMaterialSettings* settings) {
    if (!settings) {
        if (liquidType >= Liquid::CSettingsBank::s_settings.Count()) {
            return 0;
        }

        settings = Liquid::CSettingsBank::s_settings.m_data[liquidType];

        if (!settings) {
            return 0;
        }
    }

    settings->Destroy();

    LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(liquidType);

    if (!rec) {
        return 0;
    }

    LiquidMaterialRec* materialRec = g_liquidMaterialDB.GetRecord(rec->m_materialID);

    if (!materialRec) {
        return 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        SStrCopy(settings->m_textureNames[i], rec->m_texture[i], sizeof(settings->m_textureNames[i]));
    }

    *reinterpret_cast<int32_t*>(&settings->m_colors[0]) = rec->m_color[0];
    *reinterpret_cast<int32_t*>(&settings->m_colors[1]) = rec->m_color[1];

    for (uint32_t i = 0; i < 4; i++) {
        settings->m_ints[i] = rec->m_int[i];
    }

    for (uint32_t i = 0; i < 18; i++) {
        settings->m_floats[i] = rec->m_float[i];
    }

    settings->m_transparent = materialRec->m_flags & 0x1;
    settings->Initialize(liquidType);
    return 1;
}

// OFFSET: 0x0071C3B0/0x1403F1290
void Liquid::CSettingsBank::Purge() {
    for (uint32_t i = 0; i < Liquid::CSettingsBank::s_settings.Count(); i++) {
        CMaterialSettings* settings = Liquid::CSettingsBank::s_settings.m_data[i];

        if (settings) {
            settings->Destroy();
            settings->~CMaterialSettings();
            Liquid::s_settingsAllocator.PutData(settings, NULL, 0);
        }
    }

    Liquid::CSettingsBank::s_settings.m_count = 0;
}

// OFFSET: 0x0071C410/0x1403F1330
Liquid::CMaterialSettings* Liquid::CSettingsBank::GetSettings(uint32_t liquidType) {
    while (true) {
        if (liquidType < Liquid::CSettingsBank::s_settings.Count() && Liquid::CSettingsBank::s_settings.m_data[liquidType]) {
            return Liquid::CSettingsBank::s_settings.m_data[liquidType];
        }

        CMaterialSettings* settings = Liquid::s_settingsAllocator.New<CMaterialSettings>(0);

        if (Liquid::CSettingsBank::UpdateRecord(liquidType, settings)) {
            Liquid::CSettingsBank::s_settings.GrowToFit(liquidType, 1);
            Liquid::CSettingsBank::s_settings.m_data[liquidType] = settings;
            return settings;
        }

        SysMsgPrintf(STATUS_ERROR, 2, "Settings Bank: Liquid type [%d] not found, defaulting to water!", liquidType);
        liquidType = 1;
    }
}

// PROVEN: 9695e1b4 IDENTICAL/DIFFERENT
// OFFSET: 0x0071C4A0/0x1403F1400
void Liquid::CSettingsBank::Destroy() {
    if (!--Liquid::CSettingsBank::s_refCount) {
        Liquid::CSettingsBank::Purge();
    }

    Liquid::s_settingsAllocator.Clear(typeid(Liquid::CMaterialSettings).raw_name(), SLOG_OBJECT, 1);
}
