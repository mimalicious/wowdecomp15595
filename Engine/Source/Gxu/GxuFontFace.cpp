#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gxu/GxuFontFace.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

TSHashTable<FACEDATA, HASHKEY_STRI> s_faceHash;

// PROVEN: 9e2e24b1 IDENTICAL/IDENTICAL
// OFFSET: 0x006224E0/0x1402B3400
FT_Face FontFaceGetFace(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return reinterpret_cast<FACEDATA*>(handle)->m_face;
}

// OFFSET: 0x00622500/0x1402B3430
void FontFaceCloseHandle(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    FACEDATA* face = reinterpret_cast<FACEDATA*>(handle);

    HandleClose(reinterpret_cast<HOBJECT>(handle));

    if (face->m_refcount <= 1) {
        HOBJECT selfReference = face->m_selfReference;
        face->m_selfReference = NULL;
        HandleClose(selfReference);
    }
}

// PROVEN: 5197b9a6 IDENTICAL/IDENTICAL
// OFFSET: 0x00622540/0x1402B3470
const char* FontFaceGetFontName(HFACE handle) {
    if (!handle) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    return reinterpret_cast<FACEDATA*>(handle)->m_key.m_str;
}

// OFFSET: 0x00622600
FACEDATA::FACEDATA() {
    this->m_data = NULL;
    this->m_face = NULL;
}

// OFFSET: 0x00622630/0x1402B3620
FACEDATA::~FACEDATA() {
    if (this->m_face) {
        FT_Done_Face(this->m_face);
    }

    if (this->m_data) {
        SFile::Unload(this->m_data);
    }
}

// OFFSET: 0x00622F70/0x1402B4370
HFACE FontFaceGetHandle(const char* fileName, FT_Library library) {
    if (!library || !fileName || !*fileName) {
        return NULL;
    }

    FACEDATA* existing = s_faceHash.Ptr(fileName);

    if (existing) {
        return reinterpret_cast<HFACE>(HandleDuplicate(existing->m_selfReference));
    }

    HFACE handle = NULL;
    void* data = NULL;
    uint32_t size;

    if (SFile::ReadFile(NULL, fileName, &data, &size, 0, 3)) {
        if (!data) {
            return handle;
        }

        FT_Face ftFace;

        if (!FT_New_Memory_Face(library, static_cast<FT_Byte*>(data), size, 0, &ftFace) && ftFace && !FT_Select_Charmap(ftFace, ft_encoding_unicode)) {
            FACEDATA* face = new ("HFACE", SLOG_OBJECT) FACEDATA;
            face->m_data = data;
            face->m_face = ftFace;
            s_faceHash.Insert(face, fileName);
            data = NULL;
            ftFace = NULL;
            face->m_selfReference = HandleCreate(face, "HFACE");
            handle = reinterpret_cast<HFACE>(HandleDuplicate(face->m_selfReference));
        }
    }

    if (data) {
        SFile::Unload(data);
    }

    return handle;
}
