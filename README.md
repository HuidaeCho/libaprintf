# Libprinta

Libprinta is the print-aligned C library. In the C language, the `strlen()` of a CJK character is not 1 because `strlen()` returns the byte length of a string and CJK characters use more than one byte. For example `strlen("가")` returns 3 because Hangle syllable Ga (가) consists of `0xEA`, `0xB0`, and `0x80`. The same byte length applies to `printf()`'s string specifiers (`%{width}s`). Again, "가" in `printf("%10s|", "가")` will take up three characters and leave seven spaces resulting in

![Ga misaligned](ga-misaligned.png "Ga misaligned")

The pipe character (`|`) is at column 10, not at 11 (10 characters from `%10s` plus `|`) as in

![Ga aligned](ga-aligned.png "Ga aligned")

This library tries to fix this alignment issue and defines the following functions:
* `int wide_count(const char *str)` counts the number of wide characters in a string,
* `int printa(const char *format, ...)` adjusts the width of string specifiers to the display space instead of the number of bytes for wide characters and printf them using the adjusted display width,
* `int sprinta(char *str, const char *format, ...)` is the `sprintf()` version of `printa()`, and
* `int fprinta(FILE *stream, const char *format, ...)` is the `fprintf()` version of `printa()`.

## Installation

```bash
# build the library and test
make

# test
./test
```

![Test](test.png "Test")
