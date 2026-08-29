#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static unsigned char rtc_read(unsigned char reg)
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static unsigned char bcd_to_binary(unsigned char value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

void rtc_get_time(
    unsigned char *second,
    unsigned char *minute,
    unsigned char *hour,
    unsigned char *day,
    unsigned char *month,
    unsigned short *year
)
{
    unsigned char rtc_year;
    unsigned char century;

    *second = bcd_to_binary(rtc_read(0x00));
    *minute = bcd_to_binary(rtc_read(0x02));
    *hour   = bcd_to_binary(rtc_read(0x04));
    *day    = bcd_to_binary(rtc_read(0x07));
    *month  = bcd_to_binary(rtc_read(0x08));

    rtc_year = bcd_to_binary(rtc_read(0x09));
    century = bcd_to_binary(rtc_read(0x32));

    if (century == 0)
        century = 20;

    *year = (unsigned short)(century * 100 + rtc_year);
}
