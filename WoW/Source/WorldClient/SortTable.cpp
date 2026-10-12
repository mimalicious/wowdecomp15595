#include "WoW/Source/WorldClient/SortTable.h"

// OFFSET: 0x00799BD0/0x14048DDC0
void CSortTable::SetPlanes(const C3Vector& normal, const C3Vector& pos) {
    float dist = 0.0f;

    for (int32_t i = 0; i < this->m_numEntries; i++) {
        CSortEntry& entry = this->m_entries.m_data[i];
        entry.m_table = this;
        float x = pos.x + normal.x * dist;
        float y = normal.y * dist + pos.y;
        float z = normal.z * dist + pos.z;
        entry.m_plane.n = normal;
        entry.m_plane.d = -(normal.x * x + normal.z * z + normal.y * y);
        dist += 33.333332f;
    }
}

// OFFSET: 0x00799E20/0x14048E0A0
void CSortTable::Clear(TSExplicitList<CMapEntity, offsetof(CMapEntity, m_viewLink)>& entities) {
    for (int32_t i = 0; i < this->m_numEntries; i++) {
        CSortEntry& entry = this->m_entries.m_data[i];
        entry.m_chunks.SetCount(0);
        entry.m_lowDetailAreas.m_count = 0;
        entry.m_lowDetailMapObjDefs.m_count = 0;
        entry.m_lowDetailOceans.m_count = 0;
        entry.m_mapObjDefGroups.UnlinkAll();
        entities.Combine(&entry.m_entities, 2, NULL);

        CWorldOccluder* occluder = entry.m_occluders.Head();

        while (occluder) {
            CWorldOccluder* next = entry.m_occluders.Next(occluder);
            occluder->Free();
            occluder = next;
        }
    }

    this->m_doodadDefs.m_count = 0;
}

// OFFSET: 0x00799F60
CSortEntry::~CSortEntry() {
}

// OFFSET: 0x0079A1F0
CSortEntry::CSortEntry() {
    this->m_plane.n = C3Vector(0.0f, 0.0f, 1.0f);
    this->m_plane.d = 0.0f;
    this->m_chunks.m_chunk = 16;
    this->m_lowDetailAreas.m_chunk = 8;
    this->m_lowDetailMapObjDefs.m_chunk = 8;
    this->m_lowDetailOceans.m_chunk = 8;
    this->m_doodadDefHead = -1;
}

// OFFSET: 0x0079A410/0x14048E8F0
CSortTable::~CSortTable() {
}

// OFFSET: 0x0079A4F0/0x14048EA40
CSortTable::CSortTable() {
    this->m_entries.m_chunk = 16;
    this->m_doodadDefs.m_chunk = 256;
    this->m_numEntries = 0;
    this->m_lastEntry = 0;
}

// OFFSET: 0x0079A520/0x14048EA70
void CSortTable::Setup(float farClip, const C3Vector& normal, const C3Vector& pos) {
    int32_t count = 3 - static_cast<int32_t>(farClip * -0.029999999f);

    if (this->m_numEntries != count) {
        this->m_numEntries = count;
        this->m_lastEntry = count - 1;
        this->m_entries.SetCount(count);
    }

    this->SetPlanes(normal, pos);
}

// OFFSET: 0x0075A830/0x14043D570
void CSortTable::AddDoodadDef(CMapDoodadDef* doodadDef, int32_t entry) {
    CSortEntryDoodadDef* node = this->m_doodadDefs.New();
    node->m_doodadDef = doodadDef;
    node->m_next = this->m_entries.m_data[entry].m_doodadDefHead;
    this->m_entries.m_data[entry].m_doodadDefHead = this->m_doodadDefs.Count() - 1;
}

// OFFSET: 0x00408540/0x14000A870
void CSortTable::Finish() {
    this->m_doodadDefs.m_count = 0;
}
