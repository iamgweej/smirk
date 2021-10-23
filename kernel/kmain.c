#include <stdint.h>
#include <stddef.h>

#include <logging/fbcon.h>

#include "include/stivale2.h"

#define STARTUP_MESSAGE ("Hello SMIRK!")

__attribute__((aligned(16))) static uint8_t stack[0x2000];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uintptr_t)&terminal_hdr_tag
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

/**
 * NOTE:
 * In the 32 bit case, we should define _STIVALE2_SPLIT_64.
 * For some reason GCC cant do the extension from uint32_t to uint64_t at "compile" (load) time.
 * This is fine since we're using brace initialization which zeros out all other members (i.e. entry_point_hi).
 * This might cuase problems if we ever want to port to a processor with different bitness...
 */
__attribute__((section(".stivale2hdr"), used)) // Fuck vscode
static struct stivale2_header stivale2_header = {
    .entry_point = 0,
    .stack = (uintptr_t)&stack + sizeof(stack),
    .flags = (1 << 2),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)(uintptr_t)stivale2_struct->tags;

    while (current_tag != NULL && current_tag->identifier != id)
        current_tag = (void *)current_tag->next;

    return current_tag;
}

void _start(struct stivale2_struct *stivale2_struct)
{
    struct stivale2_struct_tag_framebuffer *tag_framebuffer = NULL;
    tag_framebuffer = stivale2_get_tag(stivale2_struct, 
                                       STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    fbcon_t fbcon;

    fbcon_init(&fbcon,
               tag_framebuffer->framebuffer_width,
               tag_framebuffer->framebuffer_height,
               tag_framebuffer->framebuffer_bpp,
               tag_framebuffer->framebuffer_addr);


    puts(&fbcon.logger, "Hello, World!\n");
    puts(&fbcon.logger, "How Are you?");

    for (;;) {
        asm ("hlt");
    }
}
