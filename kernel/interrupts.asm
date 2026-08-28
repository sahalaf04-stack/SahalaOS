bits 32

global irq1_handler
global keyboard_scancode

section .data

keyboard_scancode db 0

section .text

irq1_handler:
    pusha

    in al, 0x60

    mov [keyboard_scancode], al

    mov al, 0x20
    out 0x20, al

    popa
    iretd
