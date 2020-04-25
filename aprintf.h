/*******************************************************************************
 * Name:	aprintf.h (part of libaprintf, the aligned printf C library)
 * Repository:	https://github.com/HuidaeCho/libaprintf
 * Author:	Huidae Cho
 * Since:	April 18, 2020
 *
 * Copyright (C) 2020, Huidae Cho <https://idea.isnew.info>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef _APRINTF_H_
#define _APRINTF_H_

#include <stdio.h>
#include <stdarg.h>

struct options
{
    FILE *stream;
    char *str, *_str;
    size_t size, _size;
};

extern int count_wide_chars(const char *str);
extern int count_wide_chars_in_cols(const char *str, int ncols, int *nbytes);
extern int oaprintf(struct options *opts, const char *format, va_list ap);

extern int aprintf(const char *format, ...);
extern int faprintf(FILE *stream, const char *format, ...);
extern int saprintf(char *str, const char *format, ...);
extern int snaprintf(char *str, size_t size, const char *format, ...);

extern int vaprintf(const char *format, va_list ap);
extern int vfaprintf(FILE *stream, const char *format, va_list ap);
extern int vsaprintf(char *str, const char *format, va_list ap);
extern int vsnaprintf(char *str, size_t size, const char *format, va_list ap);

#endif
