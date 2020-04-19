/*******************************************************************************
 * Name:	oprinta.c (part of libprinta, the print-aligned C library)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "printa.h"

static int _vprintf(struct options *opts, const char *format, va_list ap)
{
    int nbytes;

    if(opts == NULL || (opts->stream == NULL && opts->_p_str == NULL))
	nbytes = vprintf(format, ap);
    else if(opts->stream)
	nbytes = vfprintf(opts->stream, format, ap);
    else{
	nbytes = vsprintf(opts->_p_str, format, ap);
	opts->_p_str += nbytes;
    }

    return nbytes;
}

static int _printf(struct options *opts, const char *format, ...)
{
    va_list ap;
    int nbytes;

    va_start(ap, format);
    nbytes = _vprintf(opts, format, ap);
    va_end(ap);

    return nbytes;
}

/* adjust the width of string specifiers to the display space intead of the
 * number of bytes for wide characters and _printf them using the adjusted
 * display width
 *
 * compare
 *	printf("%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
    가나|
-----------
 * and
 *	oprinta(&opts, "%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
      가나|
-----------
 */
int oprinta(struct options *opts, const char *format, va_list ap)
{
    char *fmt, *asis, *p, spec[10];
    int nbytes = 0;

    /* make a copy so we can temporarily change the format string */
    p = asis = fmt = (char *)malloc(strlen(format) + 1);
    strcpy(fmt, format);

    while(*p){
	if(*p == '%'){
	    char *q = p, *p_spec = spec;

	    /* print the string before this specifier */
	    *p = 0;
	    nbytes += _printf(opts, asis);
	    *p = '%';

	    /* skip % */
	    while(*++q){
		char *c = CONVERSIONS - 1;

		while(*++c && *q != *c);
		if(*c){
		    char tmp;
		    /* found a conversion specifier */
		    if(*c == 's'){
			int width = -1, prec = -1, use_printf = 1;
			char *p_tmp;
			va_list ap_copy;

			/* save this ap and use _vprintf() for non-wide
			 * characters */
			va_copy(ap_copy, ap);

			/* if string */
			*p_spec = 0;
			p_spec = spec;
			if(*p_spec == '-')
			    p_spec++;
			if(*p_spec == '*'){
			    width = va_arg(ap, int);
			    p_spec++;
			}else if(*p_spec >= '0' && *p_spec <= '9'){
			    p_tmp = p_spec;
			    while(*p_spec >= '0' && *p_spec <= '9')
				p_spec++;
			    tmp = *p_spec;
			    *p_spec = 0;
			    width = atoi(p_tmp);
			    *p_spec = tmp;
			}
			if(*p_spec == '.'){
			    p_spec++;
			    if(*p_spec == '*'){
				prec = va_arg(ap, int);
				p_spec++;
			    }else if(*p_spec >= '0' && *p_spec <= '9'){
				p_tmp = p_spec;
				while(*p_spec >= '0' && *p_spec <= '9')
				    p_spec++;
				tmp = *p_spec;
				*p_spec = 0;
				prec = atoi(p_tmp);
				*p_spec = tmp;
			    }
			}
			if(*p_spec){
			    /* really? */
			    va_end(ap_copy);
			    return -1;
			}

			if(width > 0){
			    char *s = va_arg(ap, char *);
			    int wcount = wide_count(s);
			    if(wcount){
				width += wcount;
				prec += prec > 0 ? wcount : 0;
				p_spec = spec;
				p_spec += sprintf(p_spec, "%%%s%d",
					spec[0] == '-' ? "-" : "", width);
				if(prec >= 0)
				    p_spec += sprintf(p_spec, ".%d", prec);
				*p_spec++ = 's';
				*p_spec = 0;
				nbytes += _printf(opts, spec, s);
				use_printf = 0;
			    }
			}
			if(use_printf){
			    tmp = *(q + 1);
			    *(q + 1) = 0;
			    nbytes += _vprintf(opts, p, ap_copy);
			    *(q + 1) = tmp;
			}

			va_end(ap_copy);
		    }else{
			/* else use _vprintf() */
			tmp = *(q + 1);
			*(q + 1) = 0;
			nbytes += _vprintf(opts, p, ap);
			*(q + 1) = tmp;
		    }
		    break;
		}else
		    *p_spec++ = *q;
	    }
	    p = asis = ++q;
	}
	p++;
    }

    /* print the remaining string */
    *p = 0;
    nbytes += _printf(opts, asis);
    *p = '%';

    return nbytes;
}