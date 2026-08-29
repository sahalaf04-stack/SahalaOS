#include "../include/gdt.h"

extern void gdt_flush();

void gdt_init()
{
    gdt_flush();
}
