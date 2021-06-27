#ifndef T_CLASS_H__
#define T_CLASS_H__

#include "tinytast.hpp"

class CBase
{
public:
    CBase()
    {
        DESC("CBase() called");
    }
    ~CBase()
    {
        DESC("~CBase() called");
    }
    void Init()
    {
        DESC("CBase::Init() called");
    }

    void Process()
    {
        DESC("CBase::Process() Enter");
        Step1();
        Step2();
        DESC("CBase::Process() Exit");
    }

    void Step1()
    {
        DESC("CBase::Step1() called");
    }
    void Step2()
    {
        DESC("CBase::Step2() called");
    }
};

// 继承
// 先构造父类，再构造子类
// 先析构子类部分，再析构父类部分
class CDerived : public CBase
{
public:
    CDerived()
    {
        DESC("CDerived() called");
    }
    ~CDerived()
    {
        DESC("~CDerived() called");
    }
    void Step1()
    {
        DESC("CDerived::Step1() called");
    }
};

// 组合
// 先构造成员，再构造本身
// 先析构自身部分，再析构成员对象部分
class CComposite
{
public:
    CComposite()
    {
        DESC("CComposite() called");
    }
    ~CComposite()
    {
        DESC("~CComposite() called");
    }
private:
    CBase m_base;
};

// 组合，成员对象本身有父类
// 先构造成员（如有父类先构造其父类），再构造本身
// 先析构自身部分，再析构成员对象部分及其父类
class CCompositeSub
{
public:
    CCompositeSub()
    {
        DESC("CCompositeSub() called");
    }
    ~CCompositeSub()
    {
        DESC("~CCompositeSub() called");
    }
private:
    CDerived m_derived;
};

// 另一个类
class CBaseA
{
public:
    CBaseA()
    {
        DESC("CBaseA() called");
    }
    ~CBaseA()
    {
        DESC("~CBaseA() called");
    }
};

// 既有成员对象也有父类
// 先构造父类，再构造成员，再构造自身
// 先析构自身，再析构成员，再析构父类
class CCompositeDer : public CBase
{
public:
	CCompositeDer()
	{
        DESC("CCompositeDer() called");
	}
    ~CCompositeDer()
    {
        DESC("~CCompositeDer() called");
    }
private:
	CBaseA m_baseA;
};

// 多个成员对象
// 先析构自身，然后各成员对象按声明的相反顺序析构
class CCompositeMore
{
public:
    CCompositeMore()
    {
        DESC("CCompositeMore() called");
    }
    ~CCompositeMore()
    {
        DESC("~CCompositeMore() called");
    }
private:
    CBase m_base;
    CBaseA m_baseA;
};

// 显式指定成员对象初始化
// 成员构造顺序按声明顺序，而非构造函数的初始化列表
class CCompositeInit
{
public:
    CCompositeInit() : m_baseA(), m_base()
    {
        DESC("CCompositeInit() called");
    }
private:
    CBase m_base;
    CBaseA m_baseA;
};

// 有虚函数的继承
class CBaseV
{
public:
    virtual ~CBaseV()
    {
        DESC("~CBaseV() called");
    }

    void Init()
    {
        DESC("CBaseV::Init() called");
    }

    void Process()
    {
        DESC("CBaseV::Process() Enter");
        Step1();
        Step2();
        DESC("CBaseV::Process() Exit");
    }

    virtual void Step1()
    {
        DESC("CBaseV::Step1() called");
    }
    virtual void Step2()
    {
        DESC("CBaseV::Step2() called");
    }
};

class CDerivedV : public CBaseV
{
public:
    virtual ~CDerivedV()
    {
        DESC("~CDerivedV() called");
    }
    void Step1()
    {
        DESC("CDerivedV::Step1() called");
    }
};

// 子类不声明虚函数
class CDerivedVI : public CBaseV
{
public:
    ~CDerivedVI()
    {
        DESC("~CDerivedVI() called");
    }
};

// 父类不声明虚函数，但子类声明
class CDerivedVII : public CBase
{
public:
    virtual ~CDerivedVII()
    {
        DESC("~CDerivedVII() called");
    }
};

#endif /* end of include guard: T_CLASS_H__ */
