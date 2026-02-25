/*
 * mutils_macros.h -- a header file defining macros used in mutils.h
 * version 0.9.0, Feb. 25, 2026
 *
 * License: zlib License
 *
 * Copyright (c) 2026 Kazushi Yamasaki
 *
 * This software is provided ‘as-is’, without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */

#pragma once

#ifndef MUTILS_MACROS_H
#define MUTILS_MACROS_H


#ifndef MUTILS_CPP_C_BEGIN
	#ifdef __cplusplus  /* C++ */
		#define MUTILS_CPP_C_BEGIN extern "C" {
		#define MUTILS_CPP_C_END }
	#else               /* not C++ */
		#define MUTILS_CPP_C_BEGIN
		#define MUTILS_CPP_C_END
	#endif
#endif


#ifndef THREAD_LOCAL
	/* C++11 or later */
	#if defined(__cplusplus) && (__cplusplus >= 201103L)
		#define THREAD_LOCAL thread_local

	/* GCC or Clang */
	#elif defined(__GNUC__)
		#define THREAD_LOCAL __thread

	/* MSVC */
	#elif defined(_MSC_VER)
		#define THREAD_LOCAL __declspec(thread)

	/* C (other compiler) */
	#elif defined(__STDC_VERSION__) && !defined(__cplusplus)
		/* C23 or later */
		#if __STDC_VERSION__ >= 202311L
			#define THREAD_LOCAL thread_local

		/* C11 or later (support optional standard threads) */
		#elif (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
			#define THREAD_LOCAL _Thread_local
		#endif
	#endif
#endif  /* THREAD_LOCAL */


#ifndef LIKELY
	#ifdef __GNUC__
		#ifndef __clang__
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wunused-macros"
		#endif

		#define LIKELY(x)   __builtin_expect(!!(x), 1)
		#define UNLIKELY(x) __builtin_expect(!!(x), 0)
	
		#ifndef __clang__
			#pragma GCC diagnostic pop  // -Wunused-macros
		#endif
	#else
		#define LIKELY(x)   (x)
		#define UNLIKELY(x) (x)
	#endif
#endif


#if !defined(__cplusplus) && (!defined (__STDC_VERSION__) || (__STDC_VERSION__ < 199901L))
#error "This program requires C99 or higher."
#endif


#if defined(__cplusplus) && (__cplusplus < 201103L)
#error "This program requires C++11 or higher."
#endif


#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L))
	#if defined(__has_include)
		#if __has_include(<stdbit.h>)
			#include <stdbit.h>
			#define _MUTILS_HAVE_STDBIT_H
		#endif
	#endif
#endif


#include <stdint.h>


#ifdef __GNUC__
	#include <limits.h>
	#if (SIZE_MAX == ULLONG_MAX)
		#define _mutils_gcc_clz_sizet(n) __builtin_clzll((unsigned long long)(n))

	// Use #elif instead of #if because ULLONG_MAX might equal ULONG_MAX.
	#elif (SIZE_MAX == ULONG_MAX)
		#define _mutils_gcc_clz_sizet(n) __builtin_clzl((unsigned long)(n))

	#elif (SIZE_MAX == UINT_MAX)
		#define _mutils_gcc_clz_sizet(n) __builtin_clz((unsigned int)(n))
	#endif
#endif


#ifdef _MSC_VER
	#if (defined(_M_X64) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_ARM))
		#include <intrin.h>
	#endif

	#if (defined(_M_X64) || defined(_M_ARM64))
		#define _MUTILS_MSVC_BitScanReverse_sizet(index, mask) _BitScanReverse64((index), (unsigned __int64)(mask))

	#elif (defined(_M_IX86) || defined(_M_ARM))
		#define _MUTILS_MSVC_BitScanReverse_sizet(index, mask) _BitScanReverse((index), (unsigned long)(mask))
	#endif
#endif


#endif
