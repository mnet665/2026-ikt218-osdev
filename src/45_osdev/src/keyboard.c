#include "../include/keyboard.h"
#include "../include/vga.h"
#include "../include/pic.h"
#include <../include/libc/stdint.h>
#include <../include/libc/stdio.h>

#define KBD_DATA_PORT 0x60

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Scancode to ascii table
static const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z',
    'x','c','v','b','n','m',',','.','/',0,'*',0,' ',  /* up to 63 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


extern void pic_send_eoi(uint8_t irq);

// small ring buffer
#define KBUF_SIZE 256
static volatile uint8_t kbuf[KBUF_SIZE];
static volatile int kbuf_head = 0;
static volatile int kbuf_tail = 0;

void keyboard_handler(int irq) {
    uint8_t scancode = inb(KBD_DATA_PORT);
    if (scancode == 0x0E) { 
        // If backspace, then make code remove last char from buffer.
        if (kbuf_head != kbuf_tail) {
            // move head back
            kbuf_head = (kbuf_head - 1) & (KBUF_SIZE - 1);
            // remove from screen
            vga_backspace();
        } else {
            // nothing to delete
        }
        pic_send_eoi(1);
        return;
    }

    if (scancode < 0x80) {
        char c = scancode_to_ascii[scancode];
        if (c) {
            // push to buffer
            kbuf[kbuf_head] = (uint8_t)c;
            kbuf_head = (kbuf_head + 1) & (KBUF_SIZE - 1);
            // print character using direct VGA write
            vga_putc(c);
        }
    }
    pic_send_eoi(1);
}

char keyboard_getchar(void) {
    volatile int *head = &kbuf_head;
    volatile int *tail = &kbuf_tail;
    // Wait until there is a character in the buffer
    while (*tail == *head) {
        //asm volatile("hlt");  // sleep until next interrupt
    }
    char c = (char)kbuf[kbuf_tail];
    kbuf_tail = (kbuf_tail + 1) & (KBUF_SIZE - 1);
    return c;
} 

void keyboard_install() {
}

