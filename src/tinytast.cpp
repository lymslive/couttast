#include "tinytast.hpp"

#if defined(__GNUC__) || defined(__clang__)
    #define WEAK_SYMBOL __attribute__((weak))
#elif defined(_MSC_VER)
    #define WEAK_SYMBOL __declspec(selectany)
#else
    #define WEAK_SYMBOL
#endif

/** The default main function to run test by command line arguments.
 * The custom test parogram can define its own main function to override this
 * default main, as it is weak symbol.
 * */
int WEAK_SYMBOL main(int argc, char* argv[])
{
	return RUN_TAST(argc, argv);
}
