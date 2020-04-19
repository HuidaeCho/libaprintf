/*******************************************************************************
 * Name:	test.c (part of libprinta, the print-aligned C library)
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
#include "printa.h"

char *escape_newlines(const char *str)
{
    const char *p = str;
    int count = 0, len = 0;
    char *buf, *q;

    while(*p){
	if(*p++ == '\n')
	    count++;
	len++;
    }

    q = buf = (char *)malloc(len + count + 1);
    p = str;

    while(*p){
	if(*p == '\n'){
	    *q++ = '\\';
	    *q++ = 'n';
	}else
	    *q++ = *p;
	p++;
    }
    *q = 0;

    return buf;
}

int main()
{
    char *format = "%-10s|%-10s|\n%-10s|%-10s|\n", *fmt,
	 *en1 = "ab", *en2 = "cd", *ko1 = "가나", *ko2 = "마바",
	 buf[1024];
    size_t size = 26;

    fmt = escape_newlines(format);

    printf("printf(\"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n",
	    fmt, en1, en2, ko1, ko2);
    printf(format, en1, en2, ko1, ko2);

    printf("\n");
    printf("printa(\"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n",
	    fmt, en1, en2, ko1, ko2);
    printa(format, en1, en2, ko1, ko2);

    printf("\n");
    printf("fprinta(stdout, \"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n",
	    fmt, en1, en2, ko1, ko2);
    fprinta(stdout, format, en1, en2, ko1, ko2);

    printf("\n");
    printf("char buf[1024];\n"
	   "sprinta(buf, \"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n"
	   "printf(buf);\n",
	    fmt, en1, en2, ko1, ko2);
    sprinta(buf, format, en1, en2, ko1, ko2);
    printf(buf);

    printf("\n");
    printf("char buf[1024];\n"
	   "snprinta(buf, %ld, \"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n"
	   "printf(buf);\n",
	    size, fmt, en1, en2, ko1, ko2);
    /* truncates "가" because of the size limit (bytes not display width!) */
    snprinta(buf, size, format, en1, en2, ko1, ko2);
    printf(buf);

    printa("\n%-*.*s|%-*.*s|\n", 10, 2, "abcd", 10, 2, "가나다라");

    free(fmt);

    exit(0);
}
