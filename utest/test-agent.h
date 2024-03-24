#ifndef TEST_AGENT_H__
#define TEST_AGENT_H__

#include "couttast.h"

/// Fill `nCount` test cases in `stTastMgr`, whose name and file contains
/// `prefix` which is default "sample".
void FillSampleTast(tast::CTastMgr& stTastMgr, int nCount, const char* prefix = nullptr);

#endif /* end of include guard: TEST_AGENT_H__ */
