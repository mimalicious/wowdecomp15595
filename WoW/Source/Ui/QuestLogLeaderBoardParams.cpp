#include "QuestLogLeaderBoardParams.h"
#include "WoW/Source/Ui/QuestLog.h"

// PROVEN: 476dae72 IDENTICAL/IDENTICAL
// OFFSET: 0x00961A20/0x1406D5760
QuestLogLeaderBoardParams::QuestLogLeaderBoardParams() {
    this->m_index = -1;
    this->m_requireLogEntry = 0;
    this->m_countEmpty = 0;
    this->m_showProgress = 1;
    this->m_questCallback = QuestQueryCallback;
    this->m_objectCallback = ObjectQueryCallback;
    this->m_creatureCallback = QuestCreatureQueryCallback;
    this->m_itemCallback = ItemQueryCallback;
    this->m_param = NULL;
    this->m_text[0] = 0;
    this->m_type[0] = 0;
    this->m_complete = 0;
}
