#ifndef _SMIRK_FBCON_H
#define _SMIRK_FBCON_H

#include <stdint.h>
#include <stddef.h>

typedef struct console {
    size_t size;
    void (*putc)(struct console *console, char chr,
                 uint16_t x, 
                 uint16_t y);
} console_t;

typedef struct fbcon {
    console_t console;
    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uintptr_t framebuffer;
} fbcon_t;


void puts(console_t *console, char *str);
void putc_fbcon32(console_t *console, char chr, uint16_t x, uint16_t y);

static inline void fbcon_init(fbcon_t *fbcon, uint16_t width, 
                       uint16_t height, uint16_t bpp, uintptr_t framebuffer)
{
    fbcon->console.size = sizeof(fbcon);

    fbcon->width = width;
    fbcon->height = height;
    fbcon->bpp = bpp;
    fbcon->framebuffer = framebuffer;

    switch (fbcon->bpp) {
        case 32:
            fbcon->console.putc = &putc_fbcon32;
        default:
            break;
    }
}

static inline void putc(console_t *console, char chr, uint16_t x, uint16_t y)
{
    return console->putc(console, chr, x, y);
}

#endif /* _SMIRK_FBCON_H */
