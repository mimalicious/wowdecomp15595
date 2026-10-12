#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_STREAMING_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_STREAMING_INL

// INLINED
inline FileStack_Streaming::FileStack_Streaming(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms) : m_fs(fs), m_parms(parms), m_key(System_File::GetInode(std::string(parms->m_path))), m_unit(NULL) {
}

#endif
