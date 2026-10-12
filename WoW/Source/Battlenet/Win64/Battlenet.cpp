#if defined(WOWDECOMP_ARCH_64)

#include <windows.h>
#include <new>
#include "WoW/Source/Battlenet/Battlenet.h"

// OFFSET: 0x00663320/0x140309410
void* Battlenet::WindowsLibrary::GetProc(const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(this->m_module), name));
}

// OFFSET: 0x00663340/0x140309420
void* Battlenet::WindowsLibrary::GetProc(uint32_t ordinal) {
    if (ordinal > 0xFFFF) {
        return NULL;
    }

    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(this->m_module), MAKEINTRESOURCEA(ordinal)));
}

// OFFSET: 0x00663370/0x140309440
Battlenet::Library* Battlenet::Library::Load(const char* path, Allocator& allocator) {
    int32_t length = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);

    if (length <= 0) {
        return NULL;
    }

    wchar_t* widePath = static_cast<wchar_t*>(allocator.Alloc(2 * length));

    if (!widePath) {
        return NULL;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, path, -1, widePath, length) > 0) {
        HMODULE module = LoadLibraryW(widePath);

        if (module) {
            void* memory = allocator.Alloc(sizeof(WindowsLibrary));

            if (memory) {
                WindowsLibrary* library = new (memory) WindowsLibrary();
                library->m_module = module;
                library->m_allocator = &allocator;
                allocator.Free(widePath);
                return library;
            }
        }
    }

    allocator.Free(widePath);
    return NULL;
}

// OFFSET: 0x006634D0/0x1403095F0
Battlenet::WindowsLibrary::~WindowsLibrary() {
    FreeLibrary(static_cast<HMODULE>(this->m_module));
}

#endif
