/*******************************************************************************
 * Name:	oaprintf.c (part of libaprintf, the aligned printf C library)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "aprintf.h"

/* printf(3) man page */
#define CONVS "diouxXeEfFgGaAcsCSpnm%"

/* % + flags + width + precision + length + conversoin + NULL */
#define SPEC_BUF_SIZE 16

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
 * display width; return -1 on an illegal specifier and -2 on too long a
 * specifier
 *
 * compare
 *	printf("%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
    가나|
-----------
 * and
 *	oaprintf(&opts, "%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
      가나|
-----------
 */
int oaprintf(struct options *opts, const char *format, va_list ap)
{
    char *fmt, *asis, *p, spec[SPEC_BUF_SIZE];
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
		char *c = CONVS - 1;

		while(*++c && *q != *c);
		if(*c){
		    va_list aq;
		    char tmp;

		    /* copy ap for ovprintf() */
		    va_copy(aq, ap);

		    /* found a conversion specifier */
		    if(*c == 's'){
			/* if this is a string specifier */
			int width = -1, prec = -1, use_ovprintf = 1;
			char *p_tmp, *s;

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
			    va_end(aq);
			    return -1;
			}

			s = va_arg(ap, char *);
			if(width > 0){
			    /* if width is specified */
			    int wcount = count_wide_chars(s);

			    if(wcount){
				/* if there are wide characters */
				if(prec > 0)
				    width += count_wide_chars_in_cols(s, prec,
								      &prec);
				else if(prec < 0)
				    width += wcount;
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
			    nbytes += ovprintf(opts, p, aq);
			    *(q + 1) = tmp;
			}
		    }else{
			/* else use ovprintf() for non-string specifiers */
			tmp = *(q + 1);
			*(q + 1) = 0;
			nbytes += ovprintf(opts, p, aq);
			*(q + 1) = tmp;

			/* once ap is passed to another function that calls
			 * va_arg() on it, its value becomes undefined
			 * (printf(3) man page) or indeterminate
			 * (http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf
			 * section 7.15 paragraph 3) after the callee function
			 * returns; simply passing ap to ovprintf() works on
			 * Linux, but it doesn't on MinGW on Windows; pass its
			 * copy and skip an argument manually; argument types
			 * from printf(3) man page */
			switch(*c){
			    case 'd':
			    case 'i':
			    case 'o':
			    case 'u':
			    case 'x':
			    case 'X':
			    case 'c':
			    case 'C':
			    case 'S':
				va_arg(ap, int);
				break;
			    case 'e':
			    case 'E':
			    case 'f':
			    case 'F':
			    case 'g':
			    case 'G':
			    case 'a':
			    case 'A':
				va_arg(ap, double);
				break;
			    case 'p':
				va_arg(ap, void *);
				break;
			    case 'n':
				va_arg(ap, int *);
				break;
			    /* otherwise, no argument is required for m% */
			}
		    }
		    va_end(aq);
		    break;
		}else if(p_spec - spec < SPEC_BUF_SIZE - 2)
		    /* 2 reserved for % and NULL */
		    *p_spec++ = *q;
		else
		    return -2;
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
