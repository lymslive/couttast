#include "unittast.hpp"

int* s_pBuffer = NULL;
void cout_counter();

SETUP()
{
    s_pBuffer = new int[8];
    DESC("file SETUP called");
    cout_counter();
    s_pBuffer[0] = s_unit.tastCount;
}

TEARDOWN()
{
    delete[] s_pBuffer;
    s_pBuffer = NULL;
    DESC("file TEARDOWN called");
    cout_counter();
}

void cout_counter()
{
    COUT(s_pBuffer);
    COUT(s_unit.tastCount);
    COUT(s_unit.runCount);
}

DEF_TAST(unit_normal, "normal test not in uint")
{
    cout_counter();
    // s_pBuffer may not inited.
    // COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TAST(unit_auto1, "test in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
    int* pBuffer = new int[16];
    pBuffer[15] = 1;
    COUT(pBuffer);
    delete[] pBuffer;
}

DEU_TAST(unit_auto2, "test in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TOOL(unit_tool1, "tool in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TOOL(unit_tool2, "tool in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

namespace unest
{

double* s_pBuffer = NULL;
void cout_counter();

SETUP()
{
    s_pBuffer = new double[4];
    DESC("unest::SETUP called");
    cout_counter();
    s_pBuffer[0] = 2.0;
}

TEARDOWN()
{
    delete[] s_pBuffer;
    s_pBuffer = NULL;
    DESC("unest::TEARDOWN called");
    cout_counter();
}

void cout_counter()
{
    COUT(s_pBuffer);
    COUT(s_unit.tastCount);
    COUT(s_unit.runCount);
}

DEU_TAST(unest_auto1, "test in namespace uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
    int* pBuffer = new int[16];
    pBuffer[15] = 1;
    COUT(pBuffer);
    delete[] pBuffer;
}

DEU_TAST(unest_auto2, "test in namespace uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

} // endof unest::

namespace umore
{

double* s_pBuffer = NULL;
void cout_counter();

SETUP()
{
    s_pBuffer = new double[4];
    DESC("umore::SETUP called");
    cout_counter();
    s_pBuffer[0] = 3.0;
}

TEARDOWN()
{
    delete[] s_pBuffer;
    s_pBuffer = NULL;
    DESC("umore::TEARDOWN called");
    cout_counter();
}

void cout_counter()
{
    COUT(s_pBuffer);
    COUT(s_unit.tastCount);
    COUT(s_unit.runCount);
}

// yes, the test name is the same as above unest_auto1
DEU_TAST(unest_auto1, "test in namespace uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TAST(unest_auto2, "test in namespace uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TAST(unest_auto3, "test in namespace uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

} // end of umore::

namespace unot
{

DEU_TAST(unit_auto2, "test in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

DEU_TAST(unit_auto3, "test in uint")
{
    cout_counter();
    COUT(s_pBuffer[0], s_unit.tastCount);
}

} // end of umore
