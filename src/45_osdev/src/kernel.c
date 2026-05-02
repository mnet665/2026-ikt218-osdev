#include <../include/libc/stdio.h>
#include "../include/idt.h"
#include "../include/irq.h"
#include "../include/keyboard.h"

extern void idt_install(void);
extern void irq_install();
extern void keyboard_install();

// main kernel function
void kernel_main(void) {    
	
    printf("Helloo World!\n");
  
    idt_install();
    irq_install();
    keyboard_install();
  
    printf("keyboard enabled\n");
    printf("Testing some interrupts:\n");

      // test software interrupt
    asm volatile ("int $0x00");
    asm volatile ("int $0x01");
    asm volatile ("int $0x02");

    asm volatile("sti");


    printf("\n");
    printf("Welcome to the operating system!\n");

    // Infinite loop 
    while (1) {
        asm volatile("hlt");
    }
}

int main(void) {
 
    kernel_main();
    return 0;
}

