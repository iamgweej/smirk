#ifndef _SMIRK_STRING_H
#define _SMIRK_STRING_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
//char *strchr(const char *, int);
//int strcmp(const char *, const char *);
size_t strlen(const char *);
//char *strncat(char *, const char *, size_t);
//int strncmp(const char *, const char *, size_t);
char *strncpy(char *, const char *, size_t);
size_t strnlen(const char *, size_t);
//char *strstr(const char *, const char *);

#endif /* _SMIRK_STRING_H */
