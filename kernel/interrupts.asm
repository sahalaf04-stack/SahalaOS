bits 32

global irq0_handler
extern timer_tick

section .text

irq0_handler:
    pusha

    call timer_tick

    mov al, 0x20
    out 0x20, al

    popa
    iretd
