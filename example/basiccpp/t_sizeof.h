#ifndef CSTRUCTDEF_H__
#define CSTRUCTDEF_H__

class CEmpty
{
};

struct SEmpty
{
};

struct SNormal
{
	int begin;
	int end;
};

struct SEafront
{
	char padding[0];
	int begin;
	int end;
};

struct SEamid
{
	int begin;
	char padding[0];
	int end;
};

struct SEaend
{
	int begin;
	int end;
	char padding[0];
};

class VEmpty
{
public:
    VEmpty(){}
    virtual ~VEmpty() {}
};

class DVEmpty : public VEmpty
{
public:
    DVEmpty(){}
    virtual ~DVEmpty() {}
};

class DCEmpty : public CEmpty
{
public:
    DCEmpty() {}
};

class VEmptyOne : public VEmpty
{
public:
    VEmptyOne(){}
    virtual ~VEmptyOne() {}

    virtual void method() {}
};


class VEmptyTwo : public VEmpty
{
public:
    VEmptyTwo(){}
    virtual ~VEmptyTwo() {}

    virtual void method() {}
    virtual void more() {}
};

class MEmpty : public CEmpty
{
public:
    MEmpty() {}
    int method(int) {}
    int more(int) {}
};

class CStatic
{
public:
    static int value;
};

// 从大到小或从小到大，sizeof = 16
struct SNumberUp
{
    char c;
    short s;
    int i;
    double d;
};

struct SNumberDown
{
    double d;
    int i;
    short s;
    char c;
};

// 4*3 = 12
struct SNumberMix
{
    char c;
    int i;
    short s;
};

// 8*3 = 24
struct SNumberMixD
{
    char c;
    double d;
    short s;
};

// sizeof = 18, 只压缩尾部
struct __attribute__((__packed__)) SNumberPack
{
    char c;
    double d;
    short s;
};

// sizeof = 11
#pragma pack(1)
struct SNumberPack1
{
    char c;
    double d;
    short s;
};
#pragma pack()

// union 大小取决于最大成员
union UNumberUp
{
    char c;
    short s;
    int i;
    double d;
};

union UNumberDown
{
    double d;
    int i;
    short s;
    char c;
};

union UNumberMix
{
    char c;
    double d;
    int i;
    short s;
};

// union 也按最大成员对齐
union UNumberExtra
{
    char c[9];
    double d;
    int i;
    short s;
};

// 利用 union 检测字节序
union UByteOrder
{
    short value;
    char byte[2];
};

#endif /* end of include guard: CSTRUCTDEF_H__ */
