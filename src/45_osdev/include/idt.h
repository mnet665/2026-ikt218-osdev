#ifndef IDTR_H
#define IDTR_H
#include <../include/libc/stdint.h>


struct idt_entry {
    uint16_t offset_low;    // bits 0..15
    uint16_t selector;      // code segment selector
    uint8_t  zero;          // reserved, set to 0
    uint8_t  type_attr;     // type and attributes
    uint16_t offset_high;   // bits 16..31
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#endif
