/*******************************************************************************
 * Name:	snprinta.c (part of libprinta, the print-aligned C library)
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

#include <stdarg.h>
#include "printa.h"

/* snprintf() wrapper for oprinta() */
int snprinta(char *str, size_t size, const char *format, ...)
{
    struct options opts;
    va_list ap;
    int nbytes;

    opts.stream = NULL;
    opts.str = opts._str = str;
    opts.size = opts._size = size;

    va_start(ap, format);
    nbytes = oprinta(&opts, format, ap);
    va_end(ap);

    return nbytes;
}