volatile unsigned int timer_ticks = 0;

void timer_init()
{
    timer_ticks = 0;
}

void timer_tick()
{
    timer_ticks++;
}

unsigned int timer_get_ticks()
{
    return timer_ticks;
}
