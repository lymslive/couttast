#include "tinytast.hpp"
#include "t_sizeof.h"
#include <string>

int CStatic::value = 0;

// 可以省略描叙
// DEF_TAST(sizeofA_primary)
DEF_TAST(sizeofA_primary, "基础类型大小")
{
	int i = 0;

	DESC("sizeof 操作符可用于变量或类型");
	COUT(sizeof(i), 4);
	COUT(sizeof(int), 4);
	COUT(sizeof i, 4);

	COUT(sizeof(unsigned int) == sizeof(int), true);

	DESC("整数大小");
	COUT(sizeof(bool), 1);
	COUT(sizeof(char), 1);
	COUT(sizeof(short), 2);
	COUTF(sizeof(int), 4);
	COUTF(sizeof(long));
	COUTF(sizeof(long long));

	DESC("浮点数大小");
	COUT(sizeof(float), 4);
	COUTF(sizeof(double), 8);
	COUTF(sizeof(long double), 8);

	DESC("指针大小");
	int *pi = nullptr;
	double *pd = nullptr;
	COUT(sizeof(pi), 8);
	COUT(sizeof(pd), 8);
	COUT(sizeof(void*), 8);
}

static
void test_fun_arr(int arr[])
{
	// 会有 warnning
	// -Wsizeof-array-argument
	DESC("数组参数传给函数");
	COUT(sizeof(arr));
	COUT(sizeof(arr) == sizeof(void*));
}

DEF_TAST(sizeofB_array, "数组大小")
{
	SEmpty arrEm[5];
	int arrInt[5];

	COUT(sizeof(arrEm));
	COUT(sizeof(arrInt));

	DESC("获取数组大小经典办法");
	int arr[] = {1, 2, 3, 4};
	COUT(sizeof(arr)/sizeof(arr[0]));

	test_fun_arr(arr);

	{
		DESC("空数组大小");
		SEmpty arrEm[0];
		char arrChr[0];
		int arrInt[0];

		COUT(sizeof(arrEm));
		COUT(sizeof(arrInt));
		COUT(sizeof(arrChr));
	}


	DESC("含空数组的结构体大小");
	COUT(sizeof(SNormal), 8);
	COUT(sizeof(SEafront));
	COUT(sizeof(SEamid));
	COUT(sizeof(SEaend));
}

DEF_TAST(sizeofC_struct, "结构体大小")
{
	DESC("结构体成员对齐");
	COUT(sizeof(SNumberUp), 16);
	COUT(sizeof(SNumberDown), 16);
	COUT(sizeof(SNumberMix), 12);
	COUT(sizeof(SNumberMixD), 24);

	DESC("结构体成员地址");
	COUT(&SNumberMixD::c);
	COUT(&SNumberMixD::d);
	COUT(&SNumberMixD::s);

	printf("printf address SNumberMixD: %d %d %d\n", &SNumberMixD::c, &SNumberMixD::d, &SNumberMixD::s);

	DESC("结构体对齐压缩");
	COUT(sizeof(SNumberPack), 18);
	COUT(sizeof(SNumberPack1), 11);
}

DEF_TAST(sizeofD_union, "联合体大小")
{
	DESC("联合体大小取决于最大成员");
	COUT(sizeof(UNumberUp), 8);
	COUT(sizeof(UNumberDown), 8);
	COUT(sizeof(UNumberMix), 8);
	COUT(sizeof(UNumberExtra), 16);

	DESC("利用 union 检测字节序");
	UByteOrder test;
	test.value = 0x0001;
	if (test.byte[0] == 0x00 && test.byte[1] == 0x01)
	{
		DESC("big endian");
	}
	else if (test.byte[0] == 0x01 && test.byte[1] == 0x00)
	{
		DESC("little endian");
	}
	else
	{
		DESC("unkown endian");
	}
}

DEF_TAST(sizeofE_empty_class, "空类大小")
{
	DESC("空类");
	COUT(sizeof(CEmpty), 1);
	COUT(sizeof(SEmpty), 1);

	DESC("空类有虚函数");
	COUT(sizeof(VEmpty), 8);

	DESC("继承空类");
	COUT(sizeof(DCEmpty));
	COUT(sizeof(DVEmpty));

	DESC("空类增加虚函数");
	COUT(sizeof(VEmptyOne));
	COUT(sizeof(VEmptyTwo));

	DESC("空类增加普通函数");
	COUT(sizeof(MEmpty));

	DESC("空类只有静态成员");
	COUT(sizeof(CStatic), 1);
	COUT(sizeof(CStatic::value), 4);
}

DEF_TAST(sizeof_string, "std::string大小")
{
	DESC("std::string 实现可能依编译器及版本有所不同");
	DESC("但是 -std= 标准选项应该不影响其大小");
	COUT(sizeof(std::string));
	COUT(sizeof(std::string), 8);
}

