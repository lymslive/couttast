#include "tinytast.hpp"

#define private public
#define protected public
// 注：在 makefile 加上这样的宏，影响全局，有可能编译失败
// CXXFLAGS += -Dprivate=public -Dprotected=pblic

class CFooPrivate
{
private:
    int m_iValue;
    float m_fValue;

public:
    void SetValue(int iValue)
    {
        m_iValue = iValue;
    }
};

DEF_TAST(private_view, "访问对象私有数据")
{
    CFooPrivate obj;
    COUT(obj.m_iValue);
    obj.SetValue(3);
    COUT(obj.m_iValue, 3);
}
