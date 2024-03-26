#ifndef EXTRA_MACROS_HPP__
#define EXTRA_MACROS_HPP__

#include "tinytast.hpp"
#include "gtest-macros.hpp"

/// Expect to have `count` of errors by now, and reset to 0.
/// May used when failed statement is intentional and then make the case pass.
#define COUT_ERROR(count) tast::CStatement(SRC_LOCATION, "errors").cout_error(count)

/// Try `statement` expect to throw std::exception with `message` in what().
/// The `message` should be literal string as `const char*` type, and
/// when it is nullptr, then `statement` should not throw exception,
/// when it is empty string `""`, ignore match what() message.
/// Not report fail if the throw object is not derived from `std::exception`.
#define COUT_EXCEPTION(statement, message) do { \
    try { statement; } \
    catch (const std::exception& e) { \
        if (message == nullptr) { \
            const char* exception = "caught exception"; \
            COUT(exception, "no exception"); \
        } \
        else { \
            const char* exception = e.what(); \
            bool match = std::string(exception).find((const char*)message) != std::string::npos; \
            COUT(exception, (const char*)message, match); \
        } \
        break; \
    } \
    catch (...) { \
        if (message == nullptr) { \
            const char* exception = "caught exception"; \
            COUT(exception, "no exception"); \
        } \
        else { \
            const char* exception = "not caught std::exception"; \
            COUT(exception, "catch std::exception or derived"); \
        } \
        break; \
    } \
    if (message != nullptr) { \
        const char* exception = "not caught any exception"; \
        COUT(exception, "catch some exception"); \
    } \
} while(0)

#endif /* end of include guard: EXTRA_MACROS_HPP__ */
