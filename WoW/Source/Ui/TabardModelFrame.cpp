#include "TabardModelFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdlib>
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: #0x00ACF580
const char* CGTabardModelFrame::s_objectTypeName = "TabardModel";

// STATIC: #0x00ACF584
int32_t CGTabardModelFrame::s_metatable = LUA_NOREF;

// STATIC: #0x00C0E520
int32_t CGTabardModelFrame::s_objectType;

// STATIC: #0x00C0E524
CDataAllocator CGTabardModelFrame::s_tabardModelHeap(sizeof(CGTabardModelFrame), 1);

// STATIC: #0x00A14908
static const int32_t s_tabardVariationCounts[5] = { 170, 17, 6, 17, 51 };

// INLINED
void CGTabardModelFrame::operator delete(void* ptr) {
    CGTabardModelFrame::s_tabardModelHeap.PutData(ptr, NULL, 0);
}

// OFFSET: #0x00514350
CSimpleFrame* CGTabardModelFrame::Create(CSimpleFrame* parent) {
    if (CGTabardModelFrame::s_tabardModelHeap.m_bytesPerData < sizeof(CGTabardModelFrame)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGTabardModelFrame::s_tabardModelHeap.GetData(0, typeid(CGTabardModelFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGTabardModelFrame(parent) : NULL;
}

// OFFSET: #0x00598F50
CGTabardModelFrame::CGTabardModelFrame(CSimpleFrame* parent) : CGCharacterModelBase(parent) {
}

// OFFSET: #0x00598F80
const char* CGTabardModelFrame::GetObjectTypeName() {
    return CGTabardModelFrame::s_objectTypeName;
}

// OFFSET: %0x00942B90
int32_t CGTabardModelFrame::GetScriptMetaTable() {
    return CGTabardModelFrame::s_metatable;
}

// OFFSET: [#0x005990C0, #0x00599190]
CGTabardModelFrame::~CGTabardModelFrame() {
}

// OFFSET: #0x005990D0
bool CGTabardModelFrame::IsA(int32_t type) {
    return type == CGTabardModelFrame::s_objectType || type == CGCharacterModelBase::s_objectType || type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: #0x00599120
bool CGTabardModelFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGTabardModelFrame::s_objectTypeName, STORM_MAX_STR) || CGCharacterModelBase::IsA(typeName);
}

// PROVEN: 5e061413 IDENTICAL/NONE
// OFFSET: 0x00A05FF0/0x140171140
void EmblemTextureUpdate(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch) {
        texelStrideInBytes = 4 * width;
        texels = static_cast<TSGrowableArray<CImVector>*>(userArg)->m_data;
    }
}

// OFFSET: #0x005993B0
void CGTabardModelFrame::ApplyGuildColorFromPlayer() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->m_characterComponent) {
        player->m_characterComponent->ApplyGuildColor(this->m_variations[0], this->m_variations[1], this->m_variations[2], this->m_variations[3], this->m_variations[4]);
    }
}

// OFFSET: #0x00599410
void CGTabardModelFrame::Save() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->SaveTabard(this->m_variations[0], this->m_variations[1], this->m_variations[2], this->m_variations[3], this->m_variations[4], CGGameUI::m_interactTarget);
    }
}

// OFFSET: #0x00599470
void CGTabardModelFrame::CycleVariation(uint32_t variation, int32_t delta) {
    int32_t count = s_tabardVariationCounts[variation];

    if (abs(delta) < count) {
        this->m_variations[variation] += delta + count;
        this->m_variations[variation] %= s_tabardVariationCounts[variation];
        this->ApplyGuildColorFromPlayer();
    }
}

// OFFSET: #0x005995D0
void CGTabardModelFrame::OnModelLoaded(CM2Model* model) {
    CGCharacterModelBase::OnModelLoaded(model);

    CCamera* camera = this->m_camera;

    if (!camera) {
        return;
    }

    M2SequenceInfo info;
    info.m_radius = 0.0f;
    model->GetSequenceInfo(0, 0, info);

    C3Vector coord(0.0f, (info.m_bounds.t.y - info.m_bounds.b.y) * 0.33000001f, (info.m_bounds.t.z - info.m_bounds.b.z) * 0.5f);
    CameraSetCoord(camera, 8, coord);
}

// OFFSET: #0x00599670
void CGTabardModelFrame::InitializeTabardColors(CGPlayer_C* player) {
    CRndSeed seed;
    WOWGUID none = WOWGUID();
    GuildStats_C* stats = g_guildInfoCache.GetRecord(player->m_player->m_playerGuildid, none, NULL, NULL, false);

    if (!stats || stats->m_emblemStyle == -1 || stats->m_emblemColor == -1 || stats->m_borderStyle == -1 || stats->m_borderColor == -1 || stats->m_background == -1) {
        seed.SetSeed(0);
        seed.SetSeed(OsGetAsyncTimeMs());

        for (uint32_t i = 0; i < 5; i++) {
            this->m_variations[i] = static_cast<int32_t>((static_cast<uint64_t>(s_tabardVariationCounts[i]) * CRandom::uint32_(seed)) >> 32);
        }
    } else {
        this->m_variations[0] = stats->m_emblemStyle;
        this->m_variations[1] = stats->m_emblemColor;
        this->m_variations[2] = stats->m_borderStyle;
        this->m_variations[3] = stats->m_borderColor;
        this->m_variations[4] = stats->m_background;
    }

    CGPlayer_C* activePlayer = ClntObjMgrGetActivePlayerObj();

    if (activePlayer && activePlayer->m_characterComponent) {
        activePlayer->m_characterComponent->ApplyGuildColor(this->m_variations[0], this->m_variations[1], this->m_variations[2], this->m_variations[3], this->m_variations[4]);
    }
}
