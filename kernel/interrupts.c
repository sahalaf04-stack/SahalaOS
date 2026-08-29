#define IDT_ENTRIES 256

struct IDTEntry
{
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short offset_high;
} __attribute__((packed));

struct IDTPointer
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct IDTEntry idt[IDT_ENTRIES];
static struct IDTPointer idt_pointer;

extern void irq0_handler();

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void idt_set_gate(
    int number,
    unsigned int base,
    unsigned short selector,
    unsigned char flags
)
{
    idt[number].offset_low = base & 0xFFFF;
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].type_attr = flags;
    idt[number].offset_high = (base >> 16) & 0xFFFF;
}

void interrupts_init()
{
    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.base = (unsigned int)&idt;

    /*
     * Clear IDT.
     */
    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    /*
     * IRQ0 -> interrupt 32.
     */
    idt_set_gate(
        32,
        (unsigned int)irq0_handler,
        0x08,
        0x8E
    );

    /*
     * Load IDT.
     */
    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_pointer)
    );

    /*
     * Remap master/slave PIC.
     */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /*
     * Enable only IRQ0.
     *
     * Bit 0 = 0 -> IRQ0 enabled
     * Other IRQs masked.
     */
    outb(0x21, 0xFE);

    /*
     * Mask all slave IRQs.
     */
    outb(0xA1, 0xFF);

    /*
     * Enable CPU interrupts.
     */
    __asm__ volatile ("sti");
}
