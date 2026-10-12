#include "FileDownload.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/IOUnit/IOFileUnit.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/HttpDownload.h"

// STATIC: 0x010089C0/0x140EE0350
static SCritSect s_downloadLock;

// INLINED
FileDownload::FileDownload() {
    this->m_partial = false;
    this->m_status = 0;
    this->m_reason = 0;
    this->m_accepted = false;
    this->m_bytesSinceSave = 0;
    this->m_reasonData = 0;
    memset(this->m_fileName, 0, sizeof(this->m_fileName));
}

// PROVEN: 0d9466ed DIFFERENT/IDENTICAL
// OFFSET: 0x00B0A950/0x1408CE540
FileDownload::~FileDownload() {
}

// OFFSET: 0x00B0A2B0/0x1408CDD90
GruntFileDownload::GruntFileDownload() {
    this->m_unit = NULL;
    this->m_header.m_received = 0;
    this->m_header.m_size = 0;
    memset(this->m_header.m_md5, 0, sizeof(this->m_header.m_md5));
    this->m_header.m_version = 0;
    this->m_header.m_magic = 'DInf';
}

// PROVEN: d5afe632 IDENTICAL/DIFFERENT
// OFFSET: 0x00B0A980/0x1408CE580
int32_t GruntFileDownload::Delete() {
    char path[260];

    if (this->m_partial) {
        SStrPrintf(path, sizeof(path), "%s.partial", static_cast<const char*>(this->m_fileName));
    } else {
        SStrCopy(path, this->m_fileName, sizeof(path));
    }

    return OsDeleteFile(path);
}

// PROVEN: 282df74c IDENTICAL/IDENTICAL
// OFFSET: 0x00B0A350/0x1408CDE60
void GruntFileDownload::RenamePartial() {
    if (!this->m_partial) {
        return;
    }

    char path[260];
    SStrPrintf(path, sizeof(path), "%s.partial", static_cast<const char*>(this->m_fileName));

    if (OsFileExists(this->m_fileName)) {
        OsDeleteFile(this->m_fileName);
    }

    if (!OsMoveFile(path, this->m_fileName)) {
        this->m_status = 3;
        this->m_reason = 1;
    }
}

// OFFSET: 0x00B0A3D0/0x1408CDEF0
void STORMAPI GruntFileDownload::CalculateMD5(Storm::IOUnit* unit, uint64_t offset, uint64_t size, uint8_t* digest) {
    uint8_t buffer[0x8000];
    MD5_CTX context;
    MD5Init(&context);

    uint32_t pos = static_cast<uint32_t>(offset);
    uint32_t end = static_cast<uint32_t>(size) - static_cast<uint32_t>(offset);

    while (pos < end) {
        uint32_t count = end - pos;

        if (count >= 0x8000) {
            count = 0x8000;
        }

        if (!unit->Read(buffer, pos, count, NULL)) {
            memset(digest, 0, 16);
            return;
        }

        pos += count;
        MD5Update(&context, buffer, count);
    }

    MD5Final(digest, &context);
}

// OFFSET: 0x00B0A470/0x1408CDFC0
bool GruntFileDownload::VerifyMD5(uint64_t size) {
    uint8_t digest[16];
    GruntFileDownload::CalculateMD5(this->m_unit, 0, size, digest);
    return memcmp(digest, this->m_header.m_md5, sizeof(digest)) == 0;
}

// OFFSET: 0x00B0A4E0/0x1408CE020
bool GruntFileDownload::VerifyFile() {
    int64_t size;
    this->m_unit->GetFileSize(&size);
    return static_cast<uint64_t>(size) == this->m_header.m_size && this->VerifyMD5(size);
}

// OFFSET: 0x00B0A530/0x1408CE0B0
bool GruntFileDownload::ValidateForContinue(bool& complete) {
    complete = false;

    int64_t size;
    this->m_unit->GetFileSize(&size);

    if (static_cast<uint64_t>(size) == this->m_header.m_size + sizeof(Header)) {
        Header header;

        if (this->m_unit->Read(&header, size - sizeof(Header), sizeof(Header), NULL) && header.m_magic == 'DInf' && !header.m_version && !memcmp(header.m_md5, this->m_header.m_md5, sizeof(header.m_md5)) && header.m_size == this->m_header.m_size) {
            if (header.m_size != header.m_received) {
                this->m_header = header;
                return true;
            }

            if (this->VerifyMD5(header.m_size)) {
                complete = true;
                this->m_header = header;
                return true;
            }
        }
    } else if (static_cast<uint64_t>(size) == this->m_header.m_size) {
        complete = this->VerifyFile();

        if (complete) {
            return true;
        }
    }

    return false;
}

// PROVEN: 17418ef6 IDENTICAL/IDENTICAL
// OFFSET: 0x00B0A660/0x1408CE220
int32_t GruntFileDownload::GetStatus() {
    return this->m_status;
}

// OFFSET: 0x00B0A670/0x1408CE230
void GruntFileDownload::WriteHeader() {
    if (!this->m_unit) {
        return;
    }

    Header header = this->m_header;

    if (!this->m_unit->Write(&header, this->m_header.m_size, sizeof(header))) {
        this->m_status = 3;
        this->m_reason = 1;
    }
}

// OFFSET: 0x00B0A6D0/0x1408CE2C0
void GruntFileDownload::Proceed(const uint8_t* data, uint32_t size) {
    s_downloadLock.Enter();

    bool success = true;

    if (this->m_unit && this->m_unit->Write(data, this->m_header.m_received, size)) {
        this->m_header.m_received += size;
        this->m_bytesSinceSave += size;

        if (this->m_header.m_received <= this->m_header.m_size) {
            if (this->m_bytesSinceSave >= 0x100000 || this->m_header.m_received == this->m_header.m_size) {
                this->WriteHeader();
            }
        } else {
            this->m_status = 3;
            this->m_reason = 1;
        }

        if (this->m_header.m_received == this->m_header.m_size) {
            uint8_t digest[16];
            GruntFileDownload::CalculateMD5(this->m_unit, 0, this->m_header.m_size, digest);
            this->m_status = (memcmp(digest, this->m_header.m_md5, sizeof(digest)) != 0) + 2;
        }
    } else {
        success = false;
    }

    s_downloadLock.Leave();

    if (!success) {
        this->m_status = 3;
    }
}

// OFFSET: 0x00B0A820/0x1408CE400
float GruntFileDownload::GetProgress() {
    if (!this->m_header.m_size) {
        return 0.0f;
    }

    return static_cast<float>(static_cast<double>(this->m_header.m_received) / static_cast<double>(this->m_header.m_size));
}

// INLINED
inline void GruntFileDownload::Close(bool deleteFile, bool rename) {
    s_downloadLock.Enter();

    if (this->m_unit) {
        this->m_unit->Release();
        this->m_unit = NULL;
    }

    if (deleteFile) {
        this->Delete();
    }

    if (rename) {
        this->RenamePartial();
    }

    s_downloadLock.Leave();
}

// OFFSET: 0x00B0B020/0x1408CED60
void GruntFileDownload::Destroy(bool deleteFile) {
    this->Close(deleteFile, false);
}

// OFFSET: 0x00B0AA20/0x1408CE620
bool GruntFileDownload::Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset) {
    this->Destroy(false);

    this->m_header.m_size = size;
    memcpy(this->m_header.m_md5, md5, sizeof(this->m_header.m_md5));

    bool result = true;
    offset = 0;

    SStrCopy(this->m_fileName, fileName, sizeof(this->m_fileName));

    char path[260];
    SStrPrintf(path, sizeof(path), "%s.partial", fileName);

    if (OsFileExists(path)) {
        Storm::IOFileParam param = { path, 0xC0000000, 0, 3, 0 };
        this->m_unit = Storm::IOFileUnit::Create(param, NULL);

        if (this->m_unit) {
            bool complete;

            if (this->ValidateForContinue(complete)) {
                this->m_partial = true;

                if (complete) {
                    result = false;
                    this->m_status = 2;
                }

                offset = this->m_header.m_received;
                return result;
            }

            this->m_unit->Release();
            this->m_unit = NULL;
        }
    }

    if (OsFileExists(this->m_fileName)) {
        bool done = false;
        Storm::IOFileParam param = { this->m_fileName, 0xC0000000, 0, 3, 0 };
        this->m_unit = Storm::IOFileUnit::Create(param, NULL);

        if (this->m_unit) {
            if (this->VerifyFile()) {
                this->m_status = 2;
                offset = this->m_header.m_received;
                result = false;
                done = true;
            }

            this->m_unit->Release();
            this->m_unit = NULL;

            if (done) {
                return result;
            }
        }
    }

    Storm::IOFileParam param = { path, 0xC0000000, 0, 2, 0 };
    this->m_unit = Storm::IOFileUnit::Create(param, NULL);
    this->m_partial = true;

    if (!this->m_unit) {
        this->m_status = 3;
        this->m_reason = 1;
        return false;
    }

    if (!this->m_unit->SetFileSize(this->m_header.m_size + sizeof(Header))) {
        this->m_reason = 2;
        this->m_status = 3;
        this->m_reasonData = static_cast<uint32_t>((this->m_header.m_size + sizeof(Header)) >> 20) + 1;
        return false;
    }

    return result;
}

// OFFSET: 0x00B0B080/0x1408CEDF0
void GruntFileDownload::CompleteDownload() {
    if (!this->m_unit) {
        return;
    }

    bool rename = false;

    if (this->m_unit->SetFileSize(this->m_header.m_size)) {
        rename = this->m_partial;
    } else {
        this->m_status = 3;
        this->m_reason = 1;
    }

    this->Close(false, rename);
}

// OFFSET: 0x00B0B120/0x1408CEEB0
void GruntFileDownload::StopDownload() {
    s_downloadLock.Enter();

    this->m_status = 3;
    this->m_reason = 3;

    if (this->m_unit) {
        this->WriteHeader();
        this->Close(false, false);
    }

    s_downloadLock.Leave();
}

// OFFSET: 0x00B0ACC0/0x1408CE910
int32_t BattlenetFileDownload::GetStatus() {
    if (static_cast<uint32_t>(this->m_status) <= 1) {
        bool complete = true;
        bool done = true;

        for (int32_t i = 0; i < this->m_numDownloads; i++) {
            int64_t size = this->m_downloads[i]->Size();

            if (this->m_downloads[i]->Progress() != size) {
                complete = false;
            }

            if (!this->m_downloads[i]->Done()) {
                done = false;
            }
        }

        if (done) {
            this->m_status = complete ? 2 : 3;
        }
    }

    return this->m_status;
}

// OFFSET: 0x00B0ADA0/0x1408CEA20
float BattlenetFileDownload::GetProgress() {
    int64_t total = 0;
    int64_t progress = 0;

    for (int32_t i = 0; i < this->m_numDownloads; i++) {
        progress += this->m_downloads[i]->Progress();
        int64_t size = this->m_downloads[i]->Size();

        if (size < 0) {
            size = 100000000;
        }

        total += size;
    }

    if (total <= 0 || progress <= 0) {
        return 0.0f;
    }

    return static_cast<float>(static_cast<double>(progress) / static_cast<double>(total));
}

// PROVEN: 1b0adff3 IDENTICAL/IDENTICAL
// OFFSET: 0x00B0AE50/0x1408CEAF0
void BattlenetFileDownload::StopDownload() {
    this->m_status = 3;
    this->m_reason = 3;

    for (int32_t i = 0; i < this->m_numDownloads; i++) {
        this->m_downloads[i]->Cancel();
    }
}

// OFFSET: 0x00B0AE90/0x1408CEB60
void BattlenetFileDownload::CompleteDownload() {
    for (int32_t i = 0; i < this->m_numDownloads; i++) {
        const char* name = this->m_downloads[i]->TargetName();

        char path[260];
        CGlueMgr::GetPatchFolder(path, sizeof(path));
        SStrPack(path, "/", sizeof(path));
        SStrPack(path, name, sizeof(path));

        Storm::IOFileParam param = { path, 0xC0000000, 0, 2, 0 };
        Storm::IOUnit* unit = Storm::IOFileUnit::Create(param, NULL);

        if (!unit) {
            this->m_reason = 1;
            this->m_status = 3;
            return;
        }

        bool written = unit->Write(this->m_downloads[i]->GetBytes(), 0, static_cast<uint32_t>(this->m_downloads[i]->Size()));
        unit->Release();

        if (!written) {
            this->m_reason = 1;
            this->m_status = 3;
            return;
        }
    }
}

// OFFSET: [0x00B0B220/0x1408CEF60, 0x00B0B5D0/0x1408CF480]
GruntFileDownload::~GruntFileDownload() {
    this->Close(false, false);
}

// PROVEN: 2bf73f77 UNPROVEN/IDENTICAL
// OFFSET: 0x00B0B280/0x1408CEFE0
BattlenetFileDownload::BattlenetFileDownload() {
}

// PROVEN: 8c00ef91 IDENTICAL/IDENTICAL
// OFFSET: 0x00B0B2E0/0x1408CDE30
int32_t GruntFileDownload::GetReason(uint32_t* data) {
    if (data) {
        *data = this->m_reasonData;
    }

    return this->m_reason;
}

// PROVEN: 32e508ff IDENTICAL/IDENTICAL
// OFFSET: 0x00B0B2E0/0x1408CDE30
int32_t BattlenetFileDownload::GetReason(uint32_t* data) {
    if (data) {
        *data = this->m_reasonData;
    }

    return this->m_reason;
}

// PROVEN: d74a8d12 IDENTICAL/IDENTICAL
// OFFSET: 0x00B0A340/0x1408CDE50
bool GruntFileDownload::Accepted() {
    return this->m_accepted;
}

// PROVEN: 098d26f1 IDENTICAL/IDENTICAL
// OFFSET: 0x00B0A340/0x1408CDE50
bool BattlenetFileDownload::Accepted() {
    return this->m_accepted;
}

// PROVEN: f60321a0 IDENTICAL/IDENTICAL
// OFFSET: 0x00841AE0/0x140211B90
void BattlenetFileDownload::Proceed(const uint8_t* data, uint32_t size) {
}

// OFFSET: 0x00B0B300/0x1408CF060
bool BattlenetFileDownload::Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset) {
    BattlenetLogin* login = static_cast<BattlenetLogin*>(ClientServices::LoginConnection());

    if (!login || login->GetLoginServerType() != 1) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (login->GetLoginServerType() != 1) {
        return false;
    }

    this->m_numDownloads = 0;

    for (const char* instruction = login->GetPatchInstruction(0); instruction; instruction = login->GetPatchInstruction(++this->m_numDownloads)) {
        if (!SStrCmp(instruction, "redirect monolithic", SStrLen("redirect monolithic"))) {
            this->m_status = 4;
            return true;
        }
    }

    this->m_numDownloads = 0;

    for (const char* instruction = login->GetPatchInstruction(0); instruction; instruction = login->GetPatchInstruction(++this->m_numDownloads)) {
        char fields[4][250];
        memset(fields, 0, sizeof(fields));

        for (int32_t i = 0; i < 4; i++) {
            SStrTokenize(&instruction, fields[i], sizeof(fields[i]), ";", NULL);
        }

        if (!fields[0][0]) {
            continue;
        }

        if (!fields[1][0]) {
            const char* slash = SStrChrR(fields[0], '/');

            if (!slash) {
                continue;
            }

            SStrCopy(fields[1], slash + 1, sizeof(fields[1]));
        }

        CGlueMgr::LogConnectionStatus("Downloading patch %s;%s;%s;%s", fields[0], fields[1], fields[2], fields[3]);

        HttpDownload* download = new HttpDownload(fields[0], fields[1]);
        this->m_downloads.Add(1, &download);
    }

    this->m_numDownloads = this->m_downloads.Count();
    offset = 0;

    return true;
}

// OFFSET: 0x00B0B530/0x1408CF390
void BattlenetFileDownload::Destroy(bool deleteFile) {
    this->StopDownload();

    for (int32_t i = 0; i < this->m_numDownloads; i++) {
        if (this->m_downloads[i]) {
            delete this->m_downloads[i];
        }

        this->m_downloads[i] = NULL;
    }

    this->m_numDownloads = 0;
    this->m_downloads.Clear();
}

// PROVEN: aa9ad930 DIFFERENT/IDENTICAL
// OFFSET: 0x00B0B600/0x1408CF4D0
BattlenetFileDownload::~BattlenetFileDownload() {
    this->Destroy(false);
}
