/*******************************************************************************
 * Name:	printa.h (part of libprinta, the print-aligned C library)
 * Repository:	https://github.com/HuidaeCho/libprinta
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

#ifndef _PRINTA_H_
#define _PRINTA_H_

#include <stdio.h>
#include <stdarg.h>

/* printf(3) man page */
#define CONVERSIONS "diouxXeEfFgGaAcsCSpnm%"

struct options
{
    FILE *stream;
    char *str, *_p_str;
};

extern int wide_count(const char *str);
extern int oprinta(struct options *opts, const char *format, va_list ap);
extern int printa(const char *format, ...);
extern int sprinta(char *str, const char *format, ...);
extern int fprinta(FILE *stream, const char *format, ...);

#endif
