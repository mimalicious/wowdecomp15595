#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jmemsys.h"
#include "jmemnobs.h"

// STATIC: %0x00ED7664
static void* (*s_jpegAlloc)(size_t bytes);

// STATIC: %0x00ED7668
static void (*s_jpegFree)(void* ptr);

// OFFSET: %0x00A8F5A0
void jpeg_set_malloc(void* (*allocFunc)(size_t bytes)) {
    s_jpegAlloc = allocFunc;
}

// OFFSET: %0x00A8F5B0
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

// OFFSET: %0x009AD230
long jpeg_mem_available(j_common_ptr cinfo, long min_bytes_needed, long max_bytes_needed, long already_allocated) {
    return max_bytes_needed;
}

// OFFSET: %0x00A8F600
void jpeg_open_backing_store(j_common_ptr cinfo, backing_store_ptr info, long total_bytes_needed) {
    ERREXIT(cinfo, JERR_NO_BACKING_STORE);
}

// OFFSET: %0x00408910
long jpeg_mem_init(j_common_ptr cinfo) {
    return 0;
}

// OFFSET: %0x00401000
void jpeg_mem_term(j_common_ptr cinfo) {
}
