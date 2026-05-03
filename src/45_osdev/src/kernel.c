#include <../include/libc/stdio.h>
#include "../include/idt.h"
#include "../include/irq.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/pit.h"

extern void idt_install(void);
extern void irq_install();
extern void keyboard_install();

// main kernel function
void kernel_main(void) {    
	
    printf("Helloo World!\n");

    // Initialize memory first
    init_kernel_memory(&end);
   
    // IDT and IRQ initialization
    idt_install();
    irq_install();
    keyboard_install();

    // initialize paging
    init_paging();
    print_memory_layout();
    

    // initialize pit
    init_pit(); 
  
    printf("keyboard enabled\n");

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);

    //printf("Testing some interrupts:\n");

      // test software interrupt
    //asm volatile ("int $0x00");
    //asm volatile ("int $0x01");
    //asm volatile ("int $0x02");

    asm volatile("sti");


    printf("\n");
    printf("Welcome to the operating system!\n");

    // Evaluation code from canvas
    int counter = 0;
    while(true){
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
};

    // Infinite loop 
    while (1) {
        asm volatile("hlt");
    }
}

int main(void) {
 
    kernel_main();
    return 0;
}

