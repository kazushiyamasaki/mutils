/*
 * mutils.c -- implementation part of a compact and handy collection of C utilities
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

#include "mutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "cver_compat/cver_compat.h"


#if !defined(__cplusplus) && (!defined (__STDC_VERSION__) || (__STDC_VERSION__ < 199901L))
	#error "This program requires C99 or higher."
#endif


#if defined (_WIN32) && (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0600))
	#error "This program requires Windows Vista or later. Define _WIN32_WINNT accordingly."
#endif


#undef malloc


/* errno 記録時に関数名を記録する */
#ifdef THREAD_LOCAL
	THREAD_LOCAL const char* mutils_errfunc = PTR_NULL;
#else
	const char* mutils_errfunc = PTR_NULL;  /* 非スレッドセーフ */
#endif


size_t mutils_strnlen (const char* string, size_t max_bytes) {
	if (string == PTR_NULL || max_bytes == 0) {
		mutils_errfunc = "mutils_strnlen";
		errno = EINVAL;
		return 0;
	}

	/* stringの中に '\0' があるかをチェック */
	const void* null_pos = memchr(string, '\0', max_bytes);

	return null_pos ? (size_t)((const char*)null_pos - string) : max_bytes;
}


char* mutils_strndup (const char* string, size_t max_bytes) {
	if (string == PTR_NULL || max_bytes == 0) {
		errno = EINVAL;
		mutils_errfunc = "mutils_strndup";
		return PTR_NULL;
	}

	size_t len = mutils_strnlen(string, max_bytes);
	char* dup = malloc(len + 1);
	if (UNLIKELY(dup == PTR_NULL)) {
		errno = ENOMEM;
		mutils_errfunc = "mutils_strndup";
		return PTR_NULL;
	}

	memcpy(dup, string, len);
	dup[len] = '\0';
	return dup;
}


char* mutils_getsn (char* buf, size_t cnt) {
	if (buf == PTR_NULL || cnt == 0) {
		mutils_errfunc = "mutils_getsn";
		errno = EINVAL;
		return PTR_NULL;
	}

	memset(buf, 0, cnt);

	if (fgets(buf, cnt, stdin)) {
		size_t len = mutils_strnlen(buf, cnt);

		/* 改行が末尾にあれば取り除く */
		if (len > 0 && buf[len - 1] == '\n') {
			buf[len - 1] = '\0';
		} else {
			/* 改行がなかった = バッファに収まりきらなかった可能性あり */
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF)
				; /* 標準入力の残りをクリア */
		}

		return buf;
	}
	return PTR_NULL;
}
