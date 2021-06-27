#include "t_class.h"

DEF_TAST(class1_ctor, "对象构造顺序")
{
    // CODE 宏在执行语句前打印将要执行的语句
    CODE(CBase base);
    COUT(sizeof(base), 1);

    CODE(CDerived derived);
    COUT(sizeof(derived), 1);

    CODE(CComposite composite);
    COUT(sizeof(composite), 1);

    CODE(CCompositeSub compositeSub);
    COUT(sizeof(compositeSub), 1);

    CODE(CCompositeDer compositeDer);
    COUT(sizeof(compositeDer), 1);

    CODE(CCompositeMore compositeMore);
    COUT(sizeof(compositeMore), 2);

    CODE(CCompositeInit compositeInit);
    COUT(sizeof(compositeInit), 2);
}

DEF_TAST(class2_dector, "对象析构顺序")
{
    {
        CODE(CBase base);
        CODE(CBaseA baseA);
    }

    {
        CODE(CDerived derived);
    }

    {
        CODE(CComposite composite);
    }

    {
        CODE(CCompositeSub compositeSub);
    }

    {
        CODE(CCompositeDer compositeDer);
    }

    {
        CODE(CCompositeMore compositeMore);
    }
}

DEF_TAST(class3_virtual, "虚析构顺序")
{
    {
        CBase* pBase = new CBase;
        CBase* pBDerive = new CDerived;
        CDerived* pDerive = new CDerived;

        delete pBase;
        delete pBDerive;
        delete pDerive;
    }
    DESC(""); // 空行

    {
        CBaseV* pBase = new CBaseV;
        CBaseV* pBDerive = new CDerivedV;
        CDerivedV* pDerive = new CDerivedV;

        delete pBase;
        delete pBDerive;
        delete pDerive;
    }
    DESC("");

    // 也正常，子类自动成为虚函数
    {
        CBaseV* pBDerive = new CDerivedVI;
        delete pBDerive;
    }
    DESC("");

    // 不正常，只能析构父类
    {
        CBase* pBDerive = new CDerivedVII;
        delete pBDerive;
    }
    DESC("");

    // 也正常，按常规先析构自己再析构父类
    {
        CDerivedVII* pDerive = new CDerivedVII;
        delete pDerive;
    }
}

DEF_TAST(class4_method, "普通方法")
{
    CBase base;
    CDerived derived;

    CODE(base.Init());
    DESC("");

    CODE(derived.Init());
    DESC("");

    CODE(base.Process());
    DESC("");

    CODE(derived.Process());
    DESC("");

    CODE(derived.Step1());
    DESC("");

    CBase* pbd = &derived;
    CODE(pbd->Step1());
    DESC("");

    /*
     * 子类调用 Process ，进入了父类的命名空间，在其内调用的 Step 方法仍是父类的，
     * 没法调用子类的，虚函数才能处理这种多态行为
     */
}

DEF_TAST(class5_virmethod, "虚函数方法")
{
    CBaseV base;
    CDerivedV derived;

    CODE(base.Init());
    DESC("");

    CODE(derived.Init());
    DESC("");

    CODE(base.Process());
    DESC("");

    CODE(derived.Process());
    DESC("");

    CODE(derived.Step1());
    DESC("");

    CBaseV* pbd = &derived;
    CODE(pbd->Step1());
    DESC("");
}
