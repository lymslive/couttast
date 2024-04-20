/**
 * @file tinyini.h
 * @author lymslive
 * @date 2023-06-13 2024-04-20
 * @brief tiny ini that extend comand line argumet input.
 * @details 
 * The cli option `--section.key=val` can save in ini file in `[section]`.
 * */
#ifndef TINYINI_H__
#define TINYINI_H__

#include "tinytast.hpp"

namespace tast
{
    
/// Treat ini config file as extra cli argument inputs.
class CTinyIni : public CTinyCli
{
public:
    void ParseLines(const std::vector<std::string>& args);
    bool LoadIni(const std::string& file);

    /// Merge `that` into `this`, self arguments and options take precedence.
    void Merge(CTinyIni& that);
};

} /* tast */ 

#endif /* end of include guard: TINYINI_H__ */
