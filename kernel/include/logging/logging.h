#ifndef _SMIRK_LOGGING_H
#define _SMIRK_LOGGING_H

#include <stddef.h>

typedef struct logger
{
    void (*putc)(struct logger *logger, char c);
} logger_t;

static inline void putc(logger_t *logger, char c) {
    logger->putc(logger, c);
}

static inline void puts(logger_t *logger, const char *s) {
    char c;
    
    while ((c = *s++)) {
        putc(logger, c);
    }
}

#endif /* _SMIRK_LOGGING_H */