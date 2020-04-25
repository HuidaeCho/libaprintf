/*******************************************************************************
 * Name:	count.c (part of libaprintf, the aligned printf C library)
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

/* count the number of wide characters in a string */
int count_wide_chars(const char *str)
{
    int nwchars = 0, lead = 0;

    while(*str)
	/* if the first two bits are 10 (0x80 = 1000 0000), this byte is
	 * following a previous multi-byte character */
	if((*str++ & 0xc0) != 0x80)
	    lead = 1;
	else if(lead){
	    /* only count the second byte of a multi-byte character */
	    lead = 0;
	    nwchars++;
	}

    return nwchars;
}

/* count the numbers of wide characters and bytes in a string in a number of
 * columns */
int count_wide_chars_in_cols(const char *str, int ncols, int *nbytes)
{
    const char *p = str - 1;
    int lead = 0, nwchars = 0;

    /* count the numbers of wide characters and bytes in one loop */
    while(ncols >= 0 && *++p)
	if((*p & 0xc0) != 0x80){
	    /* a single-byte character or the leading byte of a multi-byte
	     * character; don't count it */
	    lead = 1;
	    ncols--;
	}else if(lead){
	    /* only count the second byte of a multi-byte character; don't
	     * consume more than two columns (leading and second bytes) */
	    lead = 0;
	    ncols--;
	    nwchars++;
	}

    /* if the current byte after ncols is still part of a multi-byte character,
     * trash it because it's not a full wide character */
    if((*p & 0xc0) == 0x80)
	nwchars--;

    /* see how many bytes we have advanced */
    *nbytes = p - str;

    return nwchars;
}
