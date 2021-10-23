#include <stdint.h>
#include <stddef.h>

#include <fbcon.h>
#include <font8x16.h>

#define FONT_HEIGHT     (16)
#define FONT_WIDTH      (8)

//void fbcon_init(fbcon_t *fbcon, uint16_t width, 
//                       uint16_t height, uint16_t bpp, uintptr_t framebuffer);
//void putc(struct fbcon *fbcon, char chr, uint16_t x, uint16_t y);

void putc_fbcon32(console_t *console, char chr, uint16_t x, uint16_t y)
{
    fbcon_t *fbcon = (fbcon_t *) console;
    uint32_t (*fb)[fbcon->height][fbcon->width] = fbcon->framebuffer;

    const char *chr_bmp = &font_8x16[chr * 16];

    size_t start_x = x * FONT_WIDTH,
           start_y = y * FONT_HEIGHT;

    for (size_t y = 0; y < FONT_HEIGHT; y++) {
        for (size_t x = 0; x < FONT_WIDTH; x++) {
            if ((chr_bmp[y] << x) & 0x80) {
                (*fb)[start_y + y][ start_x + x] = 0xffffffff;
            }
        }
    }
}

void puts(console_t *console, char *str)
{
    int i = 0;

    while (*str)
        putc(console, *(str++), i++, 0);
}

