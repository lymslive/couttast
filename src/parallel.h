/** 
 * @file parallel.h
 * @author lymslive
 * @date 2023-06-13
 * @brief Run plenty of test cases in parallel, with multiple process.
 * */
#ifndef PARALLEL_H__
#define PARALLEL_H__

#include "format.h"

namespace tast
{

/// The min amount of test cases to enable child process.
const int MINLIST_TO_PROCESS = 8;

/** Run a list of test cases in multiple process.
 * @param [IN] tastList, a filtered list of test cases to be run.
 * @param [IN] workers, the count of child process to run those test cases.
 * @param [IN] point to test manager where to run the cases.
 * @details 
 *  The `tastList` is may part of test cases from `pTastMgr` filtered according
 *  to command line argument, as well as the `workers` count, and `pTastMgr`
 *  is default to the global `CTastMgr` instance.
 * */
int process_run(TastList& tastList, int workers, CTastMgr* pTastMgr = nullptr);

/** Run all test cases from the test pool in multiple process.
 * */
int process_run(const TastPool& tastPool, int workers, CTastMgr* pTastMgr = nullptr);

} // end of namespace tast


#endif /* end of include guard: PARALLEL_H__ */
