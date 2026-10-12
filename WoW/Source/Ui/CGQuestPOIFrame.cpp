#include "CGQuestPOIFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/WorldMap.h"

// STATIC: 0x00D17660/0x140BD24E8
const char* CGQuestPOIFrame::s_objectTypeName = "QuestPOIFrame";

// STATIC: 0x00D1767C/0x140BD2520
int32_t CGQuestPOIFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00F76C40/0x140E6CD68
int32_t CGQuestPOIFrame::s_objectType;

// STATIC: 0x00F76C44
CDataAllocator CGQuestPOIFrame::s_questPOIHeap(sizeof(CGQuestPOIFrame), 5);

// INLINED
void CGQuestPOIFrame::operator delete(void* ptr) {
    CGQuestPOIFrame::s_questPOIHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008C6450/0x1406364A0
CSimpleFrame* CGQuestPOIFrame::Create(CSimpleFrame* parent) {
    void* data = CGQuestPOIFrame::s_questPOIHeap.GetData(0, typeid(CGQuestPOIFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGQuestPOIFrame(parent) : NULL;
}

// OFFSET: 0x00A02260/0x1407A1190
CGQuestPOIFrame::CGQuestPOIFrame(CSimpleFrame* parent) : CGBlobFrame(parent) {
}

// PROVEN: 3b4a4229 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02290/0x1407A11C0
const char* CGQuestPOIFrame::GetObjectTypeName() {
    return CGQuestPOIFrame::s_objectTypeName;
}

// PROVEN: 33061a17 IDENTICAL/IDENTICAL
// OFFSET: 0x00A022A0/0x1407A11D0
int32_t CGQuestPOIFrame::GetScriptMetaTable() {
    return CGQuestPOIFrame::s_metatable;
}

// PROVEN: 7cbf9dfa IDENTICAL/UNPROVEN
// OFFSET: 0x00A022B0/0x1407A11E0
void CGQuestPOIFrame::DrawBlob(int32_t id, bool show) {
    if (show) {
        QuestPOIInfo* info = CGQuestLog::GetQuestPOIData(id);

        if (info) {
            this->AddBlob(info, true);
        }
    } else {
        this->RemoveBlob(id);
    }
}

// PROVEN: bda78837 NONE/IDENTICAL
// OFFSET: 0x00A02350/0x1407A1290
bool CGQuestPOIFrame::IsA(int32_t type) {
    return type == CGQuestPOIFrame::s_objectType || type == CGBlobFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00A02390/0x1407A12C0
bool CGQuestPOIFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGQuestPOIFrame::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CGBlobFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x00A023E0
CGQuestPOIFrame::~CGQuestPOIFrame() {
}

// OFFSET: 0x00A02420/0x1407A1370
void CGQuestPOIFrame::UpdateMouseOverPOI(float x, float y) {
    uint32_t count = 0;
    this->m_mouseOverQuestId = 0;
    this->m_mouseOverCount = 0;
    this->m_mouseOverObjectives[0] = 0;
    this->m_mouseOverObjectives[1] = 0;
    this->m_mouseOverObjectives[2] = 0;
    this->m_mouseOverObjectives[3] = 0;

    if (x < 0.0f || y < 0.0f || x > 1.0f || y > 1.0f) {
        return;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());

    if (CGWorldMap::GetCurrentAreaID() == -1 || !area) {
        return;
    }

    int32_t questId = 0;
    C3Vector point;
    point.z = 0.0f;
    point.x = (this->m_rect.maxX - this->m_rect.minX) * x + this->m_rect.minX;
    point.y = (this->m_rect.maxY - this->m_rect.minY) * (1.0f - y) + this->m_rect.minY;

    for (uint32_t i = 0; i < 4; i++) {
        BlobRenderGroup* group = &this->m_groups[i];

        if (!group->m_info || !group->m_info->m_visible) {
            continue;
        }

        for (uint32_t j = 0; j < group->m_renderData.Count(); j++) {
            int32_t mask = group->m_info->m_objectiveMask;

            if (mask == -1) {
                continue;
            }

            BlobRenderData* data = &group->m_renderData.m_data[j];

            if (!((1 << data->m_blob->m_objectiveIndex) & mask) || !CGQuestLog::IsPOIVisibleOnWorldMap(data->m_blob->m_center, data->m_blob->m_mapId, area->m_mapID)) {
                continue;
            }

            if (data->m_fillVerts.Count() < 3 || !data->m_valid) {
                continue;
            }

            if (!NTempest::Intersect(point, data->m_fillVerts.m_data, data->m_fillVerts.Count() - 1, C3Vector::AXIS_Z)) {
                continue;
            }

            count = data->m_numObjectives;
            questId = group->m_info->m_questId;

            for (uint32_t k = 0; k < data->m_numObjectives; k++) {
                this->m_mouseOverObjectives[k] = data->m_objectives[k];
            }
        }

        if (count) {
            break;
        }
    }

    if (questId && count) {
        this->m_mouseOverQuestId = questId;
        this->m_mouseOverCount = count;
    }
}

// OFFSET: 0x00A02660/0x1407A1650
void CGQuestPOIFrame::MergeBlobs(uint32_t index) {
    BlobRenderGroup* group = &this->m_groups[index];
    int32_t mask = group->m_info->m_objectiveMask;

    if (mask == -1) {
        return;
    }

    for (uint32_t i = 0; i < group->m_renderData.Count(); i++) {
        BlobRenderData* first = &group->m_renderData.m_data[i];

        if (!first->m_valid || !((1 << first->m_blob->m_objectiveIndex) & mask)) {
            continue;
        }

        for (uint32_t j = i + 1; j < group->m_renderData.Count(); j++) {
            BlobRenderData* second = &group->m_renderData.m_data[j];

            if (!second->m_valid || !((1 << second->m_blob->m_objectiveIndex) & mask)) {
                continue;
            }

            CRect overlap = first->m_bounds & second->m_bounds;

            if (!(overlap.maxY > overlap.minY) || !(overlap.maxX > overlap.minX)) {
                continue;
            }

            BlobRenderData* smaller;
            BlobRenderData* larger;

            if ((second->m_bounds.maxX - second->m_bounds.minX) * (second->m_bounds.maxY - second->m_bounds.minY) <= (first->m_bounds.maxX - first->m_bounds.minX) * (first->m_bounds.maxY - first->m_bounds.minY)) {
                smaller = second;
                larger = first;
            } else {
                smaller = first;
                larger = second;
            }

            uint32_t inside = 0;

            for (uint32_t k = 0; k < smaller->m_fillVerts.Count() - 1; k++) {
                if (NTempest::Intersect(smaller->m_fillVerts.m_data[k], larger->m_fillVerts.m_data, larger->m_fillVerts.Count() - 1, C3Vector::AXIS_Z)) {
                    inside++;
                }
            }

            if (!(static_cast<float>(inside) / static_cast<float>(smaller->m_fillVerts.Count() - 1) > this->m_mergeThreshold)) {
                continue;
            }

            smaller->m_valid = 0;

            for (uint32_t k = 0; k < smaller->m_numObjectives; k++) {
                if (larger->m_numObjectives >= 4) {
                    continue;
                }

                uint32_t m = 0;

                while (m < larger->m_numObjectives && larger->m_objectives[m] != smaller->m_objectives[k]) {
                    m++;
                }

                if (m >= larger->m_numObjectives) {
                    larger->m_objectives[larger->m_numObjectives++] = smaller->m_objectives[k];
                }
            }
        }
    }
}
