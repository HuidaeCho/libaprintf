/*******************************************************************************
 * Name:	count.c (part of libprinta, the print-aligned C library)
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

/* count the number of wide characters in a string */
int count_wide_chars(const char *str)
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

/* count the number of wide characters in a string in a number of columns */
int count_wide_chars_in_cols(const char *str, int ncols)
{
    int count = 0, lead = 0;

    str--;
    while(ncols >= 0 && *++str){
	if((*str & 0xc0) != 0x80){
	    lead = 1;
	    ncols--;
	}else if(lead){
	    lead = 0;
	    ncols--;
	    count++;
	}
    }
    if((*str & 0xc0) == 0x80)
	count--;

    return count;
}

/* count the number of bytes in a string in a number of columns */
int count_bytes_in_cols(const char *str, int ncols)
{
    const char *p = str - 1;
    int lead = 0;

    while(ncols >= 0 && *++p){
	if((*p & 0xc0) != 0x80){
	    lead = 1;
	    ncols--;
	}else if(lead){
	    lead = 0;
	    ncols--;
	}
    }

    return p - str;
}
