//
// Created by YAD on 17.08.2020.
//

#ifndef YAIMP_ASSERT_HPP
#define YAIMP_ASSERT_HPP

#include <stdexcept>

#if !defined(VB_ASSERT_IGNORE) &&!defined(VB_ASSERT_THROW) && !defined(VB_ASSERT_TERMINATE) && !defined(NDEBUG)
#define VB_ASSERT_THROW
#endif

#if defined(__clang__) || defined(__GNUC__)
#define VB_LIKELY(x) __builtin_expect(!!(x), 1)
#define VB_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define VB_LIKELY(x) (!!(x))
#define VB_UNLIKELY(x) (!!(x))
#endif

#ifdef _MSC_VER
#define VB_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__)
#define VB_ASSUME(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#else
#define VB_ASSUME(cond) static_cast<void>((cond) ? 0 : 0)
#endif
#define VB_STRING0(x) #x
#define VB_STRING(x) VB_STRING0(x)

#ifdef VB_ASSERT_THROW
#define AssertMsg(cond,msg) VB_LIKELY(cond) ? static_cast<void>(0) : throw std::logic_error("Assertion failed: " msg " in " __FILE__ ":" VB_STRING(__LINE__))
#define AssertMsgSlow(cond,msg) Assert(cond,msg)
#elif defined(VB_ASSERT_TERMINATE)
#define AssertMsg(cond,msg) std::terminate()
#define AssertMsgSlow(cond,msg) Assert(cond,msg)
#else
#define AssertMsg(cond,msg) VB_ASSUME(cond)
#define AssertMsgSlow(cond,msg) static_cast<void>(0)
#endif
#define Assert(cond) AssertMsg(cond,#cond)
#define AssertSlow(cond) Assert(cond)
#endif //YAIMP_ASSERT_HPP
