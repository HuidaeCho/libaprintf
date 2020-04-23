/*******************************************************************************
 * Name:	test.c (part of libaprintf, the aligned printf C library)
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
#include "aprintf.h"

int main()
{
    char buf[1024];
    int size, written;

    printf("======= printf =======\n");
    printf("%-10s|%-10s|\n%-10s|%-10s|\n", "1 abcd", "efgh", "  가나", "다라");

    printf("\n====== aprintf =======\n");
    aprintf("%-10s|%-10s|\n%-10s|%-10s|\n", "2 abcd", "efgh", "  가나", "다라");
    faprintf(stdout, "%-10s|%-10s|\n%-10s|%-10s|\n", "3 abcd", "efgh", "  가나", "다라");

    saprintf(buf, "%d%9s|%10s|\n%10s|%10s|\n", 4, "abcd", "efgh", "  가나", "다라");
    printf(buf);

    /* truncates "라" because of the size limit (bytes not display width!) */
    size = 47;
    written = snaprintf(buf, size, "%d%9s|%10s|\n%10s|%10s|\n", 5, "abcd", "efgh", "  가나", "다라");
    printf(buf);
    if(written >= size)
	printf("\n");

    aprintf("%-10.0s|%-10.0s|\n", "6 abcdefghij", "가나다라마바");
    aprintf("%-10.1s|%-10.1s|\n", "7 abcdefghij", "가나다라마바");
    aprintf("%-10.*s|%-10.*s|\n", 2, "8 abcdefghij", 2, "가나다라마바");
    aprintf("%-10.*s|%-10.*s|\n", 3, "9 abcdefghij", 3, "가나다라마바");
    aprintf("%-*.4s|%-*.*s|\n", 10, "A abcdefghij", 10, 4, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 5, "B abcdefghij", 10, 5, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 6, "C abcdefghij", 10, 6, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 7, "D abcdefghij", 10, 7, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 8, "E abcdefghij", 10, 8, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 9, "F abcdefghij", 10, 9, "가나다라마바");
    aprintf("%-*.*s|%-*.*s|\n", 10, 10, "G abcdefghij", 10, 10, "가나다라마바");

    exit(0);
}
