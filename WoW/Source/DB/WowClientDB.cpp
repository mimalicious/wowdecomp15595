#include "WoW/Source/DB/WowClientDB.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

bool g_clientDbPerformCompress;

// STATIC: 0x00C9AA10/0x140B33A00
ClientDBTable g_staticDBTables[] = {
    { &g_soundEntriesAdvancedDB, &SoundEntriesAdvancedRec::s_meta },
    { &g_soundEntriesDB, &SoundEntriesRec::s_meta },
    { NULL, NULL },
};

// STATIC: 0x00C9AA28/0x140B33A30
ClientDBTable g_staticDBGlueTables[] = {
    { &g_animationDataDB, &AnimationDataRec::s_meta },
    { &g_areaTableDB, &AreaTableRec::s_meta },
    { &g_lightIntBandDB, &LightIntBandRec::s_meta },
    { &g_lightParamsDB, &LightParamsRec::s_meta },
    { &g_mapDB, &MapRec::s_meta },
    { NULL, NULL },
};

// STATIC: 0x00C9AA58/0x140B33A90
ClientDBTable g_staticDBWorldTables[] = {
    { &g_footprintTexturesDB, &FootprintTexturesRec::s_meta },
    { &g_groundEffectDoodadDB, &GroundEffectDoodadRec::s_meta },
    { &g_groundEffectTextureDB, &GroundEffectTextureRec::s_meta },
    { &g_lightDB, &LightRec::s_meta },
    { &g_lightFloatBandDB, &LightFloatBandRec::s_meta },
    { &g_lightSkyboxDB, &LightSkyboxRec::s_meta },
    { &g_liquidMaterialDB, &LiquidMaterialRec::s_meta },
    { &g_liquidObjectDB, &LiquidObjectRec::s_meta },
    { &g_liquidTypeDB, &LiquidTypeRec::s_meta },
    { &g_soundEmitterPillPointsDB, &SoundEmitterPillPointsRec::s_meta },
    { &g_soundEmittersDB, &SoundEmittersRec::s_meta },
    { &g_soundEntriesFallbacksDB, &SoundEntriesFallbacksRec::s_meta },
    { &g_terrainMaterialDB, &TerrainMaterialRec::s_meta },
    { &g_terrainTypeDB, &TerrainTypeRec::s_meta },
    { &g_weatherDB, &WeatherRec::s_meta },
    { &g_wmoAreaTableDB, &WMOAreaTableRec::s_meta },
    { &g_zoneLightDB, &ZoneLightRec::s_meta },
    { &g_zoneLightPointDB, &ZoneLightPointRec::s_meta },
    { NULL, NULL },
};

// OFFSET: 0x0048AAD0/0x1400ADB00
void StaticDBUnloadAll() {
    for (ClientDBTable* table = g_staticDBTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }

    for (ClientDBTable* table = g_staticDBGlueTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }

    for (ClientDBTable* table = g_staticDBWorldTables; table->m_db; table++) {
        table->m_db->Unload(table->m_meta);
    }
}

// INLINED
WowClientDB_Common::WowClientDB_Common() {
    this->m_numRecords = -1;
    this->m_maxID = 0;
    this->m_minID = 0;
}

// PROVEN: 854c3dfe IDENTICAL/DIFFERENT
// OFFSET: 0x0048AB40/0x1400ADBA0
void WowClientDB_Common::Load(const DBCMeta* meta) {
    char error[256];
    SFileImpl* file;

    SFileOpenFile(meta->m_filename, &file);

    if (!file) {
        SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
        SErrDisplayAppFatal("Unable to open %s: ", static_cast<const char*>(meta->m_filename), error);
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    uint8_t* data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

    if (!SFile::Read(file, data, size, NULL)) {
        SMemFree(data, __FILE__, __LINE__, 0);
        SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
        SErrDisplayAppFatal("Unable to read data for %s: %s", static_cast<const char*>(meta->m_filename), error);
    }

    SFileCloseFile(file);

    uint32_t* header = reinterpret_cast<uint32_t*>(data);

    if (header[0] != 'CBDW') {
        SErrDisplayAppFatal("Invalid signature 0x%x from %s", header[0], static_cast<const char*>(meta->m_filename));
    }

    if (header[3] != static_cast<uint32_t>(meta->m_rowSize)) {
        SErrDisplayAppFatalCustom(0x85100079, "%s has wrong row size (found %i, expected %i)", static_cast<const char*>(meta->m_filename), header[3], meta->m_rowSize);
    }

    uint8_t* rows = &data[20];
    this->LoadRecords(meta, data, header[1], rows, header[4], reinterpret_cast<const char*>(&rows[header[1] * meta->m_rowSize]));
}

// PROVEN: 1e707c58 IDENTICAL/IDENTICAL
// OFFSET: 0x0048AC70/0x1400ADD20
void WowClientDB_Common::Unload(const DBCMeta* meta) {
    this->Free();
    this->m_numRecords = 0;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;
}

// PROVEN: b563673a IDENTICAL/IDENTICAL
// OFFSET: 0x0048ACA0/0x1400ADD50
WowClientDB_Base::WowClientDB_Base() {
    this->m_data = NULL;
    this->m_records = NULL;
    this->m_recordsById = NULL;
}

// PROVEN: 8dd94dbb IDENTICAL/DIFFERENT
// OFFSET: 0x0048ACD0/0x1400ADD80
void WowClientDB_Base::Free() {
    SMemFree(this->m_data, __FILE__, __LINE__, 0);
    this->m_data = NULL;
    this->m_records = NULL;
    SMemFree(this->m_recordsById, __FILE__, __LINE__, 0);
    this->m_recordsById = NULL;
}

// OFFSET: 0x0048AD20/0x1400ADDE0
void WowClientDB_Base::LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) {
    this->m_data = data;
    this->m_records = NULL;
    this->m_recordsById = NULL;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;
    this->m_numRecords = numRecords;

    if (!numRecords) {
        return;
    }

    this->m_records = rows;

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        uint8_t* record = &this->m_records[i * meta->m_rowSize];
        meta->m_fixupStrings(record, strings);

        int32_t id = *reinterpret_cast<int32_t*>(&record[meta->m_idOffset]);
        this->m_maxID = id > this->m_maxID ? id : this->m_maxID;
        this->m_minID = id < this->m_minID ? id : this->m_minID;
    }

    this->m_recordsById = static_cast<uint8_t**>(SMemAlloc((this->m_maxID - this->m_minID) * sizeof(uint8_t*) + sizeof(uint8_t*), __FILE__, __LINE__, 8));

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        uint8_t* record = &this->m_records[i * meta->m_rowSize];
        this->m_recordsById[*reinterpret_cast<int32_t*>(&record[meta->m_idOffset]) - this->m_minID] = record;
    }
}

// PROVEN: 0d82be0b IDENTICAL/IDENTICAL
// OFFSET: 0x0048AE00/0x1400ADEF0
WowClientDB_RLE_Base::WowClientDB_RLE_Base() {
    this->m_data = NULL;
    this->m_strings = NULL;
    this->m_records = NULL;
    this->m_recordsById = NULL;
}

// PROVEN: 8ce3b1ac IDENTICAL/IDENTICAL
// OFFSET: 0x0048AE30/0x1400ADF20
void WowClientDB_RLE_Base::Free() {
    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
        this->m_data = NULL;
    }

    if (this->m_strings) {
        SMemFree(this->m_strings, __FILE__, __LINE__, 0);
        this->m_strings = NULL;
    }

    if (this->m_records) {
        SMemFree(this->m_records, __FILE__, __LINE__, 0);
        this->m_records = NULL;
    }

    if (this->m_recordsById) {
        SMemFree(this->m_recordsById, __FILE__, __LINE__, 0);
        this->m_recordsById = NULL;
    }
}

// OFFSET: 0x0048AF10/0x1400AE040
void WowClientDB_RLE_Base::LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) {
    this->m_meta = meta;
    this->m_data = NULL;
    this->m_records = NULL;
    this->m_recordsById = NULL;
    this->m_strings = NULL;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;
    this->m_numRecords = numRecords;

    if (!numRecords) {
        return;
    }

    this->m_strings = static_cast<char*>(SMemAlloc(stringSize, __FILE__, __LINE__, 0));
    memcpy(this->m_strings, strings, stringSize);

    for (int32_t i = 0; i < this->m_numRecords; i++) {
        int32_t id = *reinterpret_cast<int32_t*>(&rows[i * meta->m_rowSize + meta->m_idOffset]);
        this->m_maxID = id > this->m_maxID ? id : this->m_maxID;
        this->m_minID = id < this->m_minID ? id : this->m_minID;
    }

    this->m_records = static_cast<uint8_t**>(SMemAlloc(sizeof(uint8_t*) * this->m_numRecords, __FILE__, __LINE__, 0));
    this->m_recordsById = static_cast<uint8_t**>(SMemAlloc(sizeof(uint8_t*) * (this->m_maxID - this->m_minID) + sizeof(uint8_t*), __FILE__, __LINE__, 8));

    if (g_clientDbPerformCompress) {
        uint32_t size = 0;

        for (int32_t i = 0; i < this->m_numRecords; i++) {
            size += CompressRow(&rows[i * meta->m_rowSize], meta->m_rowSize, NULL);
        }

        uint8_t* dst = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
        this->m_data = dst;

        for (int32_t i = 0; i < this->m_numRecords; i++) {
            uint8_t* row = &rows[i * meta->m_rowSize];
            int32_t id = *reinterpret_cast<int32_t*>(&row[meta->m_idOffset]);
            this->m_records[i] = dst;
            this->m_recordsById[id - this->m_minID] = dst;
            dst += CompressRow(row, meta->m_rowSize, dst);
        }

        SMemFree(data, __FILE__, __LINE__, 0);
    } else {
        this->m_data = data;

        for (int32_t i = 0; i < this->m_numRecords; i++) {
            uint8_t* row = &rows[i * meta->m_rowSize];
            int32_t id = *reinterpret_cast<int32_t*>(&row[meta->m_idOffset]);
            this->m_records[i] = row;
            this->m_recordsById[id - this->m_minID] = row;
        }
    }
}

// OFFSET: 0x0048B0E0/0x1400AE280
void WowClientDB_RLE_Base::CopyRecord(void* rec, const uint8_t* row, uint8_t* buffer) const {
    uint8_t* dst = static_cast<uint8_t*>(rec);
    uint32_t offset = 0;

    if (g_clientDbPerformCompress) {
        DecompressRow(row, this->m_meta->m_rowSize, buffer);
        row = buffer;
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(this->m_meta->m_numColumns); i++) {
        const uint8_t* src = &row[this->m_meta->m_fieldOffsets[i]];
        int32_t count = this->m_meta->m_fieldSizes[i] / 4;

        switch (this->m_meta->m_fieldTypes[i]) {
            case 0:
                for (; count; count--) {
                    offset += -offset & (sizeof(int32_t) - 1);
                    *reinterpret_cast<int32_t*>(&dst[offset]) = *reinterpret_cast<const int32_t*>(src);
                    offset += sizeof(int32_t);
                    src += sizeof(int32_t);
                }

                break;

            case 1:
                for (; count; count--) {
                    offset += -offset & (sizeof(int64_t) - 1);
                    *reinterpret_cast<int64_t*>(&dst[offset]) = *reinterpret_cast<const int64_t*>(src);
                    offset += sizeof(int64_t);
                    src += sizeof(int64_t);
                }

                break;

            case 2:
                for (; count; count--) {
                    offset += -offset & (sizeof(const char*) - 1);
                    *reinterpret_cast<const char**>(&dst[offset]) = &this->m_strings[*reinterpret_cast<const uint32_t*>(src)];
                    offset += sizeof(const char*);
                    src += sizeof(uint32_t);
                }

                break;

            case 3:
                for (int32_t k = 0; k < count; k++) {
                    offset += -offset & (sizeof(float) - 1);
                    *reinterpret_cast<float*>(&dst[offset]) = *reinterpret_cast<const float*>(src);
                    offset += sizeof(float);
                    src += sizeof(float);
                }

                break;
        }
    }
}

// PROVEN: 070f929c IDENTICAL/DIFFERENT
// OFFSET: 0x0048AEB0/0x1400ADFD0
uint8_t* DecompressRow(const uint8_t* src, uint32_t size, uint8_t* dst) {
    uint8_t* end = &dst[size];
    *dst = *src;
    uint8_t* out = dst + 1;

    for (const uint8_t* p = src + 1; out < end; p++) {
        *out++ = *p;

        if (*p == p[-1]) {
            int32_t run = p[1];

            while (run) {
                run--;
                *out++ = *p;
            }

            p += 2;

            if (out < end) {
                *out++ = *p;
            }
        }
    }

    return out;
}

// OFFSET: 0x00872C70/0x1405A9270
uint32_t CompressRow(const uint8_t* src, uint32_t size, uint8_t* dst) {
    uint8_t* out = dst;
    const uint8_t* end = &src[size];

    if (dst) {
        *dst = *src;
        out = dst + 1;
    }

    uint32_t count = 1;

    for (const uint8_t* p = src + 1; p < end; p++) {
        if (out) {
            *out++ = *p;
        }

        uint8_t value = *p;
        count++;

        if (*p == p[-1]) {
            p++;

            uint32_t run;

            for (run = 0; p < end; run++) {
                if (*p != value || run >= 0xFF) {
                    break;
                }

                p++;
            }

            if (out) {
                *out++ = static_cast<uint8_t>(run);
            }

            count++;

            if (p == end) {
                return count;
            }

            if (out) {
                *out++ = *p;
            }

            count++;
        }
    }

    return count;
}
