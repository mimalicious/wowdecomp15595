#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_ANIMKITMANAGER_H

#include <cstddef>
#include <cstdint>

class CM2Model;
class CAnimKitManager;

class CAnimKitInst {
    public:
    /* 0000 */ uint8_t unk_0000[0x10];
    /* 0010 */ CAnimKitManager* m_manager;

    uint32_t GetID();
    void Release();
    void SetStopCallback(void (*callback)(CAnimKitInst* inst, uint64_t param), uint64_t param);
    void Stop(int32_t immediate, int32_t a2);
};

class CAnimKitManager {
    public:
    static void GarbageCollect();
    static void InitSystem();
    static void UninitSystem();
    static void UpdateAll(uint32_t elapsedMs);

    /* 0000 */ uint16_t m_flags;
    /* 0002 */ uint16_t unk_0002;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ uint8_t unk_0008 [0xB0];
    /* 00B8 */ void (*m_expressionCallback)(CAnimKitManager* manager, uint64_t param);
    /* 00BC */ uint8_t unk_00BC[4];
    /* 00C0 */ uint64_t m_expressionCallbackParam;
    /* 00C8 */ uint8_t unk_00C8 [0x28];

    CAnimKitManager();
    CAnimKitInst* GetExpressingAnimKitInst(uint32_t boneID, int32_t a2);
    CAnimKitInst* PlayAndMaintain(uint32_t animKitID, int32_t a2, int32_t a3);
    void SetExpressionCallback(void (*callback)(CAnimKitManager* manager, uint64_t param), uint64_t param);
};

#endif
