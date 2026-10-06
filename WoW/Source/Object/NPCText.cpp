#include "NPCText.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// PROVEN: 1dd4e454 IDENTICAL
// OFFSET: 0x0049B1F0
NPCText::~NPCText() {
    for (int32_t i = 0; i < 8; i++) {
        if (this->m_text[i][0]) {
            SMemFree(this->m_text[i][0], __FILE__, __LINE__, 0);
        }

        if (this->m_text[i][1]) {
            SMemFree(this->m_text[i][1], __FILE__, __LINE__, 0);
        }
    }
}

// PROVEN: bf6e499b IDENTICAL
// OFFSET: 0x00A28DE0
void NPCText::Pack(CDataStore* msg) {
    for (uint32_t i = 0; i < 8; i++) {
        msg->Put(this->m_probability[i]);

        if (this->m_text[i][0]) {
            msg->PutString(this->m_text[i][0]);
        } else {
            msg->PutString("");
        }

        if (this->m_text[i][1]) {
            msg->PutString(this->m_text[i][1]);
        } else {
            msg->PutString("");
        }

        msg->Put(this->m_language[i]);

        for (int32_t j = 0; j < 3; j++) {
            msg->Put(this->m_emoteDelay[i][j]);
            msg->Put(this->m_emote[i][j]);
        }
    }
}

// OFFSET: #0x0098E2C0
void NPCText::Unpack(CDataStore* msg) {
    char buffer[3000];

    for (int32_t i = 0; i < 8; i++) {
        msg->Get(this->m_probability[i]);

        msg->GetString(buffer, sizeof(buffer));

        if (this->m_text[i][0]) {
            SMemFree(this->m_text[i][0], __FILE__, __LINE__, 0);
        }

        this->m_text[i][0] = buffer[0] ? SStrDupA(buffer, __FILE__, __LINE__) : NULL;

        msg->GetString(buffer, sizeof(buffer));

        if (this->m_text[i][1]) {
            SMemFree(this->m_text[i][1], __FILE__, __LINE__, 0);
        }

        this->m_text[i][1] = buffer[0] ? SStrDupA(buffer, __FILE__, __LINE__) : NULL;

        msg->Get(this->m_language[i]);

        for (int32_t j = 0; j < 3; j++) {
            msg->Get(this->m_emoteDelay[i][j]);
            msg->Get(this->m_emote[i][j]);
        }
    }
}
