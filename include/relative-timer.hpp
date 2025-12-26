/**
 * @file relative-timer.hpp
 * @author lymslive
 * @date 2025-12-23
 * @brief Relative performance timer.
 * */
#pragma once
#ifndef PERF_RELATIVE_TIMER_HPP_
#define PERF_RELATIVE_TIMER_HPP_

#include "tinytast.hpp"
#include "tastargv.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <random>

namespace tast
{

/**
 * @brief Virtual polymorphic base class for relative performance testing.
 *
 * This class provides a framework for comparing the performance of two methods
 * (methodA and methodB) in derived classes. 
 *
 * Derived classes should override the pure virtual methods methodA and methodB,
 * and optionally methodVerify for functional correctness checking.
 */
class RelativeTimer
{
  public:
    //=== Descriptive name members ===
    std::string name;    //< Test scenario description
    std::string labelA;  //< Description for method A
    std::string labelB;  // Description for method B

    //=== Runtime control members ===
    //would read from command line with prefix `--timer-`.
    int loop = 1000;     //< Total number of iterations for each method
    int batch = 10;      //< Number of batches to divide the loops into
    int size = 100;      //< Size parameter for test data
    int seed = 0;        //< Random seed for reproducible tests

    //=== Runtime result members ===
    double timeA = 0.0;  //< Total execution time for methodA (milliseconds)
    double timeB = 0.0;  //< Total execution time for methodB (milliseconds)
    double ratio = 0.0;  //< Performance ratio (timeA/timeB)

    /**
     * @brief Constructor that reads command line parameters
     *
     * Initializes configuration parameters from command line with unified
     * prefixes (timer-*). Derived classes can override these values before
     * calling run() if needed.
     */
    RelativeTimer()
    {
        // Bind command line arguments with unified prefix
        BIND_ARGV(loop, "timer-loop");
        BIND_ARGV(batch, "timer-batch");
        BIND_ARGV(size, "timer-size");
        BIND_ARGV(seed, "timer-seed");
    }

    //=== Pure virtual methods for derived classes ===
    
    /**
     * @brief First method to compare (must be implemented by derived class)
     */
    virtual void methodA() = 0;

    /**
     * @brief Second method to compare (must be implemented by derived class)
     */
    virtual void methodB() = 0;

    /**
     * @brief Verification method for functional correctness
     *
     * This method should be overridden by derived classes to verify that
     * methodA and methodB produce equivalent results. The default
     * implementation returns true (no verification).
     *
     * @return bool True if methods are functionally equivalent, false otherwise
     */
    virtual bool methodVerify() { return true; }

    //=== Public interface methods ===

    /**
     * @brief Run relative performance test with verification
     *
     * Executes methodA and methodB alternately in batches for the configured
     * number of loops. Stores results in member variables (timeA, timeB, ratio).
     * Includes a verification phase to ensure functional correctness before
     * performance measurement.
     *
     * @return double Performance ratio (timeA/timeB). Returns NaN if verification fails
     */
    double run()
    {
        // Handle special values for safety
        if (batch <= 0) batch = 1;
        if (loop <= 0) loop = 1;

        int inner_loop = loop / batch;
        if (inner_loop <= 0) inner_loop = 1;

        // Verification phase - check functional correctness before performance testing
        if (!methodVerify())
        {
            // Return NaN to indicate verification failure
            timeA = 0.0;
            timeB = 0.0;
            ratio = std::numeric_limits<double>::quiet_NaN();
            return ratio;
        }

        // Warm up
        methodA();
        methodB();

        // Time accumulation
        timeA = 0.0;
        timeB = 0.0;

        // Run in alternating batches
        for (int i = 0; i < batch; ++i)
        {
            // Time methodA
            auto startA = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < inner_loop; ++j)
            {
                methodA();
            }
            auto endA = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedA = endA - startA;
            timeA += elapsedA.count();

            // Time methodB
            auto startB = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < inner_loop; ++j)
            {
                methodB();
            }
            auto endB = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedB = endB - startB;
            timeB += elapsedB.count();
        }

        // Convert to milliseconds
        timeA *= 1000.0;
        timeB *= 1000.0;

        // Calculate ratio
        if (timeB == 0.0)
        {
            // Avoid division by zero
            ratio = (timeA == 0.0) ? 1.0 : 1000.0; // Large value indicating A is much slower
        }
        else
        {
            ratio = timeA / timeB;
        }

        return ratio;
    }

    /**
     * @brief Run performance test and print results
     *
     * Executes the performance test and prints detailed results including
     * timing information and performance comparison analysis.
     *
     * @return double Performance ratio (timeA/timeB)
     */
    double runAndPrint()
    {
        double result_ratio = run();

        // Set default names if not provided
        if (name.empty()) name = "Relative Performance Test";
        if (labelA.empty()) labelA = "Method A";
        if (labelB.empty()) labelB = "Method B";

        DESC("=== %s ===", name.c_str());
        DESC("Loops: %d, Batches: %d, Size: %d, Seed: %d", loop, batch, size, seed);
        DESC("Performance ratio (%s/%s) %g ms / %g ms = %g", labelA.c_str(), labelB.c_str(), timeA, timeB, ratio);

        if (std::isnan(ratio))
        {
            DESC("Performance test failed: verification returned false");
        }
        else if (ratio < 0.95)
        {
            DESC("%s is %g%% faster", labelA.c_str(), (1.0 / ratio - 1.0) * 100);
        }
        else if (ratio > 1.05)
        {
            DESC("%s is %g%% faster", labelB.c_str(), (ratio - 1.0) * 100);
        }
        else
        {
            DESC("Performance is approximately equal");
        }

        return result_ratio;
    }

    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~RelativeTimer() = default;
};

} // namespace tast

//===================================================================
// COUT_TIMER Macro Extension
//===================================================================

namespace tast
{
namespace macro
{

/// @brief Implement single parameter COUT_TIMER(timer)
/// Calls timer.runAndPrint() method, then asserts return value is not NaN
inline bool cout_timer(const tast::CLocation& location, const char* expr, tast::RelativeTimer& timer)
{
    double ratio = timer.runAndPrint();
    bool pass = !std::isnan(ratio);
    // expr =~? cout(valExpr, valExpect, bPass)
    return tast::CStatement(location, expr).cout(ratio, "not nan", pass);
}

/// @brief Implement dual parameter COUT_TIMER(timer, max_ratio)
/// Calls timer.run() method, then asserts return value is not NaN and less than max_ratio
inline bool cout_timer(const tast::CLocation& location, const char* expr, tast::RelativeTimer& timer, double max_ratio)
{
    double ratio = timer.run();
    bool pass = !std::isnan(ratio);
    if (pass)
    {
        pass = ratio < max_ratio;
    }
    // expr =~? cout(valExpr, valExpect, bPass)
    return tast::CStatement(location, expr).cout(ratio, max_ratio, pass);
}

} // namespace macro
} // namespace tast

/// @brief COUT_TIMER Macro Definition
/// Single parameter version: COUT_TIMER(timer) - Print detailed results and assert return value is not NaN
/// Dual parameter version: COUT_TIMER(timer, max_ratio) - Assert return value is not NaN and less than max_ratio
#define COUT_TIMER(timer, ...) tast::macro::cout_timer(SRC_LOCATION, #timer, timer, ## __VA_ARGS__)

#endif // PERF_RELATIVE_TIMER_HPP_
