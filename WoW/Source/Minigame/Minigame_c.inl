#ifndef WOW_SOURCE_MINIGAME_MINIGAME_C_INL
#define WOW_SOURCE_MINIGAME_MINIGAME_C_INL

#include <cstring>

// INLINED
inline CMinigameTicTacToe_C::CMinigameTicTacToe_C(uint64_t guid) : CMinigame_C(MINIGAME_TICTACTOE, guid) {
    this->m_guid = guid;
    this->m_type = MINIGAME_TICTACTOE;
    this->m_state = 0;
    this->unk_0010 = 0;
    this->unk_0018 = 0;
    this->unk_0020 = 0;
    this->unk_0021 = 0;
    memset(this->m_board, 0, sizeof(this->m_board));
}

#endif
