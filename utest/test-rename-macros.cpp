#include "rename-macros.hpp"

fun_test(rename_fun, "use fun_test to define easy test case")
{
    expect(sizeof(int), 4);
    assert_ret(G_TESTMGR != NULL, true);
    expect(G_TESTMGR->GetTastRun()->m_name, "rename_fun");
}

fun_tool(rename_fun_tool, "use fun_tool to define easy tool case")
{
    COUT(TEST_ARGV[0]);
    COUT(TEST_ARGV[1]);
    COUT(TEST_ARGV["help"]);
    COUT(TEST_ARGV["hello"]);

    TEST_OPTION["hello"] = "world";
    COUT(TEST_ARGV["hello"], "world");
}

namespace space
{

double pi;

unit_setup()
{
    DESC("space::unit_setup called");
    pi = 3.14;
}

unit_teardown()
{
    DESC("space::teardown called");
}

unit_test(rename_unit, "use unit_test to define unit case")
{
    expect(pi, 3.14);
}

unit_tool(rename_unit_tool, "use unit_tool to define unit case --radius")
{
    expect(pi, 3.14);
    double radius = 1.0;
    BIND_ARGV(radius);
    double area = 2 * pi * radius;
    COUT(radius);
    COUT(area);
}

} // end of space::

struct Area
{
    static double pi;
    static void setupFirst()
    {
        DESC("Area::setupFirst called");
        pi = 3.142;
    }
    static void teardownLast()
    {
        DESC("Area::teardownLast called");
        pi = 0;
    }

    double radius;
    Area() : radius(1.5) {}

    void setup()
    {
        DESC("Area::setup called");
        radius = 2.0;
    }
    void teardown()
    {
        DESC("Area::teardown called");
        radius = 0;
    }
};

double Area::pi;

class_test(Area, retest, "use class_test to define unit case")
{
    expect(pi, 3.142);
    expect(radius, 2.0);
    double area = 2 * pi * radius;
    COUT(area);
}

class_tool(Area, retool, "use class_tool to define unit case --radius")
{
    expect(pi, 3.142);
    BIND_ARGV(radius);
    double area = 2 * pi * radius;
    COUT(radius);
    COUT(area);
}


static double s_pi;
unit_setup()
{
    s_pi = 3.14159;
}

unit_test(rename_area, "unit test in file")
{
    expect(s_pi, 3.14159);
    double radius = 1.0;
    double area = 2 * s_pi * radius;
    COUT(area);
}

unit_test(rename_area2, "unit test in file, reuse class by self")
{
    Area self;
    expect(s_pi, 3.14159);
    expect(self.radius, 1.5);
    double area = 2 * s_pi * self.radius;
    COUT(area);
}

unit_tool(rename_utool, "unit tool in file to calculate area --radius")
{
    DESC("some pi may not initiated");
    COUT(space::pi);
    COUT(Area::pi);
    COUT(s_pi);
    double radius = 1.0;
    BIND_ARGV(radius);
    double area = 2 * s_pi * radius;
    COUT(area);
}

fun_tool(rename_ftool, "plain tool in file to calculate area --radius")
{
    DESC("some pi may not initiated");
    COUT(space::pi);
    COUT(Area::pi);
    COUT(s_pi);
    double radius = 1.0;
    BIND_ARGV(radius);
    double area = 2 * s_pi * radius;
    COUT(area);
}
