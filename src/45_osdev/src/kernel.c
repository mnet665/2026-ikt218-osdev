#include <../include/libc/stdio.h>
#include "../include/idt.h"
#include "../include/irq.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/pit.h"
#include "../include/vga.h"
#include "../include/song.h"

extern void idt_install(void);
extern void irq_install();
extern void keyboard_install();

//  Declared the C++ song player function so C can call it
extern void play_song_impl(Song* song);

// Print Menu 
void print_menu(void) {
    vga_clear();
    printf("==========================================\n");
    printf("       UiA Operating System group 45 - Menu        \n");
    printf("==========================================\n");
    printf("\n");
    printf("  1. Info & Tests\n");
    printf("  2. Play Song\n");
    printf("  3. Terminal\n");
    printf("\n");
    printf("==========================================\n");
    printf("Select an option (1-3): ");
}

// Option 1: Info & Tests
void run_info_and_tests(void) {
    vga_clear();
    printf("==========================================\n");
    printf("           Info & Tests                   \n");
    printf("==========================================\n");
    printf("\n");

    // Memory info
    printf("--- Memory Layout ---\n");
    print_memory_layout();
    printf("\n");

    // Malloc test
    printf("--- Malloc Test ---\n");
    void* some_memory = malloc(12345);
    void* memory2     = malloc(54321);
    void* memory3     = malloc(13331);
    printf("malloc(12345) = 0x%x\n", (unsigned int)some_memory);
    printf("malloc(54321) = 0x%x\n", (unsigned int)memory2);
    printf("malloc(13331) = 0x%x\n", (unsigned int)memory3);
    printf("\n");

    // Sleep / interrupt test
    printf("--- Sleep & Interrupt Test ---\n");
    printf("Press 0 to return to menu.\n\n");

    int counter = 0;
    while (1) {
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);

        // Non-blocking check: only read if something is in the buffer
        char c = keyboard_getchar();
        if (c == '0') {
            printf("\nReturning to menu...\n");
            return;
        }
    }
}

// Option 2 - plays a Song 
void run_song(void) {
    vga_clear();
    printf("==========================================\n");
    printf("              Song Player                 \n");
    printf("==========================================\n");
    printf("\n");

    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)}
    };

    printf("Playing Twinkle Twinkle Little Star...\n");
    printf("Press 0 after song ends to return to menu.\n\n");

    play_song_impl(&songs[0]);

    printf("\nDone! Press 0 to return to menu.\n");
    while (keyboard_getchar() != '0');
}

// Option 3: Terminal
void run_terminal(void) {
    vga_clear();
    printf("==========================================\n");
    printf("               Terminal                   \n");
    printf("==========================================\n");
    printf("Type anything and press Enter.\n");
    printf("Type 0 and press Enter to return to menu.\n\n");

    while (1) {
        printf("> ");

        // Collect a line - keyboard_handler already prints each char to screen
        // so we just read from buffer without echoing again
        char line[256];
        int i = 0;

        while (1) {
            char c = keyboard_getchar();

            if (c == '\n' || c == '\r') {
                // newline handled by keyboard_handler
                line[i] = '\0';
                break;

            } else {
                //  just store charchter
                if (i < 255) line[i++] = c;
            }
        }

        // Check if user typed "0" to exit
        if (i == 1 && line[0] == '0') {
            printf("Returning to menu...\n");
            return;
        }
    }
}

//  Main Menu Loop 
void run_menu(void) {
    while (1) {
        print_menu();
        char choice = keyboard_getchar();

        switch (choice) {
            case '1':
                run_info_and_tests();
                break;
            case '2':
                run_song();
                break;
            case '3':
                run_terminal();
                break;
            default:
                // re display the menu
                break;
        }
    }
}

// Kernel Main 
void kernel_main(void) {

    // Initialize memory first
    init_kernel_memory(&end);

    // IDT and IRQ initialization
    idt_install();
    irq_install();
    keyboard_install();

    // Initialize paging
    init_paging();

    // Initialize PIT
    init_pit();

    // Enable interrupts
    asm volatile("sti");

    // Flush keypresses during boot
    sleep_interrupt(100);
    
    // Start the menu
    run_menu();

    // Should never reach here
    while (1) {
        asm volatile("hlt");
    }
}

int main(void) {
    kernel_main();
    return 0;
}