#pragma once
#ifndef MOCK_TASTMGR_H__
#define MOCK_TASTMGR_H__

#include "tinytast.hpp"

namespace mock
{
    
/// Mock CTastMgr that store strings to make the `const char*` pointer in tast
/// cases is keeping valid.
class CTastMgr : public tast::CTastMgr
{
public:
    const char* SaveName(const std::string& name);
    const char* SaveFile(const std::string& file);
    const char* SaveDesc(const std::string& desc);

    ~CTastMgr();

private:
    const char* SaveString(const std::string& str);

    struct LastCase
    {
        const char* name = nullptr;
        const char* file = nullptr;
        const char* desc = nullptr;
        size_t nameLen = 0;
        size_t fileLen = 0;
        size_t descLen = 0;
    };

    // typedef std::vector<char> StringBuffer;
    static const int IEND = 4096 - 8;
    struct StringBuffer
    {
        char* tail = nullptr;
        char buffer[IEND];
        char* end() { return &buffer[IEND]; }
    };

    StringBuffer* AllocateBuffer();
    StringBuffer* LastBuffer();

    typedef std::vector<StringBuffer*> StringPool;

    LastCase m_last;
    StringPool m_pool;
};

} /* mock:: */ 


#endif /* end of include guard: MOCK_TASTMGR_H__ */
