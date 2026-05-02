#include "../include/idt.h"
#include "../include/vga.h"   // or your printf header path
#include <../include/libc/stdint.h>

extern void lidt(void*); // assembly stub that does lidt with pointer

#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;


static void set_idt_entry(int n, uint32_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector   = sel;
    idt[n].zero       = 0;
    idt[n].type_attr  = flags;
    idt[n].offset_high= (handler >> 16) & 0xFFFF;
}

void set_idt_entry_public(int n, uint32_t handler) {
    uint16_t code_sel = 0x08;
    uint8_t intr_flags = 0x8E;
    set_idt_entry(n, handler, code_sel, intr_flags);
}


extern void isr0();
extern void isr1();
extern void isr2();

void idt_install(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base  = (uint32_t)&idt;

    // zero the table
    for (int i = 0; i < IDT_ENTRIES; ++i) {
        set_idt_entry(i, 0, 0, 0);
    }

    // Use kernel code segment selector (typically 0x08). Adjust if your GDT differs.
    uint16_t code_sel = 0x08;
    // 0x8E = present, ring 0, 32-bit interrupt gate (10001110b)
    uint8_t intr_flags = 0x8E;

    set_idt_entry(0,  (uint32_t)isr0, code_sel, intr_flags);
    set_idt_entry(1,  (uint32_t)isr1, code_sel, intr_flags);
    set_idt_entry(2,  (uint32_t)isr2, code_sel, intr_flags);

    // load IDT
    lidt(&idtp);
}

