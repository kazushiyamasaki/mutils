/*
 * mutils.h -- interface for a compact and handy collection of C utilities
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

#ifndef MUTILS_H
#define MUTILS_H


#include "mutils_macros.h"


MUTILS_CPP_C_BEGIN


#include <string.h>
#include <stdbool.h>


/*
 * mutils_errfunc is a global variable that stores the name of the function
 * where the most recent error occurred within this library.
 *
 * It is set to NULL when no error has occurred.
 * This variable is used to provide more informative error diagnostics,
 * especially in combination with errno.
 *
 * It is recommended to check this variable and errno after calling
 * any library function that may fail.
 */
#ifdef THREAD_LOCAL
	extern THREAD_LOCAL const char* mutils_errfunc;
#else
	extern const char* mutils_errfunc;
#endif


/*
 * mutils_strnlen
 * @param string: a pointer to the string to measure
 * @param max_bytes: the maximum number of bytes to check for number of characters
 * @return: the length of the string up to max_bytes, or max_bytes if no null terminator is found
 * @note: this function does not count the null terminator
 */
extern size_t mutils_strnlen (const char* string, size_t max_bytes);

/*
 * mutils_strndup
 * @param string: the string to duplicate
 * @param max_bytes: the maximum number of bytes to copy from the string
 * @return: a newly allocated string that is a duplicate of the input string, or NULL on failure
 * @note: the strings duplicated by this function must be freed with free()
 */
extern char* mutils_strndup (const char* string, size_t max_bytes);

/*
 * mutils_getsn
 * @param buf: a pointer to the buffer where the input will be stored
 * @param cnt: the size of the buffer
 * @return: a pointer to the buffer containing the input string, or NULL if an error occurs
 * @note: buf must be writable; non-writable formats like string literal cannot be used
 */
extern char* mutils_getsn (char* buf, size_t cnt);

/*
 * bool_text
 * @param flag: a boolean value (true or false)
 * @return: a pointer to a static string representing the boolean value, "true" if flag is true, "false" if flag is false
 * @note: returned pointer points to a static string, do not free
 */
static inline const char* bool_text (bool flag) {
	return flag ? "true" : "false";
}

/*
 * mutils_is_power_of_two
 * @param n: number to check
 * @return: true if n is a power of 2, false otherwise
 */
static inline bool mutils_is_power_of_two (size_t n) {
	return (n != 0) && ((n & (n - 1)) == 0);
}

/*
 * mutils_next_power_of_two
 * @param n: number to find the next power of two
 * @return: next power of two greater than or equal to n, returns 0 if the result would overflow size_t
 */
static inline size_t mutils_next_power_of_two (size_t n) {
	if (n == 0) return 1;
	if (n > (SIZE_MAX >> 1) + 1) return 0;

#if defined(_MUTILS_HAVE_STDBIT_H)  // C23
	return stdc_bit_ceil(n);

#elif defined(_mutils_gcc_clz_sizet)  // defined(__GNUC__)
	if (n == 1) return 1;

	size_t bits = sizeof(size_t) * CHAR_BIT;
	return (size_t)1 << (bits - _mutils_gcc_clz_sizet(n - 1));

#elif defined(_MUTILS_MSVC_BitScanReverse_sizet)  // defined(_MSC_VER)
	if (n == 1) return 1;

	unsigned long index;
	_MUTILS_MSVC_BitScanReverse_sizet(&index, n - 1);
	return (size_t)1 << (index + 1);

#else
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
#if SIZE_MAX > UINT32_MAX
	n |= n >> 32;
#endif
	return n + 1;

#endif
}


MUTILS_CPP_C_END


#endif
