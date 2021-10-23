#ifndef _SMIRK_FBCON_H
#define _SMIRK_FBCON_H

#include <logging/logging.h>

#include <stdint.h>
#include <stddef.h>

#include <logging/font8x16.h>

typedef struct fbcon {
    logger_t logger; // Embedded logger

    uint16_t width;
    uint16_t height;
    uint16_t bpp;
    uintptr_t framebuffer;

    uint16_t next_char_x;
    uint16_t next_char_y;
    uint16_t width_chars;
    uint16_t height_chars;

} fbcon_t;


void putc_fbcon32(logger_t *logger, char c);

static inline void fbcon_init(fbcon_t *fbcon, uint16_t width, 
                       uint16_t height, uint16_t bpp, uintptr_t framebuffer)
{
    fbcon->width = width;
    fbcon->height = height;
    fbcon->bpp = bpp;
    fbcon->framebuffer = framebuffer;

    fbcon->next_char_x = 0;
    fbcon->next_char_y = 0;
    fbcon->width_chars = width / FONT_WIDTH;
    fbcon->height_chars = height / FONT_HEIGHT;

    switch (fbcon->bpp) {
        case 32:
            fbcon->logger.putc = &putc_fbcon32;
        default:
        // TODO: panic();
            break;
    }
}

#endif /* _SMIRK_FBCON_H */
