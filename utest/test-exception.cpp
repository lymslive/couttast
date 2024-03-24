#include "extra-macros.hpp"

DEF_TOOL(except_raw, "test exception")
{
    int m = 1;
    int n = 0;
    do {
        try
        {
            throw 42;
            throw std::runtime_error("runtime error:");
            // div 0 not throw but abort the process
            COUT(m/n);
        }
        catch (const std::exception& e)
        {
            COUT(e.what());
            break;
        }
        catch (...)
        {
            COUT("catch not std::exception");
            break;
        }
        DESC("after catch");
    } while(0);

    DESC("done: after do while");

    std::cout << (const char*)nullptr << std::endl;
    COUT((const char*)nullptr);
}

struct CMyError : public std::logic_error
{
    std::string message;
    virtual const char* what() const noexcept override
    {
        return message.c_str();
    }

    CMyError(const char* message_in)
        : std::logic_error(message_in), message(message_in)
    {}
};

void MyFunction()
{
    if (1)
    {
        throw CMyError("my error");
    }
}

DEF_TAST(except_macro, "test exception")
{
    DESC("throw 42"); // fail
    COUT_EXCEPTION(throw 42, "int exception");

    DESC("throw std::runtime_error, match what()");
    COUT_EXCEPTION(throw std::runtime_error("runtime error"), "runtime");

    DESC("throw std::runtime_error, dismatch what()"); // fail
    COUT_EXCEPTION(throw std::runtime_error("runtime error"), "system");

    DESC("throw std::runtime_error, ignore match");
    COUT_EXCEPTION(throw std::runtime_error("runtime error"), "");

    DESC("throw std::runtime_error, but expect no exception"); // fail
    COUT_EXCEPTION(throw std::runtime_error("runtime error"), nullptr);

    DESC("throw 42, but expect no exception"); // fail
    COUT_EXCEPTION(throw 42, nullptr);

    DESC("normal statement, no exception");
    COUT_EXCEPTION(1+1, nullptr);

    DESC("try block");
    COUT_EXCEPTION({ int i = 1; ++i; }, NULL);
    COUT_EXCEPTION({ int i = 1; ++i; }, "any exception?"); // fail

    DESC("try function call");
    COUT_EXCEPTION(MyFunction(), nullptr); // fail
    COUT_EXCEPTION(MyFunction(), "");
    COUT_EXCEPTION(MyFunction(), "my");

    COUT_ERROR(6);
}
