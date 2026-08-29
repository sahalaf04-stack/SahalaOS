extern void pfs_init();
extern const char *pfs_pwd();
extern int pfs_cd(const char *);
extern int pfs_create_dir(const char *);
extern int pfs_create_file(const char *);
extern int pfs_create(const char *);
extern const char *pfs_read(const char *);
extern int pfs_write(const char *, const char *);
extern int pfs_delete(const char *);
extern void pfs_list(void (*)(const char *, int));
extern int ata_read_sector(unsigned int, unsigned char *);
extern int ata_write_sector(unsigned int, const unsigned char *);
extern void timer_init();
extern void interrupts_init();
extern void rtc_get_time(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned short *);
extern void fs_init();
extern int fs_delete(const char *);
extern int fs_create(const char *);
extern const char *fs_read(const char *);
extern int fs_write(const char *, const char *);
extern int fs_delete(const char *);
extern void fs_list(void (*)(const char *));
extern void gdt_init();
extern unsigned int timer_get_ticks();
#define VIDEO_MEMORY 0xB8000

#define WHITE 0x07
#define GREEN 0x0A
#define CYAN  0x0B
#define RED   0x0C


static int row = 5;
static int col = 0;

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

void put_char(char c, int r, int cpos, unsigned char color)
{
    char *video = (char *)VIDEO_MEMORY;
    int index = (r * 80 + cpos) * 2;

    video[index] = c;
    video[index + 1] = color;
}

void clear_screen()
{
    char *video = (char *)VIDEO_MEMORY;

    for (int i = 0; i < 80 * 25; i++)
    {
        video[i * 2] = ' ';
        video[i * 2 + 1] = WHITE;
    }
}

void print(const char *text, int r, int cpos, unsigned char color)
{
    while (*text && cpos < 80)
    {
        put_char(*text, r, cpos, color);
        text++;
        cpos++;
    }
}

int equal(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return 0;

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

char scancode_to_char(unsigned char code)
{
    switch (code)
    {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        case 0x20: return 'd';
        case 0x12: return 'e';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x17: return 'i';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x32: return 'm';
        case 0x31: return 'n';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x10: return 'q';
        case 0x13: return 'r';
        case 0x1F: return 's';
        case 0x14: return 't';
        case 0x16: return 'u';
        case 0x2F: return 'v';
        case 0x11: return 'w';
        case 0x2D: return 'x';
        case 0x15: return 'y';
        case 0x2C: return 'z';

        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';

        case 0x39: return ' ';

        default: return 0;
    }
}

void print_prompt()
{
    const char *path = pfs_pwd();
    int pos = 0;

    print("SahalaOS:", row, 0, GREEN);
    pos = 9;

    while (path[pos - 9] != '\0' && pos < 30)
    {
        put_char(path[pos - 9], row, pos, GREEN);
        pos++;
    }

    put_char('$', row, pos, GREEN);
    put_char(' ', row, pos + 1, GREEN);

    col = pos + 2;
}

void reboot()
{
    unsigned char good;

    do
    {
        good = inb(0x64);
    }
    while (good & 0x02);

    __asm__ volatile (
        "movb $0xFE, %%al\n"
        "outb %%al, $0x64\n"
        :
        :
        : "al"
    );

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}

void print_file(const char *name, int type)
{
    if (type == 2)
        print(name, row, 2, CYAN);
    else
        print(name, row, 2, WHITE);
    row++;
}

void execute_command(char *command)
{
    row++;

    if (equal(command, "help"))
    {
        print("Available commands:", row, 0, CYAN);
        row++;

        print("help     - Show this help", row, 2, WHITE);
        row++;

        print("about    - About SahalaOS", row, 2, WHITE);
        row++;

        print("version  - Show OS version", row, 2, WHITE);
        row++;
        print("time     - Show system uptime", row, 2, WHITE);
        row++;

        print("echo     - Print text", row, 2, WHITE);
        row++;

        print("clear    - Clear screen", row, 2, WHITE);
        row++;
        print("reboot   - Restart the OS", row, 2, WHITE);
        row++;

        print("ls       - List files", row, 2, WHITE);
        row++;

        print("mkdir    - Create a file entry", row, 2, WHITE);
        row++;

        print("write    - Write to a file", row, 2, WHITE);
        row++;

        print("cat      - Read a file", row, 2, WHITE);
        row++;

        print("delete   - Delete a file", row, 2, WHITE);
        row++;
        print("touch    - Create an empty file", row, 2, WHITE);
        row++;
        print("pwd      - Show current directory", row, 2, WHITE);
        row++;
        print("cd       - Change directory", row, 2, WHITE);
        row++;

        print("disktest - Test disk read/write", row, 2, WHITE);
        row++;

        print("cat      - Read a file", row, 2, WHITE);
        row++;

        print("rm       - Delete a file", row, 2, WHITE);
        row++;
    }
    else if (equal(command, "about"))
    {
        print("SahalaOS is a custom operating system", row, 0, CYAN);
        row++;

        print("built from scratch using C and Assembly.", row, 0, WHITE);
    }
    else if (equal(command, "version"))
    {
        print("SahalaOS v0.2", row, 0, GREEN);
        row++;

        print("Architecture: x86 32-bit", row, 0, WHITE);
    }
    else if (command[0] == 'e' &&
             command[1] == 'c' &&
             command[2] == 'h' &&
             command[3] == 'o' &&
             command[4] == ' ')
    {
        print(command + 5, row, 0, WHITE);
    }
    else if (equal(command, "time"))
    {
        unsigned int seconds = timer_get_ticks() / 100;
        char text[16];
        int i = 0;

        print("SahalaOS uptime: ", row, 0, CYAN);

        if (seconds == 0)
        {
            put_char('0', row, 17, WHITE);
            put_char('s', row, 18, WHITE);
        }
        else
        {
            while (seconds > 0)
            {
                text[i++] = '0' + (seconds % 10);
                seconds /= 10;
            }

            int pos = 17;

            while (i > 0)
            {
                put_char(text[--i], row, pos++, WHITE);
            }

            put_char('s', row, pos, WHITE);
        }

        row++;
    }

    else if (equal(command, "rtc"))
    {
        unsigned char second, minute, hour;
        unsigned char day, month;
        unsigned short year;

        rtc_get_time(
            &second,
            &minute,
            &hour,
            &day,
            &month,
            &year
        );

        print("RTC: ", row, 0, CYAN);

        int pos = 5;

        /* HH:MM:SS */
        put_char('0' + (hour / 10), row, pos++, WHITE);
        put_char('0' + (hour % 10), row, pos++, WHITE);
        put_char(':', row, pos++, WHITE);

        put_char('0' + (minute / 10), row, pos++, WHITE);
        put_char('0' + (minute % 10), row, pos++, WHITE);
        put_char(':', row, pos++, WHITE);

        put_char('0' + (second / 10), row, pos++, WHITE);
        put_char('0' + (second % 10), row, pos++, WHITE);

        pos += 2;

        /* DD/MM/YYYY */
        put_char('0' + (day / 10), row, pos++, WHITE);
        put_char('0' + (day % 10), row, pos++, WHITE);
        put_char('/', row, pos++, WHITE);

        put_char('0' + (month / 10), row, pos++, WHITE);
        put_char('0' + (month % 10), row, pos++, WHITE);
        put_char('/', row, pos++, WHITE);

        put_char('0' + ((year / 1000) % 10), row, pos++, WHITE);
        put_char('0' + ((year / 100) % 10), row, pos++, WHITE);
        put_char('0' + ((year / 10) % 10), row, pos++, WHITE);
        put_char('0' + (year % 10), row, pos++, WHITE);

        row++;
    }

    else if (equal(command, "date"))
    {
        unsigned char second, minute, hour;
        unsigned char day, month;
        unsigned short year;

        rtc_get_time(
            &second,
            &minute,
            &hour,
            &day,
            &month,
            &year
        );

        print("Date: ", row, 0, CYAN);

        int pos = 6;

        put_char('0' + (day / 10), row, pos++, WHITE);
        put_char('0' + (day % 10), row, pos++, WHITE);
        put_char('/', row, pos++, WHITE);

        put_char('0' + (month / 10), row, pos++, WHITE);
        put_char('0' + (month % 10), row, pos++, WHITE);
        put_char('/', row, pos++, WHITE);

        put_char('0' + ((year / 1000) % 10), row, pos++, WHITE);
        put_char('0' + ((year / 100) % 10), row, pos++, WHITE);
        put_char('0' + ((year / 10) % 10), row, pos++, WHITE);
        put_char('0' + (year % 10), row, pos++, WHITE);

        row++;
    }

    else if (equal(command, "clock"))
    {
        unsigned char second, minute, hour;
        unsigned char day, month;
        unsigned short year;

        rtc_get_time(
            &second,
            &minute,
            &hour,
            &day,
            &month,
            &year
        );

        print("Time: ", row, 0, CYAN);

        int pos = 6;

        put_char('0' + (hour / 10), row, pos++, WHITE);
        put_char('0' + (hour % 10), row, pos++, WHITE);
        put_char(':', row, pos++, WHITE);

        put_char('0' + (minute / 10), row, pos++, WHITE);
        put_char('0' + (minute % 10), row, pos++, WHITE);
        put_char(':', row, pos++, WHITE);

        put_char('0' + (second / 10), row, pos++, WHITE);
        put_char('0' + (second % 10), row, pos++, WHITE);

        row++;
    }

    else if (command[0] == 'c' &&
             command[1] == 'd' &&
             command[2] == ' ')
    {
        char *name = command + 3;

        if (pfs_cd(name) == 0)
            print("Directory changed.", row, 0, GREEN);
        else
            print("Directory not found.", row, 0, RED);

        row++;
    }

    else if (equal(command, "pwd"))
    {
        print("Current directory: ", row, 0, CYAN);
        print(pfs_pwd(), row, 18, WHITE);
        row++;
    }

    else if (equal(command, "ls"))
    {
        print("Files:", row, 0, CYAN);
        row++;

        pfs_list(print_file);
    }

    else if (command[0] == 'm' &&
             command[1] == 'k' &&
             command[2] == 'd' &&
             command[3] == 'i' &&
             command[4] == 'r' &&
             command[5] == ' ')
    {
        char *name = command + 6;

        if (pfs_create_dir(name) == 0)
            print("Directory created.", row, 0, GREEN);
        else
            print("Could not create directory.", row, 0, RED);

        row++;
    }

    else if (command[0] == 'w' &&
             command[1] == 'r' &&
             command[2] == 'i' &&
             command[3] == 't' &&
             command[4] == 'e' &&
             command[5] == ' ')
    {
        char *name = command + 6;
        char *content = name;

        while (*content != '\0' && *content != ' ')
            content++;

        if (*content == ' ')
        {
            *content = '\0';
            content++;

            if (pfs_write(name, content) == 0)
                print("File written.", row, 0, GREEN);
            else
                print("Write failed.", row, 0, RED);
        }
        else
        {
            print("Usage: write filename content", row, 0, RED);
        }

        row++;
    }

    else if (command[0] == 'c' &&
             command[1] == 'a' &&
             command[2] == 't' &&
             command[3] == ' ')
    {
        char *name = command + 4;
        const char *content = pfs_read(name);

        if (content)
            print(content, row, 0, WHITE);
        else
            print("File not found.", row, 0, RED);

        row++;
    }

    else if (command[0] == 'r' &&
             command[1] == 'm' &&
             command[2] == ' ')
    {
        char *name = command + 3;

        if (pfs_delete(name) == 0)
            print("File deleted.", row, 0, GREEN);
        else
            print("File not found.", row, 0, RED);

        row++;
    }

    else if (command[0] == 'd' &&
             command[1] == 'e' &&
             command[2] == 'l' &&
             command[3] == 'e' &&
             command[4] == 't' &&
             command[5] == 'e' &&
             command[6] == ' ')
    {
        char *name = command + 7;

        if (pfs_delete(name) == 0)
            print("File deleted.", row, 0, GREEN);
        else
            print("File not found.", row, 0, RED);

        row++;
    }

    else if (equal(command, "disktest"))
    {
        unsigned char write_buffer[512];
        unsigned char read_buffer[512];
        int i;
        int ok = 1;

        for (i = 0; i < 512; i++)
            write_buffer[i] = (unsigned char)(i & 0xFF);

        print("Disk test: writing sector...", row, 0, CYAN);

        if (ata_write_sector(10, write_buffer) != 0)
        {
            print("Disk write FAILED.", row + 1, 0, RED);
            row += 2;
        }
        else
        {
            print("Disk write OK.", row + 1, 0, GREEN);

            if (ata_read_sector(10, read_buffer) != 0)
            {
                print("Disk read FAILED.", row + 2, 0, RED);
                row += 3;
            }
            else
            {
                for (i = 0; i < 512; i++)
                {
                    if (read_buffer[i] != write_buffer[i])
                    {
                        ok = 0;
                        break;
                    }
                }

                if (ok)
                    print("Disk read/write test PASSED.", row + 2, 0, GREEN);
                else
                    print("Disk data mismatch.", row + 2, 0, RED);

                row += 3;
            }
        }
    }

    else if (command[0] == 't' &&
             command[1] == 'o' &&
             command[2] == 'u' &&
             command[3] == 'c' &&
             command[4] == 'h' &&
             command[5] == ' ')
    {
        char *name = command + 6;

        if (*name == '\0')
        {
            print("Usage: touch filename", row, 0, RED);
        }
        else if (pfs_create_file(name) == 0)
        {
            print("File created.", row, 0, GREEN);
        }
        else
        {
            print("Could not create file.", row, 0, RED);
        }

        row++;
    }

    else if (equal(command, "reboot"))
    {
        print("Rebooting SahalaOS...", row, 0, RED);
        reboot();
    }
    else if (equal(command, "clear"))
    {
        clear_screen();

        row = 0;

        print("========================================", row, 0, CYAN);
        row++;

        print("           SAHALA OS v0.2", row, 0, GREEN);
        row++;

        print("========================================", row, 0, CYAN);
        row += 2;

        print("Welcome to SahalaOS!", row, 0, WHITE);
        row += 2;
    }
    else if (command[0] != '\0')
    {
        print("Command not found. Type 'help'.", row, 0, RED);
    }
}

void kernel_main()
{
    char command[64];
    int length = 0;

    timer_init();
    gdt_init();
    interrupts_init();
    pfs_init();
    clear_screen();

    print("========================================", 0, 0, CYAN);
    print("           SAHALA OS v0.2", 1, 0, GREEN);
    print("========================================", 2, 0, CYAN);

    print("Welcome to SahalaOS!", 4, 0, WHITE);
    print("Type 'help' to see available commands.", 5, 0, WHITE);

    row = 7;
    print_prompt();

    while (1)
    {
        while ((inb(0x64) & 1) == 0)
        {
        }

        unsigned char code = inb(0x60);

        if (code & 0x80)
            continue;

        if (code == 0x1C)
        {
            command[length] = '\0';

            execute_command(command);

            length = 0;

            row++;

            if (row >= 24)
            {
                clear_screen();
                row = 0;
            }

            print_prompt();

            continue;
        }

        if (code == 0x0E)
        {
            if (length > 0)
            {
                length--;
                col--;

                put_char(' ', row, col, WHITE);
            }

            continue;
        }

        char c = scancode_to_char(code);

        if (c != 0 && length < 63)
        {
            command[length] = c;
            length++;

            put_char(c, row, col, WHITE);

            col++;

            if (col >= 79)
            {
                row++;
                col = 0;
            }
        }
    }
}
