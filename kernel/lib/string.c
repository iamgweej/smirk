#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <limits.h>

/*
 * TODO: more efficient implementation (i.e. use dwords)
 */

int memcmp(const void *buf1, const void *buf2, size_t n)
{
    const uint8_t *_buf1 = buf1;
    const uint8_t *_buf2 = buf2;

    for (size_t i = 0; i < n; i++) {
        if (_buf1[i] != _buf2[i])
            return _buf1[i] - _buf2[i];
    }

    return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *_dest = dest;
    const char *_src = src;

    for (size_t i = 0; i < n; i++)
        _dest[i] = _src[i];

    return dest;
}


void *memset(void *dest, int chr, size_t n)
{
    char *_dest = dest;

    for (size_t i = 0; i < n; i++)
        _dest[i] = chr;

    return dest;
}


char *strchr(const char *str, int chr)
{
    char _chr = chr;

    while (*str != '\0' || chr == '\0')
        if (*(str++) == _chr) return (char *) str;

    return (char *) NULL;
}

size_t strlen(const char *str)
{
    return strnlen(str, INT_MAX);
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];

    for (; i < n; i++)
        dest[i] = '\0';

    return dest;
}


size_t strnlen(const char *str, size_t n)
{
    size_t i;

    for (i = 0; i < n && str[i] != '\0'; i++);

    return i;
}


