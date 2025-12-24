#include "tinytast.hpp"
#include "extra-macros.hpp"
#include "coutstd.hpp"
#include "relative-timer.hpp"
#include <algorithm>
#include <cmath>
#include <thread>

// 插入排序实现
void insertionSort(std::vector<int>& data)
{
    for (size_t i = 1; i < data.size(); ++i)
    {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && data[j] > key)
        {
            data[j + 1] = data[j];
            j--;
        }
        data[j + 1] = key;
    }
}

// 冒泡排序实现
void bubbleSort(std::vector<int>& data)
{
    bool swapped;
    int n = data.size();
    do
    {
        swapped = false;
        for (int i = 0; i < n - 1; i++)
        {
            if (data[i] > data[i + 1])
            {
                std::swap(data[i], data[i + 1]);
                swapped = true;
            }
        }
        n--;
    } while (swapped);
}

// 正常用例：从 RelativeTimer 派生的排序性能比较测试
class SortPerformanceTest : public tast::RelativeTimer
{
public:
    std::vector<int> dataA;
    std::vector<int> dataB;

public:
    SortPerformanceTest()
    {
        // 设置测试描述
        name = "排序算法性能比较测试";
        labelA = "插入排序";
        labelB = "冒泡排序";
        
        // 随机生成测试数据
        std::random_device rd;
        std::mt19937 gen(seed != 0 ? seed : rd());
        std::uniform_int_distribution<> dis(1, 1000);
        
        dataA.resize(size);
        dataB.resize(size);
        
        for (int i = 0; i < size; ++i)
        {
            int value = dis(gen);
            dataA[i] = value;
            dataB[i] = value;  // 确保两个方法使用相同的测试数据
        }
    }
    
    // 方法A：插入排序
    virtual void methodA() override
    {
        insertionSort(dataA);
    }
    
    // 方法B：冒泡排序
    virtual void methodB() override
    {
        bubbleSort(dataB);
    }
    
    // 验证功能正确性
    virtual bool methodVerify() override
    {
        // 排序后数组相等
        return dataA == dataB;
    }
};

// 快速排序性能测试：从 RelativeTimer 派生的排序性能比较测试
class QuickSortPerformanceTest : public tast::RelativeTimer
{
public:
    std::vector<int> dataA;
    std::vector<int> dataB;

public:
    // 构造函数接收 size 和 loop 参数
    QuickSortPerformanceTest(int testSize, int testLoop)
    {
        // 设置测试参数
        if (testSize > 0) size = testSize;
        if (testLoop > 0) loop = testLoop;
        
        // 设置测试描述
        name = "快速排序与冒泡排序性能比较测试";
        labelA = "快速排序";
        labelB = "冒泡排序";
        
        // 随机生成测试数据
        std::random_device rd;
        std::mt19937 gen(seed != 0 ? seed : rd());
        std::uniform_int_distribution<> dis(1, 1000);
        
        dataA.resize(size);
        dataB.resize(size);
        
        for (int i = 0; i < size; ++i)
        {
            int value = dis(gen);
            dataA[i] = value;
            dataB[i] = value;  // 确保两个方法使用相同的测试数据
        }
    }
    
    // 方法A：快速排序
    virtual void methodA() override
    {
        std::sort(dataA.begin(), dataA.end());
    }
    
    // 方法B：冒泡排序
    virtual void methodB() override
    {
        bubbleSort(dataB);
    }
    
    // 验证功能正确性
    virtual bool methodVerify() override
    {
        // 排序后数组相等
        return dataA == dataB;
    }
};

// 异常用例：从 RelativeTimer 派生的验证失败测试
class VerificationFailTest : public tast::RelativeTimer
{
private:
    int dummyData;

public:
    VerificationFailTest()
    {
        name = "验证失败测试用例";
        labelA = "空操作A";
        labelB = "空操作B";
        dummyData = 42;
    }
    
    // 方法A：空操作 + sleep
    virtual void methodA() override
    {
        // 模拟一些工作
        volatile int temp = dummyData * 2;
        temp = temp / 2;
        
        // sleep 一段时间
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    // 方法B：空操作 + sleep  
    virtual void methodB() override
    {
        // 模拟一些工作
        volatile int temp = dummyData + 10;
        temp = temp - 10;
        
        // sleep 一段时间
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    
    // 故意返回 false 来触发验证失败
    virtual bool methodVerify() override
    {
        return false;  // 故意失败
    }
};

// 使用 DEF_TOOL 创建正常用例入口
DEF_TOOL(sort_performance, "测试插入排序与冒泡排序的性能比较")
{
    DESC("创建排序性能测试实例");
    SortPerformanceTest test;
    
    DESC("使用 COUT_TIMER 宏运行性能测试并验证结果");
    COUT_TIMER(test);
}

// 使用 DEF_TAST 创建异常用例入口
DEF_TAST(verification_fail, "测试验证失败时返回NaN的情况")
{
    DESC("创建验证失败测试实例");
    VerificationFailTest test;
    
    DESC("使用 COUT_TIMER 宏运行验证失败测试并验证结果");
    COUT_TIMER(test);

    COUT_ERROR(1); // 断言确实有个错误
}

// 快速排序性能测试用例（小规模数据）
DEF_TOOL(qsort_perf, "快速排序性能测试（小规模数据）")
{
    DESC("创建快速排序性能测试实例（不同规模数据）");
    QuickSortPerformanceTest s2(100, 100);
//  COUT(s2.dataA);
//  COUT(s2.dataB);
    COUT_TIMER(s2);
//  COUT(s2.dataA);
//  COUT(s2.dataB);
    
    QuickSortPerformanceTest s3(1000, 100);
    COUT_TIMER(s3);
    
    // 根据命令行参数测试
    QuickSortPerformanceTest s0(0, 0);
    COUT_TIMER(s0);
}

