/**
 * @file tinyini.h
 * @author lymslive
 * @date 2023-06-13
 * @brief tiny ini that extend comand line argumet input.
 * @details 
 * The cli option `--section.key=val` can save in ini file in `[section]`.
 * */
#ifndef TINYINI_H__
#define TINYINI_H__

#include "tinytast.hpp"
#include "coutstd.hpp"

namespace tast
{
    
/// Wrap a `CTinyCli` to extend some methods.
class CTinyCliPtr
{
    const CTinyCli* m_ptr = nullptr;

public:
    CTinyCliPtr() {}
    CTinyCliPtr(const CTinyCli* ptr) : m_ptr(ptr) {}

    /// get option value in string value, pass `key` or `kp1.kp2` in tow parts
    bool GetValue(std::string& val, const std::string& key);
    bool GetValue(std::string& val, const char* kp1, const char* kp2);

    bool GetValue(int& val, const std::string& key);
    bool GetValue(int& val, const char* kp1, const char* kp2);
};

/// Treat ini config file as extra cli argument inputs.
class CTinyIni : public CTinyCli
{
public:
    void ParseLines(const std::vector<std::string>& args);
    bool LoadIni(const std::string& file);

    /// Merge `that` into `this`, self arguments and options take precedence.
    void Merge(CTinyIni& that);

    /// forward to `CTinyCliPtr::GetValue`
    template <typename... Targs>
    bool GetValue(Targs&&... Fargs) const
    {
        return CTinyCliPtr(this).GetValue(std::forward<Targs>(Fargs)...);
    }

};

} /* tast */ 

/// operator<< overload for tast::CTinyCli in global namesapce.
std::ostream& operator<<(std::ostream& os, const tast::CTinyCli& cli);

#endif /* end of include guard: TINYINI_H__ */
