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
	 *en1 = "ab", *en2 = "cd", *ko1 = "가나", *ko2 = "마바";

    fmt = escape_newlines(format);

    printf("printf(\"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n",
	    fmt, en1, en2, ko1, ko2);
    printf(format, en1, en2, ko1, ko2);

    printf("\n");
    printf("printa(\"%s\", \"%s\", \"%s\" \"%s\", \"%s\");\n",
	    fmt, en1, en2, ko1, ko2);
    printa(format, en1, en2, ko1, ko2);

    free(fmt);

    exit(0);
}
