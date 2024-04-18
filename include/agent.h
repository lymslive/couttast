/** 
 * @file agent.h
 * @author lymslive
 * @date 2024-03-24
 * @brief Extend advanced features to the tinytast framework.
 * */
#ifndef TAST_AGENT_H__
#define TAST_AGENT_H__

#include "tinyini.h"

namespace tast
{
    
/** Wrap a tast manager and run with command line argument config.
 * @param [IN] stTastMgr, may the global `G_TASTMGR` but not necessary.
 * @param [IN] cfg, cli option/arguemnt and may merged with ini config.
 * @param [IN] firstArg, the first argument from cli, but not ini, may nullptr.
 * @return int, the count of failed test cases.
 * @details 
 *   The tast agent enhance the basic tast manager, and support some extra
 *   command line options, mainly include following aspects:
 * - more advance filter mechanism to select and unselect some test case to
 *   run, and `--list` or `--List` would list the filtered case only, except
 *   no positional argument provided.
 * - run test concurrently in multiple process or thread, to reduce the run
 *   time if there are many test cases.
 * - Sub command mode, if the first argument match exactly a test case defined
 *   by 'DEF_TOOL'.
 * - Print colourful output to terminal if supported.
 * */
int agent_run(CTastMgr& stTastMgr, CTinyIni& cfg, const char* firstArg);

/** Calculate the average run time of a function.
 * @param [IN] fun, the function to measure time.
 * @param [IN] times, loop times to run `fun`.
 * @param [IN] msleep, the milli-second to sleep after each run, 0 to disable
 *  it. Sleep a while for each run may more realistic for cpu switch.
 * @return the average time in micro-second.
 * @note The macro `TIME_TAST` may not thread safe, it is better to use this
 * function manually to time another function, if want to run many test cases
 * in multiple threads.
 * */
int64_t average_time(voidfun_t fun, int times = 10, int msleep = 1000);

} /* end of tast:: */ 

#endif /* end of include guard: TAST_AGENT_H__ */
