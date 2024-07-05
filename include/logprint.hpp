/**
 * @file logprint.hpp
 * @author lymslive
 * @date 2021-12-14
 * @brief lightweith log front in printf style with type safe (C++11)
 * @lisence MIT
 * */
#pragma once
#ifndef LOGPRINT_HPP__
#define LOGPRINT_HPP__

#include <cstring>
#include <cstdio>
#include <string>
#include <type_traits>
#include <typeinfo>

/// Utility namespace which is named after `std::`.
namespace utd
{

/* ********************************************************************** */
/// @section to_string(), convert any type to string as much as possible

/** printf some type value to an already existed string buffer */
inline std::string& to_string(std::string& target, const std::string& str)
{
    return target.append(str);
}

inline std::string& to_string(std::string& target, const char* str)
{
    return target.append(str);
}

/** print pointer as address unless it is char* where treated as string.
 * @note also print it typeid name.
 * */
template <typename T>
std::string& to_string(std::string& target, T* ptr)
{
    char str[16] = {0};
    target.append(typeid(ptr).name());
    std::sprintf(str, "%p", ptr);
    return target.append(str);
}

inline std::string& to_string(std::string& target, bool tf)
{
    return target.append(tf ? "true" : "false");
}

template <typename numT>
typename std::enable_if<std::is_arithmetic<numT>::value, std::string&>::type
/*std::string&*/ to_string(std::string& target, const numT& val)
{
    return target.append(std::to_string(val));
}

// when an object define memthod: std::string to_string() const
template <typename objT>
typename std::enable_if<std::is_member_function_pointer<decltype(&objT::to_string)>::value, std::string&>::type
/*std::string&*/ to_string(std::string& target, const objT& val)
{
    return target.append(val.to_string());
}

template <typename T>
inline std::string to_string(const T& val)
{
    std::string str;
    return utd::to_string(str, val);
}

/* ********************************************************************** */
/// @section implement of utd::sprintf

/// Internal namespace to support other public functionality.
namespace helper
{

inline
const char* sprintf(std::string& buffer, const char* format)
{
    buffer.append(format);
    return nullptr;
}

template <typename headT>
const char* sprintf(std::string& buffer, const char* format, const headT& first)
{
    const char* slot = std::strchr(format, '%');
    if (slot == nullptr)
    {
        buffer.append(format);
        return nullptr;
    }

    buffer.append(format, slot);
    utd::to_string(buffer, first);

    format = ++slot;
    if (*format >= 'a' && *format <= 'z' || *format >= '0' && *format <= '9')
    {
        ++format;
    }
    return format;
}

template <typename headT, typename... Types>
const char* sprintf(std::string& buffer, const char* format, const headT& first, const Types& ...args)
{
    if (!format || *format == '\0')
    {
        return nullptr;
    }

    format = utd::helper::sprintf(buffer, format, first);
    return utd::helper::sprintf(buffer, format, args...);
}

} /* helper */ 

template <typename... Types>
void sprintf(std::string& buffer, const char* format, const Types& ...args)
{
    format = utd::helper::sprintf(buffer, format, args...);
    if (format)
    {
        buffer.append(format);
    }
}

template <typename... Types>
void sprintf(std::string& buffer, const std::string& format, const Types& ...args)
{
    return utd::sprintf(buffer, format.c_str(), args...);
}

template <typename formatT, typename... Types>
void fprintf(FILE* file, const formatT& format, const Types& ...args)
{
    std::string buffer;
    utd::sprintf(buffer, format, args...);
    std::fwrite(buffer.c_str(), 1, buffer.size(), file);
}

template <typename formatT, typename... Types>
void printf(const formatT& format, const Types& ...args)
{
    utd::fprintf(stdout, format, args...);
}

/* ********************************************************************** */
/// @section back compatible special format

/** format any type of value in std::snprintf style. */
template <typename... Types>
std::string strf(const char* format, const Types& ...args)
{
    int length = std::snprintf(nullptr, 0, format, args...);
    std::string buffer;
    buffer.reserve(length + 1);
    buffer.resize(length);
    std::snprintf(const_cast<char*>(buffer.c_str()), length+1, format, args...);
    return buffer;
}

#ifndef UTD_STRF_MAX_BUFFER
#define UTD_STRF_MAX_BUFFER 64
#endif

/** format any type of value in std::snprintf style, within limited buffer.
 * @details Expect more efficient than utd::strf as only one snprintf call.
 * The stack buffer size can be custom defined in UTD_STRF_MAX_BUFFER which is
 * default 64.
 * */
template <typename... Types>
std::string strF(const char* format, const Types& ...args)
{
    if (UTD_STRF_MAX_BUFFER <= 0)
    {
        return strf(format, args...);
    }
    char buffer[UTD_STRF_MAX_BUFFER];
    std::snprintf(buffer, sizeof(buffer), format, args...);
    return buffer;
}

union UEndianChecker
{
    int i;
    char c;
};

/** check current cpu is little endian, return false if big endian. */
inline bool little_endian()
{
    UEndianChecker endian;
    endian.i = 1;
    return endian.c == 1;
}

struct CHex
{
    static std::string From(const unsigned char* ptr, size_t length, bool upper = false)
    {
        const char* HEX = upper ? "0123456789ABCDEF" : "0123456789abcdef";
        if (!ptr || !length)
        {
            return "";
        }
        std::string buffer;
        buffer.reserve(length * 3 + 1);
        for (size_t i = 0; i < length; ++i)
        {
            buffer.push_back(HEX[(ptr[i] >> 4) & 0x0F]);
            buffer.push_back(HEX[(ptr[i]) & 0x0F]);
            buffer.push_back(' ');
        }
        buffer.pop_back();
        return buffer;
    }

    static std::string FromEnd(const unsigned char* ptr, size_t length, bool upper = false)
    {
        unsigned char* reptr = new unsigned char[length];
        for (size_t i = 0; i < length; ++i)
        {
            reptr[i] = ptr[length - i - 1];
        }
        std::string buffer = From(reptr, length, upper);
        delete[] reptr;
        return buffer;
    }
};

/** print val in hex bytes with lower 0-9a-f */
template <typename T>
std::string strhex(const T& val)
{
    return std::is_integral<T>::value && little_endian()
        ?  CHex::FromEnd(reinterpret_cast<const unsigned char*>(&val), sizeof(T))
        :  CHex::From(reinterpret_cast<const unsigned char*>(&val), sizeof(T));
}

template <typename T>
std::string strhex(const T* val, size_t length)
{
    return CHex::From(reinterpret_cast<const unsigned char*>(val), length);
}

/** print val in hex bytes with upper 0-9A-F */
template <typename T>
std::string strheX(const T& val)
{
    return std::is_integral<T>::value && little_endian()
        ?  CHex::FromEnd(reinterpret_cast<const unsigned char*>(&val), sizeof(T), true)
        :  CHex::From(reinterpret_cast<const unsigned char*>(&val), sizeof(T), true);
}

template <typename T>
std::string strheX(const T* val, size_t length)
{
    return CHex::From(reinterpret_cast<const unsigned char*>(val), length, true);
}

/** print val in binary bits. */
template <typename T>
std::string strbit(const T& val)
{
    static const char *BIT[16] = {
        [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
        [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
        [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
        [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
    };

    std::string buffer;
    buffer.reserve(sizeof(T) * 9 + 1);
    std::string hex = strhex(val);
    for (auto c : hex)
    {
        if (c >= '0' && c <= '9')
        {
            buffer.append(BIT[c-'0']);
        }
        else if (c >= 'a' && c <= 'f')
        {
            buffer.append(BIT[c-'a'+10]);
        }
        else
        {
            buffer.push_back(c);
        }
    }

    return buffer;
}

/* ********************************************************************** */
/// @section log front implement

/** collection of information about one typical log expression. */
struct CLogData
{
    int level = 0;
    int line = 0;
    const char* file = nullptr;
    const char* function = nullptr;
    const char* message = nullptr;
    size_t length;
};

struct CLogTargetConcept
{
    static void log(const CLogData& data, void* context) {}
};

struct CDefaultLog
{
    static void log(const CLogData& data, void* context = nullptr)
    {
        FILE* fp = stderr;
        if (context)
        {
            fp = (FILE*) context;
        }
        std::fprintf(fp, "[%s:%d (%s)] %s\n", data.file, data.line, data.function, data.message);
    }
};

/** Generic Logger class.
 * @param logT, a concept type to handle how and where to write log
 * @param TOPIC, log for some differnet topic/group, may be usefull to enable/disable
 * with macro in complie time, or other purpose.
 * @note the `level` and `context` is public member that can set directlly if
 * needed.
 * */
template <typename logT, uint32_t TOPIC = 0>
struct CLoggerT
{
    /// the min log level setting, any log whose level is less won't write
    int level = 0;
    /// the extra user provided argument that will pass to logT::log()
    void* context = nullptr;

    /** write log with format and any args in printf style, but type safe
     * @note usually call by macro __FILE__, __LINE__, __FUNCTION__ 
     * */
    template <typename formatT, typename... Types>
    void log(int level,
            const char* file, int line, const char* function,
            const formatT& format, const Types& ...args)
    {
        if (this->level > level)
        {
            return;
        }

        std::string buffer;
        utd::sprintf(buffer, format, args...);

        CLogData st;
        st.level = level;
        st.line = line;
        st.file = file;
        st.function = function;
        st.message = buffer.c_str();
        st.length = buffer.size();

        logT::log(st, context);
    }

    /// provide an instance, while not necessary singleton
    static CLoggerT& instance()
    {
        static CLoggerT me;
        return me;
    }
};

/** A simple example logger type that just log to stderr.
 * @note Actually can specify other FILE* beside stdout.
 *  */
typedef CLoggerT<CDefaultLog> CLogger;

/** pre-defined log macor, can re-define if necessary.
 * @details 
 * - COUT_LOG: write all log with level 0, unless explicly set with unusual level
 *   bellow -1.
 * - COUT_LOGX: write log with extra level argument at the beginning.
 * - COUT_DBG: only write log at _DEBUG version.
 * */
#ifndef COUT_LOG
#define COUT_LOG(format, ...) utd::CLogger::instance().log(0, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__)
#define COUT_LOGX(level, format, ...) utd::CLogger::instance().log(level, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__)
#endif

#ifndef COUT_DBG
#ifdef _DEBUG
typedef CLoggerT<CDefaultLog, 0xdeb0deb0> CDebugLogger;
#define COUT_DBG(format, ...) utd::CDebugLogger::instance().log(0, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__)
#else
#define COUT_DBG(format, ...) 
#endif
#endif
} /* namespace utd */ 

#endif /* end of include guard: LOGPRINT_HPP__ */
