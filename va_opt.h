/* SPDX-License-Identifier: Apache-2.0 AND BSL-1.0 AND CC0-1.0 */

#ifndef VA_OPT_H
#define VA_OPT_H
#define VA_OPT_H_VERSION 20251227

/*
A VA_OPT polyfill by Robert Rapier.
Code sections are licensed as CC0 1.0 Universal unless specified otherwise.
To view a copy of this license,
visit https://creativecommons.org/publicdomain/zero/1.0/

EXAMPLE USAGE:

#define LOG(fmt, ...) \
    printf(fmt VA_OPT((__VA_ARGS__), ,) __VA_ARGS__)

When __VA_ARGS__ is empty: printf(fmt)
When __VA_ARGS__ is "x": printf(fmt , x)

Equivalent to:

#define LOG(fmt, ...) \
    printf(fmt __VA_OPT__(,) __VA_ARGS__)

RUN TESTS:
    cc -x c -DTEST_VA_OPT va_opt.h -o va_opt_test && ./va_opt_test
RUN TESTS WITH VA_OPT POLYFILL:
  cc -x c -DVA_OPT_USE_C99 -DTEST_VA_OPT va_opt.h -o va_opt_test &&
    ./va_opt_test
RUN TESTS WITH GNU FALLBACK:
  cc -x c -DVA_OPT_USE_GNU -DTEST_VA_OPT va_opt.h -o va_opt_test &&
    ./va_opt_test
RUN TESTS WITH MSVC FALLBACK:
  Define VA_OPT_USE_MSVC and TEST_VA_OPT
RUN TESTS WITH NATIVE __VA_OPT__ PATH:
  cc -x c -DVA_OPT_USE_NATIVE -DTEST_VA_OPT va_opt.h -o va_opt_test &&
    ./va_opt_test

FEATURE DETECTION:
  Native __VA_OPT__ support is detected automatically and used if available,
  bypassing tradeoffs entirely.
  GNU and MSVC comma elision extensions are detected and used if __VA_OPT__
  is not available. This detection is imperfect, for example, a compiler may
  support GNU style comma elision but not define __GNUC__, in which case
  the C99 polyfill will be used instead unless it's caught by additional checks,
  which is the case for ICC, but others may be missed.

CHOOSE IMPLEMENTATIONS:
  Define exactly one of the following to override auto-selection:
  - VA_OPT_USE_NATIVE
  - VA_OPT_USE_GNU
  - VA_OPT_USE_MSVC
  - VA_OPT_USE_C99

IMPLEMENTATION NOTES:
    Native __VA_OPT__ support or comma elision compiler extensions are detected 
    automatically and used if available, bypassing tradeoffs entirely. Otherwise
    a portable C99 polyfill is used. The foundation of this polyfill is the
    ISEMPTY macro from Jens Gustedt, extended to support unlimited arguments.
    VA_ISEMPTY and VA_OPT, in pure C99 mode, will not compile if the first
    argument is a macro that requires 2 or more non-variadic
    parameters. E.g.:

    #define MAC2(x,y) whatever
    VA_OPT((MAC2), ... ) // Will not compile

    Any modern compiler with a comma elision extension OR __VA_OPT__ support
    will not have any edge cases like this.
*/

/* Implementation identifiers */
#define NTRNLVA_IMPL_NATIVE 1
#define NTRNLVA_IMPL_GNU    2
#define NTRNLVA_IMPL_MSVC   3
#define NTRNLVA_IMPL_C99    4

/* Detect __VA_OPT__ support */
#define NTRNLVA_EXPAND(...) __VA_ARGS__
#define NTRNLVA_SEL3_I(a, b, c, ...) c
#define NTRNLVA_SEL3(...) NTRNLVA_EXPAND(NTRNLVA_SEL3_I(__VA_ARGS__))
#define NTRNLVA_OPT_SUPPORTED_I(...) NTRNLVA_SEL3(__VA_OPT__(, ), 1, 0, )
#ifndef VA_OPT_SUPPORTED
#ifdef __PCC__
#define VA_OPT_SUPPORTED 0
#endif
#endif
#ifndef VA_OPT_SUPPORTED
#define VA_OPT_SUPPORTED NTRNLVA_OPT_SUPPORTED_I(?)
#endif

/* Select implementation */
#ifdef VA_OPT_USE_NATIVE
    #define NTRNLVA_IMPL NTRNLVA_IMPL_NATIVE
#elif defined(VA_OPT_USE_GNU)
    #define NTRNLVA_IMPL NTRNLVA_IMPL_GNU
#elif defined(VA_OPT_USE_MSVC)
    #define NTRNLVA_IMPL NTRNLVA_IMPL_MSVC
#elif defined(VA_OPT_USE_C99)
    #define NTRNLVA_IMPL NTRNLVA_IMPL_C99
#endif

#ifndef NTRNLVA_IMPL 
/* Auto-select implementation */
#if VA_OPT_SUPPORTED
    #define NTRNLVA_IMPL NTRNLVA_IMPL_NATIVE

#elif defined(_MSC_VER) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
    #ifndef _MSVC_TRADITIONAL
        #define _MSVC_TRADITIONAL 1
    #endif
    
    #if _MSVC_TRADITIONAL
        #define NTRNLVA_IMPL NTRNLVA_IMPL_MSVC
    #else
        #define NTRNLVA_IMPL NTRNLVA_IMPL_GNU
    #endif

#elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
    #define NTRNLVA_IMPL NTRNLVA_IMPL_GNU

#elif defined(__INTEL_COMPILER)
    #define NTRNLVA_IMPL NTRNLVA_IMPL_GNU

#else
    #define NTRNLVA_IMPL NTRNLVA_IMPL_C99

#endif /* Auto-select implementation */
#endif /* NTRNLVA_IMPL */

#ifdef _MSVC_TRADITIONAL
    #define NTRNLVA_MSVC_TRADITIONAL _MSVC_TRADITIONAL
#else
    #define NTRNLVA_MSVC_TRADITIONAL 0
#endif

#undef NTRNLVA_SEL3_I
#undef NTRNLVA_SEL3

#define NTRNLVA_SEL1_I(a, ...) a
#define NTRNLVA_SEL2_I(a, b, ...) b
#define NTRNLVA_SEL3_I(a, b, c, ...) c
#if NTRNLVA_MSVC_TRADITIONAL
    #define NTRNLVA_SEL1(...) NTRNLVA_EXPAND(NTRNLVA_SEL1_I(__VA_ARGS__))
    #define NTRNLVA_SEL2(...) NTRNLVA_EXPAND(NTRNLVA_SEL2_I(__VA_ARGS__))
    #define NTRNLVA_SEL3(...) NTRNLVA_EXPAND(NTRNLVA_SEL3_I(__VA_ARGS__))
#else
    #define NTRNLVA_SEL1(...) NTRNLVA_SEL1_I(__VA_ARGS__)
    #define NTRNLVA_SEL2(...) NTRNLVA_SEL2_I(__VA_ARGS__)
    #define NTRNLVA_SEL3(...) NTRNLVA_SEL3_I(__VA_ARGS__)
#endif /* NTRNLVA_MSVC_TRADITIONAL check */

/* START OF BOOST LICENSED CLOAK.H CODE ***************************************/
/*=============================================================================
    Copyright (c) 2015 Paul Fultz II
    cloak.h
    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
/* SOURCE:                                                                    */
/* https://github.com/pfultz2/Cloak/blob/master/cloak.h                       */

#define NTRNLVA_EMPTY(...)

#define NTRNLVA_CAT(a, ...) NTRNLVA_PRIMITIVE_CAT(a, __VA_ARGS__)
#define NTRNLVA_PRIMITIVE_CAT(a, ...) a##__VA_ARGS__

#define NTRNLVA_AND(x) NTRNLVA_PRIMITIVE_CAT(NTRNLVA_AND_, x)
#define NTRNLVA_AND_0(y) 0
#define NTRNLVA_AND_1(y) y

#define NTRNLVA_CHECK_N(x, n, ...) n
#if NTRNLVA_MSVC_TRADITIONAL
#define NTRNLVA_CHECK(...) NTRNLVA_EXPAND(NTRNLVA_CHECK_N(__VA_ARGS__, 0, ))
#else
#define NTRNLVA_CHECK(...) NTRNLVA_CHECK_N(__VA_ARGS__, 0, )
#endif
#define NTRNLVA_PROBE(x) x, 1,

#define NTRNLVA_IS_PAREN(x) NTRNLVA_CHECK(NTRNLVA_IS_PAREN_PROBE x)
#define NTRNLVA_IS_PAREN_PROBE(...) NTRNLVA_PROBE(~)

#define NTRNLVA_COMPL(b) NTRNLVA_PRIMITIVE_CAT(NTRNLVA_COMPL_, b)
#define NTRNLVA_COMPL_0 1
#define NTRNLVA_COMPL_1 0

/* END OF BOOST LICENSED CLOAK.H CODE *****************************************/


#define NTRNLVA_UP_I(...) __VA_ARGS__
#define NTRNLVA_UP(x) NTRNLVA_UP_I x
#define NTRNLVA_SENTINEL_ ()()()
#define NTRNLVA_IS_SENTINEL2(...) ()
#define NTRNLVA_IS_SENTINEL1(...) NTRNLVA_IS_SENTINEL2
#define NTRNLVA_IS_SENTINEL(...) NTRNLVA_IS_SENTINEL1
#define NTRNLVA_CHECK_SENTINEL(x)                                              \
  NTRNLVA_IS_PAREN(NTRNLVA_EXPAND(NTRNLVA_IS_SENTINEL x))

/* ISEMPTY IMPLEMENTATIONS */
#if NTRNLVA_IMPL == NTRNLVA_IMPL_NATIVE
    #define NTRNLVA_ISEMPTY_I0 1
    #define NTRNLVA_ISEMPTY_I01 0
    #define NTRNLVA_ISEMPTY_I(x) NTRNLVA_PRIMITIVE_CAT(NTRNLVA_ISEMPTY_I0, x)
    #define VA_ISEMPTY(...) NTRNLVA_ISEMPTY_I(__VA_OPT__(1))
    
    #define NTRNLVA_NOTEMPTY_I0 0
    #define NTRNLVA_NOTEMPTY_I01 1
    #define NTRNLVA_NOTEMPTY_I(x) NTRNLVA_PRIMITIVE_CAT(NTRNLVA_NOTEMPTY_I0, x)
    #define VA_NOTEMPTY(...) NTRNLVA_NOTEMPTY_I(__VA_OPT__(1))
#elif NTRNLVA_IMPL == NTRNLVA_IMPL_GNU
    #define NTRNLVA_ISEMPTY_I(_0, _1, _2, ...) NTRNLVA_CHECK_SENTINEL(_2)
    #define NTRNLVA_ISEMPTY_INDIRECT(...)                                                    \
        NTRNLVA_ISEMPTY_I(,##__VA_ARGS__, 0, NTRNLVA_SENTINEL_)
    #define VA_ISEMPTY(...) NTRNLVA_ISEMPTY_INDIRECT(__VA_ARGS__)
    #define VA_NOTEMPTY(...) NTRNLVA_COMPL(VA_ISEMPTY(__VA_ARGS__))
#elif NTRNLVA_IMPL == NTRNLVA_IMPL_MSVC
    #define NTRNLVA_ISEMPTY_I(_0, _1, _2, ...) NTRNLVA_CHECK_SENTINEL(_2)
    #define NTRNLVA_ISEMPTY_INDIRECT(...)                                                    \
        NTRNLVA_ISEMPTY_I NTRNLVA_EMPTY()(,__VA_ARGS__, 0, NTRNLVA_SENTINEL_)
    #define VA_ISEMPTY(...) NTRNLVA_ISEMPTY_INDIRECT(__VA_ARGS__)
    #define VA_NOTEMPTY(...) NTRNLVA_COMPL(VA_ISEMPTY(__VA_ARGS__))
#elif NTRNLVA_IMPL == NTRNLVA_IMPL_C99

    #define NTRNLVA_ISEMPTY_I(...)                                              \
      NTRNLVA_AND(NTRNLVA_ISEMPTY_BASE(NTRNLVA_SEL1(__VA_ARGS__)))             \
      (NTRNLVA_CHECK_SENTINEL(NTRNLVA_SEL2(__VA_ARGS__)))
    #define VA_ISEMPTY(...) NTRNLVA_ISEMPTY_I(__VA_ARGS__, NTRNLVA_SENTINEL_)
    #define VA_NOTEMPTY(...) NTRNLVA_COMPL(VA_ISEMPTY(__VA_ARGS__))

/* START OF APACHE LICENSED P99 CODE ******************************************/
/* (C) copyright  2010-2012 Jens Gustedt, INRIA, France                       */
/* (C) copyright  2012 William Morris                                         */
/*                                                                            */
/* This section is free software; it is part of the P99 project.              */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License");            */
/* you may not use this file except in compliance with the License.           */
/* You may obtain a copy of the License at                                    */
/*                                                                            */
/*     http://www.apache.org/licenses/LICENSE-2.0                             */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/*                                                                            */
/* SOURCES:                                                                   */
/* https://gitlab.inria.fr/gustedt/p99/-/blob/master/p99/p99_args.h#L75       */
/* https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/     */

/* Modified HAS_COMMA with SENTINEL instead of 1 works on unlimited args      */
#if NTRNLVA_MSVC_TRADITIONAL
  #define NTRNLVA_HAS_COMMA(...)                                                 \
    NTRNLVA_CHECK_SENTINEL(NTRNLVA_EXPAND( \
      NTRNLVA_SEL3_I(__VA_ARGS__, NTRNLVA_SENTINEL_, 0) \
    ))
#else
  #define NTRNLVA_HAS_COMMA(...)                                                 \
    NTRNLVA_CHECK_SENTINEL(NTRNLVA_SEL3_I(__VA_ARGS__, NTRNLVA_SENTINEL_, 0))
#endif /* NTRNLVA_MSVC_TRADITIONAL check */
#define NTRNLVA_TRIGGER_PARENTHESIS_(...) ,

#define NTRNLVA_ISEMPTY_BASE(...)                                              \
  NTRNLVA_ISEMPTY_BASE_I(/* test if there is just one argument, eventually an  \
                            empty one */                                       \
                         NTRNLVA_HAS_COMMA(                                    \
                             __VA_ARGS__), /* test if                          \
                                              NTRNLVA_TRIGGER_PARENTHESIS_     \
                                              together with the argument adds  \
                                              a comma */                       \
                         NTRNLVA_HAS_COMMA(                                    \
                             NTRNLVA_TRIGGER_PARENTHESIS_                      \
                                 __VA_ARGS__), /* test if the argument         \
                                                  together with a parenthesis  \
                                                  adds a comma */              \
                         NTRNLVA_HAS_COMMA(__VA_ARGS__(                        \
                             /*empty*/)), /* test if placing it between        \
                                             NTRNLVA_TRIGGER_PARENTHESIS_ and  \
                                             the parenthesis adds a comma */   \
                         NTRNLVA_HAS_COMMA(                                    \
                             NTRNLVA_TRIGGER_PARENTHESIS_ __VA_ARGS__(         \
                                 /*empty*/)))

#define NTRNLVA_PASTE5(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define NTRNLVA_ISEMPTY_BASE_I(_0, _1, _2, _3)                                 \
  NTRNLVA_HAS_COMMA(NTRNLVA_PASTE5(NTRNLVA_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define NTRNLVA_IS_EMPTY_CASE_0001 ,

#endif /* End of VA_ISEMPTY implementations */

#ifdef TEST_VA_OPT
#define EATER0(...)
#define EATER1(...) ,
#define EATER2(...) (/*empty*/)
#define EATER3(...) (/*empty*/),
#define EATER4(...) EATER1
#define EATER5(...) EATER2
#define MAC0() ()
#define MAC1(x) ()
#define MACV(...) ()
#define MACMANYPLUS(...) +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +, +
/* A macro with 2+ non-variadic parameters will not compile when it's the first
argument in the list passed to VA_OPT or VA_ISEMPTY */
#define MAC2(x, y) whatever

/* Expected, test args */
#define SINGLE_TEST_CASES                                                      \
  X(1, )                                                                       \
  X(1, /*comment*/)                                                            \
  X(0, a)                                                                      \
  X(0, (void))                                                                 \
  X(0, NTRNLVA_TRIGGER_PARENTHESIS_)                                           \
  X(0, EATER0)                                                                 \
  X(0, EATER1)                                                                 \
  X(0, EATER2)                                                                 \
  X(0, EATER3)                                                                 \
  X(0, EATER4)                                                                 \
  X(0, MAC0)                                                                   \
  X(0, MAC1)                                                                   \
  X(0, MACV)                                                                   \
  X(0, MACMANYPLUS)                                                            \
  X(0, "unpastable")

#define VARIADIC_TEST_CASES                                                    \
  X(0, +, "many", "unpastable", "tokens", +, +, +, +, +, +, +, +, +, +, +, +)  \
  X(0, a, b)                                                                   \
  X(0, a, b, c)                                                                \
  X(0, a, b, c, d)                                                             \
  X(0, a, b, c, d, e)                                                          \
  X(0, (void), b, c, d)

#define ISEMPTY_TEST_CASES                                                     \
  SINGLE_TEST_CASES                                                            \
  VARIADIC_TEST_CASES

#endif /* TEST_VA_OPT */
/* END OF APACHE LICENSED P99 CODE ********************************************/

/* VA_OPT IMPLEMENTATIONS */
#define NTRNLVA_OPT_IMPL(opt, ...)                                             \
  NTRNLVA_CAT(NTRNLVA_OPT_IMPL_, opt)(__VA_ARGS__)
#define NTRNLVA_OPT_IMPL_0(...) __VA_ARGS__
#define NTRNLVA_OPT_IMPL_1(...)

#if NTRNLVA_IMPL != NTRNLVA_IMPL_NATIVE
    #define VA_OPT(args, ...)                                                  \
      NTRNLVA_OPT_IMPL(VA_ISEMPTY(NTRNLVA_UP(args)), __VA_ARGS__)
#else
    #define NTRNLVA_OPT_I(opt, ...) __VA_OPT__(NTRNLVA_UP(opt))
    #define VA_OPT(args, ...) NTRNLVA_OPT_I((__VA_ARGS__), NTRNLVA_UP(args))
#endif /* NTRNLVA_IMPL check */
#define VA_NOPT(args, ...)                                                     \
  NTRNLVA_OPT_IMPL(VA_NOTEMPTY(NTRNLVA_UP(args)), __VA_ARGS__)

#ifdef TEST_VA_OPT
#include <stdio.h>

#define EXPECT(test, expected, fail_msg_fmt, ...)                              \
  do {                                                                         \
    int result = test;                                                         \
    if (result != expected) {                                                  \
      printf("Test failed: " fail_msg_fmt, #__VA_ARGS__);                      \
      printf(" (expected %d, got %d)\n", expected, result);                    \
      failed++;                                                                \
    } else {                                                                   \
      passed++;                                                                \
    }                                                                          \
  } while (0)

int main() {
#if NTRNLVA_IMPL == NTRNLVA_IMPL_NATIVE
  printf("Testing with native __VA_OPT__ support\n");
#elif NTRNLVA_IMPL == NTRNLVA_IMPL_GNU
  printf("Testing with GNU style comma elision fallback\n");
#elif NTRNLVA_IMPL == NTRNLVA_IMPL_MSVC
  printf("Testing with MSVC style comma elision fallback\n");
#else
  printf("Testing with C99 VA_OPT polyfill\n");
#endif
  int passed = 0;
  int failed = 0;
#define X(expected, ...)                                                       \
  EXPECT(VA_ISEMPTY(__VA_ARGS__), expected, "VA_ISEMPTY failed for args %s",   \
         __VA_ARGS__);
  ISEMPTY_TEST_CASES
#undef X
#define X(expected, ...)                                                       \
  EXPECT(!VA_NOTEMPTY(__VA_ARGS__), expected,                                  \
         "VA_NOTEMPTY failed for args %s", __VA_ARGS__);
  ISEMPTY_TEST_CASES
#undef X
#define X(expected, ...)                                                       \
  EXPECT(1 VA_OPT((__VA_ARGS__), -1), expected, "VA_OPT failed for args %s",   \
         __VA_ARGS__);
  ISEMPTY_TEST_CASES
#undef X
#define X(expected, ...)                                                       \
  EXPECT(0 VA_NOPT((__VA_ARGS__), +1), expected, "VA_NOPT failed for args %s", \
         __VA_ARGS__);
  ISEMPTY_TEST_CASES
#undef X
  printf("Tests passed: %d\n", passed);
  printf("Tests failed: %d\n", failed);
  if (failed == 0) {
    printf("All tests passed!\n");
  }
  return !!failed;
}
#endif /* TEST_VA_OPT */

#endif /* VA_OPT_H */
