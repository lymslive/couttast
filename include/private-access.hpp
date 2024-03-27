/**
 * @file private-access.hpp 
 * @author lymslive 
 * @date 2024-03-27
 * @brief Simple conditional control to access private member in unit test.
 * */
#pragma once
#ifndef PRIVATE_ACCESS_HPP__
#define PRIVATE_ACCESS_HPP__

#ifdef HAS_UNIT_TEST
#define private__    public
#define PRIVATE      public
#define protected__  public
#define PROTECTED    public
#else
#define private__    private
#define PRIVATE      private
#define protected__  protected
#define PROTECTED    protected
#endif

#endif /* end of include guard: PRIVATE_ACCESS_HPP__ */
