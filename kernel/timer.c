#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40

volatile unsigned int timer_ticks = 0;

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void timer_init()
{
    unsigned short divisor = 11932;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void timer_tick()
{
    timer_ticks++;
}

unsigned int timer_get_ticks()
{
    return timer_ticks;
}
