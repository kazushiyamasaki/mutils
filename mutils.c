/*
 * mutils.c -- implementation part of a compact and handy collection of C utilities
 * version 0.9.0, Apr. 18, 2026
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


#ifdef THREAD_LOCAL
	/* errno 記録時に関数名を記録する */
	THREAD_LOCAL const char* mutils_errfunc = PTR_NULL;

	/* 直前に実行した getsn() で切り捨てた文字数のカウント変数 */
	THREAD_LOCAL size_t getsn_truncated_len;
#else
	/* errno 記録時に関数名を記録する */
	const char* mutils_errfunc = PTR_NULL;  /* 非スレッドセーフ */

	/* 直前に実行した getsn() で切り捨てた文字数のカウント変数 */
	size_t getsn_truncated_len;  /* 非スレッドセーフ */
#endif


size_t mutils_strnlen (const char* string, size_t max_bytes) {
	if (string == PTR_NULL) {
		mutils_errfunc = "mutils_strnlen";
		errno = EINVAL;
		return 0;
	}

	if (max_bytes == 0) return 0;

	/* stringの中に '\0' があるかをチェック */
	const void* null_pos = memchr(string, '\0', max_bytes);

	return null_pos ? (size_t)((const char*)null_pos - string) : max_bytes;
}


char* mutils_strndup (const char* string, size_t max_bytes) {
	if (string == PTR_NULL || max_bytes == SIZE_MAX) {
		mutils_errfunc = "mutils_strndup";
		errno = EINVAL;
		return PTR_NULL;
	}

	size_t len = mutils_strnlen(string, max_bytes);

	char* dup = malloc(len + 1);
	if (UNLIKELY(dup == PTR_NULL)) {
		mutils_errfunc = "mutils_strndup";
		errno = ENOMEM;
		return PTR_NULL;
	}

	memcpy(dup, string, len);
	dup[len] = '\0';
	return dup;
}


char* mutils_getsn (char* buf, int cnt) {
	getsn_truncated_len = 0;

	if (buf == PTR_NULL || cnt <= 0) {
		mutils_errfunc = "mutils_getsn";
		errno = EINVAL;
		return PTR_NULL;
	}

	if (fgets(buf, cnt, stdin)) {
		size_t len = strlen(buf);

		/* 改行が末尾にあれば取り除く */
		if (LIKELY(len > 0)) {
			/* \r\n を改行に用いる環境向け */
			if (len > 1 && buf[len - 2] == '\r' && buf[len - 1] == '\n') {
				buf[len - 2] = '\0';

			/* \n または \r を改行に用いる環境向け */
			} else if (buf[len - 1] == '\n' || buf[len - 1] == '\r') {
				buf[len - 1] = '\0';

			/* 改行もEOFもなかった = バッファに収まりきらなかった */
			} else if (!feof(stdin)) {
				int ch;

				/* 標準入力の残りをクリア */
				while ((ch = getchar()) != '\n' && ch != EOF)
					getsn_truncated_len++;  /* 副作用の明示のためこちらに記述 */
			}
		}

		return buf;
	}
	return PTR_NULL;
}
