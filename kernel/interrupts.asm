bits 32

global irq0_handler
global irq0_count

section .data

irq0_count dd 0

section .text

irq0_handler:
    pusha

    inc dword [irq0_count]

    mov al, 0x20
    out 0x20, al

    popa
    iretd
