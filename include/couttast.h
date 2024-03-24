/**
 * @file couttast.h
 * @author tanshuil
 * @date 2024-03-24
 * @brief 单元测试主程序库导出的可用全局变量
 * @note 提供给外部单元测试程序 include 的头文件。
 * 需要用到 LY 配置与日志功能时请包含该头文件，不需要时可只包含 `tinytast.hpp`
 * 使用单元测试框架提供的基本宏，以及提供个基本的 main() 入口函数。
 * @note 配置文件可选，读取在当前目录下与测试程序同名加 `.ini` 后缀的文件。
 * */
#ifndef TEST_MAIN_H__
#define TEST_MAIN_H__

#include "tinytast.hpp"

#define HAS_UNIT_TEST 1

namespace tast
{

/// Type for command line argument and option.
typedef std::map<std::string, std::string> Options;
typedef std::vector<std::string> Arguments;

/// Get the global cli option and argument, stored in `CTastMgr` instance.
bool GetOption(const std::string& key, std::string& val);
bool GetOption(const std::string& key, int& val);
const std::vector<std::string>& GetArguments();

} // end of namespace tast

/// 使用函数只调用一次，后续调用直接返回可选参数.
#define CALL_FUNCTION_ONCE(...) \
    static bool once_ = false; \
    if (once_) { return __VA_ARGS__; } \
    once_ = true

/// 改名
#define DEF_TEST(...) DEF_TAST(__VA_ARGS__)
#define RUN_TEST(...) RUN_TAST(__VA_ARGS__)

#endif /* end of include guard: TEST_MAIN_H__ */
