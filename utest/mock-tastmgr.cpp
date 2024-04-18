#include "mock-tastmgr.h"

#include <string.h>

namespace mock
{
    
CTastMgr::~CTastMgr()
{
    for (StringBuffer* pBuffer : m_pool)
    {
        delete pBuffer;
    }
}

CTastMgr::StringBuffer* CTastMgr::AllocateBuffer()
{
    StringBuffer* pBuffer = new StringBuffer();
    pBuffer->buffer[IEND-1] = 0;
    pBuffer->tail = &(pBuffer->buffer[0]);
    m_pool.push_back(pBuffer);
    return pBuffer;
}

CTastMgr::StringBuffer* CTastMgr::LastBuffer()
{
    if (m_pool.empty())
    {
        return nullptr;
    }
    return m_pool.back();
}

const char* CTastMgr::SaveString(const std::string& str)
{
    if (str.empty())
    {
        return "";
    }

    if (str.size() >= IEND)
    {
        return nullptr;
    }

    StringBuffer* pBuffer = LastBuffer();
    if (pBuffer == nullptr)
    {
        pBuffer = AllocateBuffer();
    }

    if (pBuffer->tail + str.size() >= pBuffer->end())
    {
        pBuffer = AllocateBuffer();
    }

    const char* store = pBuffer->tail;

    ::memcpy(pBuffer->tail, str.c_str(), str.size());
    pBuffer->tail += str.size();
    *(pBuffer->tail) = '\0';
    pBuffer->tail++;

    return store;
}

const char* CTastMgr::SaveName(const std::string& name)
{
    if (m_last.name != nullptr && m_last.nameLen == name.size() && 0 == memcmp(m_last.name, name.c_str(), name.size()))
    {
        return m_last.name;
    }

    const char* store = SaveString(name);
    m_last.name = store;
    m_last.nameLen = name.size();
    return store;
}

const char* CTastMgr::SaveFile(const std::string& file)
{
    if (m_last.file != nullptr && m_last.fileLen == file.size() && 0 == memcmp(m_last.file, file.c_str(), file.size()))
    {
        return m_last.file;
    }

    const char* store = SaveString(file);
    m_last.file = store;
    m_last.fileLen = file.size();
    return store;
}

const char* CTastMgr::SaveDesc(const std::string& desc)
{
    if (m_last.desc != nullptr && m_last.descLen == desc.size() && 0 == memcmp(m_last.desc, desc.c_str(), desc.size()))
    {
        return m_last.desc;
    }

    const char* store = SaveString(desc);
    m_last.desc = store;
    m_last.descLen = desc.size();
    return store;
}

} /* mock:: */ 
