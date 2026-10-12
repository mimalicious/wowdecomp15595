#include "ArchaeologyDigSiteFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/ResearchFrame.h"

// STATIC: 0x00D17680/0x140BD2528
const char* CGArchaeologyDigSiteFrame::s_objectTypeName = "ArchaeologyDigSiteFrame";

// STATIC: 0x00D17684/0x140BD2530
int32_t CGArchaeologyDigSiteFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00F76C58/0x140E6CD90
int32_t CGArchaeologyDigSiteFrame::s_objectType;

// STATIC: 0x00F76C5C
CDataAllocator CGArchaeologyDigSiteFrame::s_digSiteHeap(sizeof(CGArchaeologyDigSiteFrame), 5);

// INLINED
void CGArchaeologyDigSiteFrame::operator delete(void* ptr) {
    CGArchaeologyDigSiteFrame::s_digSiteHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008C6490/0x1406364F0
CSimpleFrame* CGArchaeologyDigSiteFrame::Create(CSimpleFrame* parent) {
    void* data = CGArchaeologyDigSiteFrame::s_digSiteHeap.GetData(0, typeid(CGArchaeologyDigSiteFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGArchaeologyDigSiteFrame(parent) : NULL;
}

// OFFSET: 0x00A02AA0/0x1407A1B60
CGArchaeologyDigSiteFrame::CGArchaeologyDigSiteFrame(CSimpleFrame* parent) : CGBlobFrame(parent) {
}

// PROVEN: 9514d2f3 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02AD0/0x1407A1B90
const char* CGArchaeologyDigSiteFrame::GetObjectTypeName() {
    return CGArchaeologyDigSiteFrame::s_objectTypeName;
}

// PROVEN: 0eb0dca8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02AE0/0x1407A1BA0
int32_t CGArchaeologyDigSiteFrame::GetScriptMetaTable() {
    return CGArchaeologyDigSiteFrame::s_metatable;
}

// PROVEN: 34ab39dc IDENTICAL/UNPROVEN
// OFFSET: 0x00A02AF0/0x1407A1BB0
void CGArchaeologyDigSiteFrame::DrawBlob(int32_t id, bool show) {
    if (show) {
        CGResearchFrame::BLOBPOIDATA* info = CGResearchFrame::GetPOIData(id);

        if (info) {
            this->AddBlob(reinterpret_cast<QuestPOIInfo*>(info), false);
        }
    } else {
        this->RemoveBlob(id);
    }
}

// PROVEN: f6dde4a7 IDENTICAL/IDENTICAL
// OFFSET: 0x00A02B30/0x1407A1BF0
void CGArchaeologyDigSiteFrame::RegisterScriptMethods(lua_State* L) {
    CGBlobFrame::RegisterScriptMethods(L);
}

// PROVEN: ea4f296f IDENTICAL/IDENTICAL
// OFFSET: 0x00A02B40/0x1407A1C00
void CGArchaeologyDigSiteFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGArchaeologyDigSiteFrame::s_metatable);
    CGArchaeologyDigSiteFrame::s_metatable = LUA_NOREF;
}

// PROVEN: 3fa314cc NONE/IDENTICAL
// OFFSET: 0x00A02B80/0x1407A1C40
bool CGArchaeologyDigSiteFrame::IsA(int32_t type) {
    return type == CGArchaeologyDigSiteFrame::s_objectType || type == CGBlobFrame::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00A02BC0/0x1407A1C70
bool CGArchaeologyDigSiteFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGArchaeologyDigSiteFrame::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CGBlobFrame::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x00A02C10
CGArchaeologyDigSiteFrame::~CGArchaeologyDigSiteFrame() {
}

// PROVEN: 6315dccb IDENTICAL/IDENTICAL
// OFFSET: 0x00A02C50/0x1407A1D20
void CGArchaeologyDigSiteFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGArchaeologyDigSiteFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGArchaeologyDigSiteFrame::RegisterScriptMethods);
}
