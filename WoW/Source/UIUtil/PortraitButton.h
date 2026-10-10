#ifndef WOW_SOURCE_UIUTIL_PORTRAITBUTTON_H
#define WOW_SOURCE_UIUTIL_PORTRAITBUTTON_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGUnit_C;
class CM2Model;
class CM2Scene;
class CSimpleTexture;
class CTexture;

struct PortraitData {
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ TSGrowableArray<CImVector> m_texels;
};

struct PortraitBase {
    /* 0000 */ uint8_t m_dirty;
    /* 0001 */ uint8_t m_pad[3];
    /* 0004 */ PortraitData m_data;
    /* 0018 */ PortraitBase* m_prev;
    /* 001C */ PortraitBase* m_next;
};

struct PLAYERPORTRAIT : public TSHashObject<PLAYERPORTRAIT, CHashKeyGUID> {
    /* 0020 */ PortraitBase m_cache;
};

struct ITEMPORTRAIT : public TSHashObject<ITEMPORTRAIT, HASHKEY_STR> {
    /* 0018 */ PortraitData m_data;
};

struct PORTRAITREQUEST : public TSLinkedNode<PORTRAITREQUEST> {
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint32_t m_flags;
};

struct UNITPORTRAIT : public TSHashObject<UNITPORTRAIT, HASHKEY_NONE> {
    /* 0018 */ PortraitBase m_cache;
};

extern const char* s_sexNames[3];

int32_t RTAlphaSupported();
void __fastcall UpdateAllPortraits();

void PortraitInitialize();
void PortraitShutdown();
void UpdatePendingPortraits();
void UpdateModelForUI(const WOWGUID& guid, uint32_t flags);
bool RenderPortraitTexture(CSimpleTexture* texture, CM2Model* model, CM2Scene* scene, PortraitData* data);
void SetPortraitTexture(CSimpleTexture* texture, const char* fileName);
void SetPortraitTexture(CSimpleTexture* texture, uint32_t race, uint32_t sex, WOWGUID guid);
void SetPortraitTexture(CSimpleTexture* texture, int32_t displayId, const WOWGUID& petGuid);
void SetPortraitTexture(CSimpleTexture* texture, CGUnit_C* unit);
void SetPortraitTextureFromDisplayID(CSimpleTexture* texture, int32_t displayId);

int32_t ReadyModelForUI(CGUnit_C* unit, uint32_t flags);

#endif
