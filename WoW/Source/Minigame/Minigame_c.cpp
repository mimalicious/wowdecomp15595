#include "Minigame_c.h"
#include <cstring>
#include <new>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// INLINED
CMinigame_C::CMinigame_C(MINIGAME_TYPE type, uint64_t guid) {
    this->m_guid = guid;
    this->m_type = type;
}

// INLINED
CMinigameTicTacToe_C::CMinigameTicTacToe_C(uint64_t guid) : CMinigame_C(MINIGAME_TICTACTOE, guid) {
    this->m_guid = guid;
    this->m_type = MINIGAME_TICTACTOE;
    this->m_state = 0;
    this->unk_0010 = 0;
    this->unk_0018 = 0;
    this->unk_0020 = 0;
    this->unk_0021 = 0;
    memset(this->m_board, 0, sizeof(this->m_board));
}

// OFFSET: #0x00847C20
CMinigame_C* CMinigame_C::CreateMinigame(MINIGAME_TYPE type, uint64_t guid) {
    if (type != MINIGAME_TICTACTOE) {
        return NULL;
    }

    void* memory = SMemAlloc(sizeof(CMinigameTicTacToe_C), __FILE__, __LINE__, 0);
    return memory ? new (memory) CMinigameTicTacToe_C(guid) : NULL;
}

// OFFSET: #0x00847CA0
void CMinigame_C::UpdateSetup(CDataStore* msg) {
    uint8_t state;
    msg->Get(state);
    this->m_state = state;
    msg->Get(this->unk_0010);
    msg->Get(this->unk_0018);
    msg->GetData(this->m_board, sizeof(this->m_board));

    if (this->m_state == 2) {
        msg->Get(this->unk_0021);
    }
}

// PROVEN: 5a310b5e IDENTICAL/NONE
// OFFSET: 0x00B38100/0x140903FC0
void CMinigameTicTacToe_C::UpdateState(CDataStore* msg) {
    for (uint32_t i = 0; i < 9; i++) {
        msg->Get(this->m_board[i]);
    }

    msg->Get(this->unk_0020);
}

// OFFSET: #0x00847D40
bool CMinigameTicTacToe_C::ValidateMoveClient(uint8_t moveType, int32_t param) {
    return CMinigameTicTacToe::ValidateMove(this, moveType, param);
}

// OFFSET: #0x00847D70
int32_t CMinigameTicTacToe_C::PushStateToLua(lua_State* L) {
    lua_checkstack(L, 9);

    for (uint32_t i = 0; i < 9; i++) {
        lua_pushnumber(L, this->m_board[i]);
    }

    return 9;
}

// OFFSET: #0x00847DC0
void CMinigame_C::MakeMove(uint8_t moveType, int32_t param) {
    if (!this->ValidateMoveClient(moveType, param)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MINIGAME_MOVE));
    msg.Put(this->m_guid);
    msg.Put(moveType);
    msg.Put(static_cast<uint32_t>(param));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x00847E70
bool CMinigameTicTacToe::ValidateMove(MiniGameData* data, uint8_t moveType, int32_t param) {
    return static_cast<uint32_t>(param) <= 9 && data->m_board[param] == 0xFF;
}
