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

static int ovprintf(struct options *opts, const char *format, va_list ap)
{
    int nbytes;

    if(opts == NULL || (opts->stream == NULL && opts->_str == NULL))
	nbytes = vprintf(format, ap);
    else if(opts->stream)
	nbytes = vfprintf(opts->stream, format, ap);
    else{
	if((long int)opts->size >= 0){
	    /* snprintf(str, 0, ...) does not alter str */
	    nbytes = vsnprintf(opts->_str, opts->_size, format, ap);
	    opts->_size -= nbytes;
	}else
	    /* snprintf(str, negative, ...) is equivalent to snprintf(str, ...)
	     * because size_t is unsigned */
	    nbytes = vsprintf(opts->_str, format, ap);
	opts->_str += nbytes;
    }

    return nbytes;
}

static int oprintf(struct options *opts, const char *format, ...)
{
    va_list ap;
    int nbytes;

    va_start(ap, format);
    nbytes = ovprintf(opts, format, ap);
    va_end(ap);

    return nbytes;
}

/* adjust the width of string specifiers to the display space intead of the
 * number of bytes for wide characters and oprintf them using the adjusted
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
	    nbytes += oprintf(opts, asis);
	    *p = '%';

	    /* skip % */
	    while(*++q){
		char *c = CONVERSIONS - 1;

		while(*++c && *q != *c);
		if(*c){
		    char tmp;
		    /* found a conversion specifier */
		    if(*c == 's'){
			/* if this is a string specifier */
			int width = -1, prec = -1, use_ovprintf = 1;
			char *p_tmp, *s;
			va_list ap_copy;

			/* save this ap and use ovprintf() for non-wide
			 * characters */
			va_copy(ap_copy, ap);

			*p_spec = 0;
			p_spec = spec;
			if(*p_spec == '-')
			    /* alignment */
			    p_spec++;
			if(*p_spec == '*'){
			    /* read width from next argument */
			    width = va_arg(ap, int);
			    p_spec++;
			}else if(*p_spec >= '0' && *p_spec <= '9'){
			    /* read width */
			    p_tmp = p_spec;
			    while(*p_spec >= '0' && *p_spec <= '9')
				p_spec++;
			    tmp = *p_spec;
			    *p_spec = 0;
			    width = atoi(p_tmp);
			    *p_spec = tmp;
			}
			if(*p_spec == '.'){
			    /* precision */
			    p_spec++;
			    if(*p_spec == '*'){
				/* read precision from next argument */
				prec = va_arg(ap, int);
				p_spec++;
			    }else if(*p_spec >= '0' && *p_spec <= '9'){
				/* read precision */
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
			    /* illegal string specifier? */
			    va_end(ap_copy);
			    return -1;
			}

			s = va_arg(ap, char *);
			if(width > 0){
			    /* if width is specified */
			    int wcount = wide_count(s);

			    if(wcount){
				/* if there are wide characters */
				width += wcount;
				prec += prec > 0 ? wcount : 0;
				p_spec = spec;
				p_spec += sprintf(p_spec, "%%%s%d",
					spec[0] == '-' ? "-" : "", width);
				if(prec >= 0)
				    p_spec += sprintf(p_spec, ".%d", prec);
				*p_spec++ = 's';
				*p_spec = 0;
				nbytes += oprintf(opts, spec, s);
				use_ovprintf = 0;
			    }
			    /* else use ovprintf() as much as possible */
			}
			/* else use ovprintf() as much as possible */
			if(use_ovprintf){
			    tmp = *(q + 1);
			    *(q + 1) = 0;
			    nbytes += ovprintf(opts, p, ap_copy);
			    *(q + 1) = tmp;
			}

			va_end(ap_copy);
		    }else{
			/* else use ovprintf() for non-string specifiers */
			tmp = *(q + 1);
			*(q + 1) = 0;
			nbytes += ovprintf(opts, p, ap);
			*(q + 1) = tmp;
		    }
		    break;
		}else
		    *p_spec++ = *q;
	    }
	    asis = (p = q) + 1;
	}
	p++;
    }

    /* print the remaining string */
    *p = 0;
    nbytes += oprintf(opts, asis);
    *p = '%';

    return nbytes;
}
