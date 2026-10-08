#include "Engine/Source/Services/SysMessage.h"
#include "WoW/Source/Glue/CWOWClientStatus.h"

// INLINED
CWOWClientStatus::CWOWClientStatus(const char* logFileName) {
    this->m_log = NULL;

    if (!SLogCreate(logFileName, 0, &this->m_log)) {
        SysMsgPrintf(STATUS_WARNING, "Cannot create WOWClient log file \"%s\"!", logFileName);
    }
}

// OFFSET: 0x008043D0
CWOWClientStatus::CWOWClientStatus(const char* logFileName, int32_t append) {
    this->m_log = NULL;

    if (logFileName && *logFileName && !SLogCreate(logFileName, append ? 4 : 0, &this->m_log)) {
        SysMsgPrintf(STATUS_WARNING, "Cannot create WOWClient log file \"%s\"!", logFileName);
    }
}

// OFFSET: [0x00803700/0x140518420, 0x00803760/0x140518490]
CWOWClientStatus::~CWOWClientStatus() {
    if (!this->m_log) {
        return;
    }

    for (STATUSENTRY* entry = this->m_statusList.Head(); entry; entry = this->m_statusList.Next(entry)) {
        SLogWrite(this->m_log, entry->m_text);
    }

    SLogClose(this->m_log);
}
