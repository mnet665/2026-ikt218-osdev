bits 32
global isr0
global isr1
global isr2
global lidt
extern isr_common_c

SECTION .text

isr0:
    cli
    push dword 0        ; error code placeholder
    push dword 0        ; interrupt number
    call isr_common_c
    add  esp, 8
    sti
    iretd

isr1:
    cli
    push dword 0
    push dword 1
    call isr_common_c
    add  esp, 8
    sti
    iretd

isr2:
    cli
    push dword 0
    push dword 2
    call isr_common_c
    add  esp, 8
    sti
    iretd

; lidt wrapper: expects pointer to idt_ptr as argument (C pushes it on stack)
lidt:
    ; [esp+4] is return address, [esp+0] is old esp? In cdecl, argument at [esp+4]
    ; but in NASM we access [esp+4] for first arg after return address.
    mov eax, [esp + 4]
    lidt [eax]
    ret
