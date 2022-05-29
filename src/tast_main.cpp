#include "tinytast.hpp"

#define WEAK_SYMBOL __attribute__((weak))

/** The default main function to run test by command line arguments.
 * The client test parogram can define its own main function to override this
 * default main, as it is weak symbol.
 * */
int WEAK_SYMBOL main(int argc, char* argv[])
{
	return RUN_TAST(argc, argv);
}
