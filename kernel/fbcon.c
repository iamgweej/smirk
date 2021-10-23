#include <stdint.h>
#include <stddef.h>

#include <logging/font8x16.h>
#include <logging/fbcon.h>

static void putc_fbcon32_at(fbcon_t *fbcon, char c, uint16_t x, uint16_t y)
{
    uint32_t(*fb)[fbcon->height][fbcon->width] = (void *)fbcon->framebuffer;

    const char *chr_bmp = &font_8x16[c * 16];

    size_t start_x = x * FONT_WIDTH,
           start_y = y * FONT_HEIGHT;

    for (size_t y = 0; y < FONT_HEIGHT; y++)
    {
        for (size_t x = 0; x < FONT_WIDTH; x++)
        {
            if ((chr_bmp[y] << x) & 0x80)
            {
                (*fb)[start_y + y][start_x + x] = 0xffffffff;
            }
            else
            {
                (*fb)[start_y + y][start_x + x] = 0;    
            }
        }
    }
}

static void put_newline_fbcon32(fbcon_t *fbcon)
{
    fbcon->next_char_x = 0;
    fbcon->next_char_y++;

    // Did we fall off the screen?
    if (fbcon->next_char_y >= fbcon->height_chars)
    {
        uint32_t(*fb)[fbcon->height][fbcon->width] = (void *)fbcon->framebuffer;

        // Shift the upper lines up (pixel by pixel)
        for (uint16_t row = FONT_HEIGHT; row < fbcon->height; row++)
        {
            for (uint16_t col = 0; col < fbcon->width; col++)
            {
                (*fb)[row - FONT_HEIGHT][col] = (*fb)[row][col];
            }
        }
        

        fbcon->next_char_y = fbcon->height_chars - 1;
        for (uint16_t col = 0; col < fbcon->width; col++)
        {
            putc_fbcon32_at(fbcon, ' ', col, fbcon->next_char_y);
        }
    }
}

void putc_fbcon32(logger_t *logger, char c)
{
    fbcon_t *fbcon = (fbcon_t *)logger;

    if ('\n' == c)
    {
        put_newline_fbcon32(fbcon);
        return;
    }

    putc_fbcon32_at(fbcon, c, fbcon->next_char_x, fbcon->next_char_y);

    fbcon->next_char_x++;

    // Did we fall off a line?
    if (fbcon->next_char_x >= fbcon->width_chars)
    {
        put_newline_fbcon32(fbcon);
    }
}
