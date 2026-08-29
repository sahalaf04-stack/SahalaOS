#include "../include/ata.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_PRIMARY_CTRL 0x3F6

#define ATA_REG_DATA       0
#define ATA_REG_ERROR      1
#define ATA_REG_SECCOUNT0  2
#define ATA_REG_LBA0       3
#define ATA_REG_LBA1       4
#define ATA_REG_LBA2       5
#define ATA_REG_HDDEVSEL   6
#define ATA_REG_COMMAND    7
#define ATA_REG_STATUS     7

#define ATA_CMD_READ_PIO   0x20
#define ATA_CMD_WRITE_PIO  0x30
#define ATA_CMD_CACHE_FLUSH 0xE7

#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01

static inline void outb(unsigned short port, unsigned char value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;

    __asm__ volatile ("inb %1, %0"
                      : "=a"(value)
                      : "Nd"(port));

    return value;
}

static inline void insw(unsigned short port, void *buffer, int count)
{
    __asm__ volatile (
        "cld\n"
        "rep insw"
        : "+D"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
}

static inline void outsw(unsigned short port, const void *buffer, int count)
{
    __asm__ volatile (
        "cld\n"
        "rep outsw"
        : "+S"(buffer), "+c"(count)
        : "d"(port)
        : "memory"
    );
}

static int ata_wait_bsy()
{
    int timeout = 1000000;

    while ((inb(ATA_PRIMARY_IO + ATA_REG_STATUS) & ATA_SR_BSY) && timeout--)
    {
    }

    return timeout > 0;
}

static int ata_wait_drq()
{
    int timeout = 1000000;

    while (timeout--)
    {
        unsigned char status = inb(ATA_PRIMARY_IO + ATA_REG_STATUS);

        if (status & ATA_SR_ERR)
            return 0;

        if ((status & ATA_SR_BSY) == 0 &&
            (status & ATA_SR_DRQ))
            return 1;
    }

    return 0;
}

int ata_read_sector(unsigned int lba, unsigned char *buffer)
{
    if (!ata_wait_bsy())
        return -1;

    outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
         0xE0 | ((lba >> 24) & 0x0F));

    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 1);

    outb(ATA_PRIMARY_IO + ATA_REG_LBA0,
         (unsigned char)(lba));

    outb(ATA_PRIMARY_IO + ATA_REG_LBA1,
         (unsigned char)(lba >> 8));

    outb(ATA_PRIMARY_IO + ATA_REG_LBA2,
         (unsigned char)(lba >> 16));

    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND,
         ATA_CMD_READ_PIO);

    if (!ata_wait_drq())
        return -1;

    insw(ATA_PRIMARY_IO + ATA_REG_DATA, buffer, 256);

    return 0;
}

int ata_write_sector(unsigned int lba, const unsigned char *buffer)
{
    if (!ata_wait_bsy())
        return -1;

    outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL,
         0xE0 | ((lba >> 24) & 0x0F));

    outb(ATA_PRIMARY_IO + ATA_REG_SECCOUNT0, 1);

    outb(ATA_PRIMARY_IO + ATA_REG_LBA0,
         (unsigned char)(lba));

    outb(ATA_PRIMARY_IO + ATA_REG_LBA1,
         (unsigned char)(lba >> 8));

    outb(ATA_PRIMARY_IO + ATA_REG_LBA2,
         (unsigned char)(lba >> 16));

    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND,
         ATA_CMD_WRITE_PIO);

    if (!ata_wait_drq())
        return -1;

    outsw(ATA_PRIMARY_IO + ATA_REG_DATA, buffer, 256);

    outb(ATA_PRIMARY_IO + ATA_REG_COMMAND,
         ATA_CMD_CACHE_FLUSH);

    if (!ata_wait_bsy())
        return -1;

    return 0;
}
