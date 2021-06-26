#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// define tast_main symbol name and function type
typedef int(*tast_main_ft)(int, char**);
static const char* s_tast_main_sym = "tast_main";

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "required a *.so filename\n");
		return -1;
	}

	const char* file = argv[1];
	int flag = RTLD_NOW | RTLD_GLOBAL;
	void* pHandle = dlopen(file, flag);
	if (!pHandle)
	{
		fprintf(stderr, "load dll failed: %s, errno=%d, msg=%s\n", file, errno, dlerror());
		return (errno == 0 ? -1 : errno);
	}

	tast_main_ft pTestMain = (tast_main_ft)dlsym(pHandle, s_tast_main_sym);
	if (!pTestMain)
	{
		fprintf(stderr, "no test_main function in dll: %s\n", file);
		return -1;
	}

	// pass the rest of arguments
	int iRet = pTestMain(argc-1, argv+1);
	dlclose(pHandle);

	return iRet;
}
