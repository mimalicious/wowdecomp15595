#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jmemsys.h"
#include "jmemnobs.h"

// STATIC: %0x00ED7664
static void* (*s_jpegAlloc)(size_t bytes);

// STATIC: %0x00ED7668
static void (*s_jpegFree)(void* ptr);

// PROVEN: 2f4b4a9f IDENTICAL/NONE
// OFFSET: 0x00B5D340
void jpeg_set_malloc(void* (*allocFunc)(size_t bytes)) {
    s_jpegAlloc = allocFunc;
}

// PROVEN: 1ae689b1 IDENTICAL/NONE
// OFFSET: 0x00B5D350
void jpeg_set_free(void (*freeFunc)(void* ptr)) {
    s_jpegFree = freeFunc;
}

// OFFSET: %0x00A8F5C0
void* jpeg_get_small(j_common_ptr cinfo, size_t sizeofobject) {
    if (s_jpegAlloc) {
        return s_jpegAlloc(sizeofobject);
    }

    return NULL;
}

// OFFSET: %0x00A8F5E0
void jpeg_free_small(j_common_ptr cinfo, void* object, size_t sizeofobject) {
    if (s_jpegFree) {
        s_jpegFree(object);
    }
}

// OFFSET: %0x00A8F5C0
void* jpeg_get_large(j_common_ptr cinfo, size_t sizeofobject) {
    if (s_jpegAlloc) {
        return s_jpegAlloc(sizeofobject);
    }

    return NULL;
}

// OFFSET: %0x00A8F5E0
void jpeg_free_large(j_common_ptr cinfo, void* object, size_t sizeofobject) {
    if (s_jpegFree) {
        s_jpegFree(object);
    }
}

// PROVEN: 0b3e45f9 IDENTICAL/NONE
// OFFSET: 0x00B5D3A0/0x140925DA0
long jpeg_mem_available(j_common_ptr cinfo, long min_bytes_needed, long max_bytes_needed, long already_allocated) {
    return max_bytes_needed;
}

// PROVEN: ee28f001 IDENTICAL/NONE
// OFFSET: 0x00B5D3B0/0x140925DB0
void jpeg_open_backing_store(j_common_ptr cinfo, backing_store_ptr info, long total_bytes_needed) {
    ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}

// PROVEN: 78e85b40 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
long jpeg_mem_init(j_common_ptr cinfo) {
    return 0;
}

// PROVEN: 61e9e9d9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void jpeg_mem_term(j_common_ptr cinfo) {
}
