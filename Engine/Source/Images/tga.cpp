#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Images/tga.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"

// INLINED
CImageFileImpl::~CImageFileImpl() {
}

// INLINED
CTgaFile::CTgaFile() {
    this->m_file = NULL;
    this->m_image = NULL;
    this->m_addlHeaderData = NULL;
    this->m_imageBytes = 0;
    this->m_colorMap = NULL;
}

// OFFSET: %0x00489E30
CTgaFile::~CTgaFile() {
    this->Close();
}

// OFFSET: %0x00489E50
uint32_t CTgaFile::Width() const {
    return this->m_header.m_wWidth;
}

// OFFSET: %0x00489E60
uint32_t CTgaFile::Height() const {
    return this->m_header.m_wHeight;
}

// OFFSET: %0x00489E70
uint32_t CTgaFile::Size() const {
    return this->Width() * this->Height();
}

// OFFSET: %0x00489E90
int32_t CTgaFile::BytesPerPixel() const {
    return (this->m_header.m_bPixelDepth + 7) / 8;
}

// OFFSET: %0x00489EB0
uint32_t CTgaFile::Bytes() const {
    return this->m_imageBytes;
}

// OFFSET: %0x00489EC0
uint8_t CTgaFile::AlphaBits() const {
    return this->m_header.m_desc.m_bAlphaChannelBits;
}

// OFFSET: %0x00489ED0
int32_t CTgaFile::IsTopDown() const {
    return this->m_header.m_desc.m_bTopBottomOrder;
}

// OFFSET: %0x00644550
uint32_t CTgaFile::ColorMapEntryBytes() {
    int32_t bits = this->m_header.m_bColorMapEntrySize / 3;

    if (bits >= 8) {
        bits = 8;
    }

    return 3 * bits / 8;
}

// OFFSET: %0x00644580
uint32_t CTgaFile::ColorMapBytes() {
    return this->m_header.m_wColorMapEntries * this->ColorMapEntryBytes();
}

// OFFSET: %0x006445B0
uint32_t CTgaFile::ComputeAlphaDepth() const {
    if (!this->AlphaBits()) {
        return 0;
    }

    uint32_t depth = 0;
    const TGA32Pixel* pixels = this->ImageTGA32Pixel();
    uint32_t count = this->Size();

    for (uint32_t i = 0; i < count; i++) {
        uint8_t alpha = pixels[i].m_a;

        if (alpha >= 0xFC) {
            continue;
        }

        if (!alpha) {
            if (!depth) {
                depth = 1;
            }

            continue;
        }

        if (alpha & 0xF) {
            return 8;
        }

        depth = 4;
    }

    return depth;
}

// OFFSET: %0x00644620
int32_t CTgaFile::HasNonBlackZeroAlphaPixels() const {
    const TGA32Pixel* pixels = this->ImageTGA32Pixel();

    for (uint32_t i = 0; i < this->Size(); i++) {
        if (pixels[i].m_a > 0) {
            continue;
        }

        uint8_t value = (pixels[i].m_r > pixels[i].m_g ? pixels[i].m_r : pixels[i].m_g) > pixels[i].m_b ? (pixels[i].m_r > pixels[i].m_g ? pixels[i].m_r : pixels[i].m_g) : pixels[i].m_b;

        if (value * (1.0f / 255.0f) > 0.04f) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: %0x00644800
int32_t CTgaFile::ValidateColorDepth() {
    if (this->m_header.m_bPixelDepth - this->m_header.m_desc.m_bAlphaChannelBits == 24) {
        return 1;
    }

    SErrSetLastError(this->m_header.m_bPixelDepth == 16 ? 0xF720007C : 0xF720007D);

    return 0;
}

// OFFSET: %0x00644830
uint32_t CTgaFile::PreImageBytes() {
    return this->m_header.m_bIDLength + sizeof(TGAHeader) + this->ColorMapBytes();
}

// OFFSET: %0x00644870
void CTgaFile::AddAlphaChannel(uint8_t* dest, uint8_t* source, const uint8_t* alpha) {
    if (alpha) {
        for (uint32_t pixels = this->Size(); pixels; pixels--) {
            memmove(dest, source, this->BytesPerPixel());
            source += this->BytesPerPixel();
            dest += this->BytesPerPixel();
            *dest++ = *alpha++;
        }
    } else {
        for (uint32_t pixels = this->Size(); pixels; pixels--) {
            memmove(dest, source, this->BytesPerPixel());
            source += this->BytesPerPixel();
            dest += this->BytesPerPixel();
            *dest++ = 0xFF;
        }
    }

    uint8_t alphaBits = this->AlphaBits();
    this->m_header.m_desc.m_bAlphaChannelBits = 8;
    this->m_header.m_bPixelDepth += 8 - alphaBits;
    this->m_imageBytes = this->BytesPerPixel() * this->Size();
}

// OFFSET: %0x00644960
int32_t CTgaFile::ReadRawImage(uint32_t flags) {
    int32_t addAlpha = (flags & 1) && !this->AlphaBits() ? 1 : 0;

    if (SFile::SetFilePointer(this->m_file, this->PreImageBytes(), NULL, 0) == -1) {
        return 0;
    }

    uint32_t bytes = this->BytesPerPixel() * this->Size();
    this->m_image = static_cast<uint8_t*>(SMemAlloc(bytes + addAlpha * this->Size(), __FILE__, __LINE__, 0));

    if (!this->m_image || !SFileReadFile(this->m_file, &this->m_image[addAlpha * this->Size()], bytes, NULL, NULL, 0)) {
        return 0;
    }

    if (addAlpha) {
        this->AddAlphaChannel(this->m_image, &this->m_image[this->Size()], NULL);
    }

    return 1;
}

// OFFSET: %0x00644A70
int32_t CTgaFile::RLEDecompressImage(uint8_t* rleData, uint8_t* data) {
    int32_t pixels = this->Size();
    int32_t bytesPerPixel = this->BytesPerPixel();

    while (pixels) {
        uint8_t packet = *rleData++;

        if (packet & 0x80) {
            int32_t count = (packet & 0x7F) + 1;
            pixels -= count;

            while (count--) {
                memcpy(data, rleData, bytesPerPixel);
                data += bytesPerPixel;
            }

            rleData += bytesPerPixel;
        } else {
            int32_t count = packet + 1;
            pixels -= count;
            memcpy(data, rleData, bytesPerPixel * count);
            rleData += bytesPerPixel * count;
            data += bytesPerPixel * count;
        }

        if (pixels < 0) {
            SErrSetLastError(0xF7200077);
            return 0;
        }
    }

    this->m_header.m_bImageType -= 8;

    return 1;
}

// OFFSET: %0x00644B20
int32_t CTgaFile::SetTopDown(int32_t topDown) {
    int32_t isTopDown = this->IsTopDown();

    if (topDown) {
        if (isTopDown) {
            return 1;
        }
    } else if (!isTopDown) {
        return 1;
    }

    if (this->m_header.m_bImageType >= 9 && this->m_header.m_bImageType <= 11) {
        SErrSetLastError(0xF7200083);
        return 0;
    }

    uint8_t* image = static_cast<uint8_t*>(SMemAlloc(this->BytesPerPixel() * this->Size(), __FILE__, __LINE__, 0));
    uint8_t* source = this->m_image;
    uint8_t* dest = &image[(this->Height() - 1) * this->BytesPerPixel() * this->Width()];

    for (uint32_t row = this->Height(); row; row--) {
        memcpy(dest, source, this->BytesPerPixel() * this->Width());
        source += this->BytesPerPixel() * this->Width();
        dest -= this->BytesPerPixel() * this->Width();
    }

    SMemFree(this->m_image, __FILE__, __LINE__, 0);
    this->m_header.m_desc.m_bTopBottomOrder = topDown != 0;
    this->m_image = image;

    return 1;
}

// OFFSET: %0x00644C70
const uint8_t* CTgaFile::Image() const {
    if (!this->m_image) {
        SErrSetLastError(0xF720007F);
        return NULL;
    }

    return this->m_image;
}

// OFFSET: %0x00644C70
uint8_t* CTgaFile::Image() {
    if (!this->m_image) {
        SErrSetLastError(0xF720007F);
        return NULL;
    }

    return this->m_image;
}

// OFFSET: %0x00644C90
const TGA32Pixel* CTgaFile::ImageTGA32Pixel() const {
    if (!this->m_image) {
        SErrSetLastError(0xF720007F);
        return NULL;
    }

    if (this->m_header.m_bPixelDepth != 32) {
        SErrSetLastError(0xF720007D);
        return NULL;
    }

    return reinterpret_cast<const TGA32Pixel*>(this->m_image);
}

// OFFSET: %0x00644C90
TGA32Pixel* CTgaFile::ImageTGA32Pixel() {
    if (!this->m_image) {
        SErrSetLastError(0xF720007F);
        return NULL;
    }

    if (this->m_header.m_bPixelDepth != 32) {
        SErrSetLastError(0xF720007D);
        return NULL;
    }

    return reinterpret_cast<TGA32Pixel*>(this->m_image);
}

// OFFSET: %0x00644CC0
int32_t CTgaFile::RemoveAlphaChannels() {
    if (!this->Image()) {
        return 0;
    }

    if (!this->m_header.m_desc.m_bAlphaChannelBits) {
        if (this->m_header.m_bPixelDepth == 24) {
            return 1;
        }

        SErrSetLastError(0x8720012D);
        this->m_header.m_desc.m_bAlphaChannelBits = this->m_header.m_bPixelDepth - 8;
    }

    if (this->m_header.m_desc.m_bAlphaChannelBits != 8) {
        SErrSetLastError(0xF7200082);
        return 0;
    }

    uint32_t pixels = this->Size();
    uint8_t* source = this->m_image;
    this->m_header.m_bPixelDepth -= this->m_header.m_desc.m_bAlphaChannelBits;
    this->m_header.m_desc.m_bAlphaChannelBits = 0;
    uint8_t* dest = source;

    while (pixels--) {
        memmove(dest, source, this->BytesPerPixel());
        dest += this->BytesPerPixel();
        source += this->BytesPerPixel() + 1;
    }

    return 1;
}

// OFFSET: %0x006455E0
int32_t CTgaFile::SetImage(const void* bits, uint16_t width, uint16_t height, uint8_t pixelDepth, uint8_t alphaBits, int32_t topBottomOrder, int32_t leftRightOrder) {
    if (!bits || (pixelDepth != 32 && pixelDepth != 24) || (alphaBits && alphaBits != 8)) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    memset(&this->m_header, 0, sizeof(this->m_header));
    this->m_header.m_bPixelDepth = pixelDepth;
    this->m_header.m_wWidth = width;
    this->m_header.m_wHeight = height;
    this->m_header.m_desc.m_bAlphaChannelBits = alphaBits;
    this->m_header.m_desc.m_bLeftRightOrder = leftRightOrder != 0;
    this->m_header.m_desc.m_bTopBottomOrder = topBottomOrder != 0;
    this->m_header.m_bImageType = 2;
    this->m_imageBytes = this->BytesPerPixel() * this->Size();

    if (this->m_image) {
        SMemFree(this->m_image, __FILE__, __LINE__, 0);
    }

    this->m_image = static_cast<uint8_t*>(SMemAlloc(this->m_imageBytes, __FILE__, __LINE__, 0));

    if (!this->m_image) {
        return 0;
    }

    memcpy(this->m_image, bits, this->m_imageBytes);
    this->m_footer.m_dwExtensionOffset = 0;
    this->m_footer.m_dwDeveloperOffset = 0;
    SStrCopy(this->m_footer.m_szSigniture, "TRUEVISION-XFILE.", sizeof(this->m_footer.m_szSigniture));

    return 1;
}

// OFFSET: %0x00644D90
int32_t CTgaFile::CountRun(const uint8_t* source, int32_t count) {
    int32_t run = 0;
    uint32_t pixel = 0;
    memcpy(&pixel, source, this->BytesPerPixel());

    if (count > 128) {
        count = 128;
    }

    while (count--) {
        if (memcmp(source, &pixel, this->BytesPerPixel())) {
            break;
        }

        source += this->BytesPerPixel();
        run++;
    }

    return run;
}

// OFFSET: %0x00644E40
int32_t CTgaFile::RleCompressLine(uint8_t** source, uint8_t** dest) {
    uint8_t* in = *source;
    uint8_t* out = *dest;
    uint8_t* rawPacket = NULL;
    int32_t remaining = this->Width();

    while (remaining) {
        int32_t run = this->CountRun(in, remaining);

        if (run >= 2) {
            this->m_imageBytes += this->BytesPerPixel() + 1;
            rawPacket = NULL;

            if (this->m_imageBytes >= this->BytesPerPixel() * this->Size()) {
                return 0;
            }

            *out++ = (run - 1) | 0x80;
            memcpy(out, in, this->BytesPerPixel());
            out += this->BytesPerPixel();
            in += run * this->BytesPerPixel();
            remaining -= run;
        } else {
            if (!rawPacket || *rawPacket == 127) {
                rawPacket = out;
                *out++ = 0;
                this->m_imageBytes++;
            } else {
                (*rawPacket)++;
            }

            this->m_imageBytes += this->BytesPerPixel();

            if (this->m_imageBytes >= this->BytesPerPixel() * this->Size()) {
                return 0;
            }

            memcpy(out, in, this->BytesPerPixel());
            out += this->BytesPerPixel();
            remaining--;
            in += this->BytesPerPixel();
        }
    }

    *source = in;
    *dest = out;

    return 1;
}

// OFFSET: %0x00644FB0
int32_t CTgaFile::Compress() {
    if (!this->m_image) {
        SErrSetLastError(0xF7200081);
        return 0;
    }

    if (this->m_header.m_bImageType >= 9) {
        SErrSetLastError(0xF7200083);
        return 0;
    }

    uint8_t* source = this->m_image;
    uint8_t* compressed = static_cast<uint8_t*>(SMemAlloc(this->m_imageBytes, __FILE__, __LINE__, 0));
    uint8_t* dest = compressed;

    if (!compressed) {
        return 0;
    }

    this->m_imageBytes = 0;

    for (uint32_t row = this->Height(); row--; ) {
        if (!this->RleCompressLine(&source, &dest)) {
            SMemFree(compressed, __FILE__, __LINE__, 0);
            this->m_imageBytes = this->BytesPerPixel() * this->Size();
            return 1;
        }
    }

    this->m_header.m_bImageType += 8;
    SMemFree(this->m_image, __FILE__, __LINE__, 0);
    this->m_image = compressed;

    return 1;
}

// OFFSET: %0x006450A0
int32_t CTgaFile::Write(const char* fileName) {
    if (!fileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!this->m_image) {
        SErrSetLastError(0xF7200081);
        return 0;
    }

    HOSFILE file = OsCreateFile(fileName, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return 0;
    }

    int32_t result = 0;
    uint32_t written;

    if (OsWriteFile(file, &this->m_header, sizeof(this->m_header), &written) && written == sizeof(this->m_header)
        && (!this->m_header.m_bIDLength || (OsWriteFile(file, this->m_addlHeaderData, this->m_header.m_bIDLength, &written) && written == this->m_header.m_bIDLength))
        && (!this->m_header.m_bColorMapType || (OsWriteFile(file, this->m_colorMap, this->ColorMapBytes(), &written) && written == this->ColorMapBytes()))
        && OsWriteFile(file, this->m_image, this->m_imageBytes, &written) && written == this->m_imageBytes
        && OsWriteFile(file, &this->m_footer, sizeof(this->m_footer), &written) && written == sizeof(this->m_footer)) {
        result = 1;
    }

    OsCloseFile(file);

    if (!result) {
        OsDeleteFile(fileName);
    }

    return result;
}

// OFFSET: %0x00645200
void CTgaFile::Close() {
    if (this->m_image) {
        SMemFree(this->m_image, __FILE__, __LINE__, 0);
    }

    this->m_image = NULL;

    if (this->m_file) {
        SFileCloseFile(this->m_file);
    }

    this->m_file = NULL;

    if (this->m_addlHeaderData) {
        SMemFree(this->m_addlHeaderData, __FILE__, __LINE__, 0);
    }

    this->m_addlHeaderData = NULL;
    this->m_header.m_bIDLength = 0;

    if (this->m_colorMap) {
        SMemFree(this->m_colorMap, __FILE__, __LINE__, 0);
    }

    this->m_colorMap = NULL;
}

// OFFSET: %0x006446C0
int32_t CTgaFile::Open(const char* fileName, int32_t flags) {
    if (!fileName || !*fileName) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    this->Close();

    if (!SFile::OpenEx(NULL, fileName, flags != 0, &this->m_file)) {
        return 0;
    }

    if (!SFileReadFile(this->m_file, &this->m_header, sizeof(this->m_header), NULL, NULL, 0)) {
        return 0;
    }

    if (this->m_header.m_bIDLength) {
        this->m_addlHeaderData = static_cast<uint8_t*>(SMemAlloc(this->m_header.m_bIDLength, __FILE__, __LINE__, 0));

        if (!SFileReadFile(this->m_file, this->m_addlHeaderData, this->m_header.m_bIDLength, NULL, NULL, 0)) {
            return 0;
        }
    } else {
        this->m_addlHeaderData = NULL;
    }

    if (this->m_header.m_bColorMapType) {
        this->m_colorMap = static_cast<uint8_t*>(SMemAlloc(this->ColorMapBytes(), __FILE__, __LINE__, 0));

        if (!SFileReadFile(this->m_file, this->m_colorMap, this->ColorMapBytes(), NULL, NULL, 0)) {
            return 0;
        }
    } else {
        this->m_colorMap = NULL;
    }

    if (this->m_header.m_bPixelDepth == 24 && this->m_header.m_desc.m_bAlphaChannelBits == 8) {
        this->m_header.m_desc.m_bAlphaChannelBits = 0;
        SErrSetLastError(0x8720012E);
    }

    if (this->m_header.m_bPixelDepth == 32 && !this->m_header.m_desc.m_bAlphaChannelBits) {
        this->m_header.m_desc.m_bAlphaChannelBits = 8;
        SErrSetLastError(0x8720012F);
    }

    return 1;
}

// OFFSET: %0x00645270
void CTgaFile::ConvertColorMapped(uint32_t flags) {
    uint32_t addAlpha = flags & 1;
    uint32_t pixelDepth = this->AlphaBits() + 24;
    uint8_t* indices = this->m_image;
    this->m_image = static_cast<uint8_t*>(SMemAlloc((pixelDepth >> 3) * this->Size() + addAlpha * this->Size(), __FILE__, __LINE__, 0));

    uint8_t* dest = &this->m_image[addAlpha * this->Size()];
    uint8_t* index = indices;

    for (uint32_t pixels = this->Size(); pixels; pixels--) {
        memcpy(dest, &this->m_colorMap[(*index++ - this->m_header.m_wColorMapStartIndex) * this->ColorMapEntryBytes()], this->ColorMapEntryBytes());
        dest += this->ColorMapEntryBytes();
    }

    SMemFree(indices, __FILE__, __LINE__, 0);
    SMemFree(this->m_colorMap, __FILE__, __LINE__, 0);
    this->m_header.m_wColorMapEntries = 0;
    this->m_header.m_bPixelDepth = pixelDepth;
    this->m_colorMap = NULL;
    this->m_header.m_bColorMapType = 0;
    this->m_header.m_bImageType = 2;
    this->m_imageBytes = this->BytesPerPixel() * this->Size();

    if (addAlpha) {
        this->AddAlphaChannel(this->m_image, &this->m_image[this->Size()], NULL);
    }
}

// OFFSET: %0x006453C0
int32_t CTgaFile::ReadRleImage(uint32_t flags) {
    int32_t addAlpha = (flags & 1) && !this->AlphaBits() ? 1 : 0;
    this->m_image = static_cast<uint8_t*>(SMemAlloc(this->BytesPerPixel() * this->Size() + addAlpha * this->Size(), __FILE__, __LINE__, 0));

    if (!this->m_image) {
        return 0;
    }

    uint32_t bytes = SFileGetFileSize(this->m_file, NULL) - this->PreImageBytes();

    if (bytes == 0xFFFFFFFF) {
        return 0;
    }

    uint8_t* rleData = static_cast<uint8_t*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));

    if (!rleData) {
        return 0;
    }

    if (SFile::SetFilePointer(this->m_file, this->PreImageBytes(), NULL, 0) == -1) {
        return 0;
    }

    if (!SFileReadFile(this->m_file, rleData, bytes, NULL, NULL, 0)) {
        return 0;
    }

    int32_t result = this->RLEDecompressImage(rleData, &this->m_image[addAlpha * this->Size()]);
    SMemFree(rleData, __FILE__, __LINE__, 0);

    if (!result) {
        return 0;
    }

    if (addAlpha) {
        this->AddAlphaChannel(this->m_image, &this->m_image[this->Size()], NULL);
    }

    return 1;
}

// OFFSET: %0x00645530
int32_t CTgaFile::AddAlphaChannel(const void* alpha) {
    if (!this->Image()) {
        return 0;
    }

    if (this->m_header.m_bImageType >= 9 && this->m_header.m_bImageType <= 11) {
        SErrSetLastError(0xF7200083);
        return 0;
    }

    if (this->m_header.m_desc.m_bAlphaChannelBits) {
        this->RemoveAlphaChannels();
    }

    uint8_t* image = static_cast<uint8_t*>(SMemAlloc((this->BytesPerPixel() + 1) * this->Size(), __FILE__, __LINE__, 0));

    if (!image) {
        return 0;
    }

    this->AddAlphaChannel(image, this->m_image, static_cast<const uint8_t*>(alpha));
    SMemFree(this->m_image, __FILE__, __LINE__, 0);
    this->m_image = image;

    return 1;
}

// OFFSET: %0x00645720
int32_t CTgaFile::ReadColorMappedImage(uint32_t flags) {
    if (!this->m_header.m_bColorMapType) {
        SErrSetLastError(0xF7200084);
        return 0;
    }

    int32_t result = this->m_header.m_bImageType < 9 ? this->ReadRawImage(0) : this->ReadRleImage(0);

    if (this->m_header.m_bColorMapType && (flags & 2)) {
        this->ConvertColorMapped(flags);
    }

    return result;
}

// OFFSET: %0x00645780
int32_t CTgaFile::LoadImageData(uint32_t flags) {
    if (this->m_image) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 1;
    }

    if (!this->m_file) {
        SErrSetLastError(0xF720007E);
        return 0;
    }

    this->m_imageBytes = this->BytesPerPixel() * this->Size();

    switch (this->m_header.m_bImageType) {
        case 0:
            SErrSetLastError(0xF7200078);
            return 0;

        case 1:
        case 9:
            return this->ReadColorMappedImage(flags);

        case 2:
            if (!this->ValidateColorDepth()) {
                return 0;
            }

            return this->ReadRawImage(flags);

        case 3:
        case 11:
            SErrSetLastError(0xF720007A);
            return 0;

        case 10:
            if (!this->ValidateColorDepth()) {
                return 0;
            }

            return this->ReadRleImage(flags);

        default:
            SErrSetLastError(0xF720007B);
            return 0;
    }
}
