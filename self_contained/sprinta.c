/*******************************************************************************
 * Name:	sprinta.c self-contained version
 *		(part of libprinta, the print-aligned C library)
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

/* printf(3) man page */
#define CONVERSIONS "diouxXeEfFgGaAcsCSpnm%"

/* count the number of wide characters in a string */
int wide_count(const char *str)
{
    int count = 0, lead = 0;

    while(*str)
	/* if the first two bits are 10 (0x80 = 1000 0000), this byte is
	 * following a previous multi-byte character; only count the leading
	 * byte */
	if((*str++ & 0xc0) != 0x80)
	    lead = 1;
	else if(lead){
	    lead = 0;
	    count++;
	}

    return count;
}

/* adjust the width of string specifiers to the display space intead of the
 * number of bytes for wide characters and sprintf them using the adjusted
 * display width
 *
 * compare
 *	sprintf(buf, "%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
    가나|
-----------
 * and
 *	sprinta(buf, "%10s|\n%10s|\n", "ABCD", "가나");
-----------
      ABCD|
      가나|
-----------
 */
int sprinta(char *str, const char *format, ...)
{
    va_list ap;
    char *fmt, *asis, *p, spec[10];
    int nbytes = 0;

    va_start(ap, format);

    /* make a copy so we can temporarily change the format string */
    p = asis = fmt = (char *)malloc(strlen(format) + 1);
    strcpy(fmt, format);

    while(*p){
	if(*p == '%'){
	    char *q = p, *p_spec = spec;

	    /* print the string before this specifier */
	    *p = 0;
	    nbytes += sprintf(str + nbytes, asis);
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

			/* save this ap and use vprintf() for non-wide characters */
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
				p_spec += sprintf(p_spec, "%%%s%d", spec[0] == '-' ? "-" : "", width);
				if(prec >= 0)
				    p_spec += sprintf(p_spec, ".%d", prec);
				*p_spec++ = 's';
				*p_spec = 0;
				nbytes += sprintf(str + nbytes, spec, s);
				use_printf = 0;
			    }
			}
			if(use_printf){
			    tmp = *(q + 1);
			    *(q + 1) = 0;
			    nbytes += vsprintf(str + nbytes, p, ap_copy);
			    *(q + 1) = tmp;
			}

			va_end(ap_copy);
		    }else{
			/* else use vprintf() */
			tmp = *(q + 1);
			*(q + 1) = 0;
			nbytes += vsprintf(str + nbytes, p, ap);
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
    nbytes += sprintf(str + nbytes, asis);
    *p = '%';

    va_end(ap);

    return nbytes;
}