#define VIDEO_MEMORY 0xB8000

static int row = 4;
static int col = 10;

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

void put_char(char c, int r, int cpos)
{
    char *video = (char *)VIDEO_MEMORY;
    int index = (r * 80 + cpos) * 2;

    video[index] = c;
    video[index + 1] = 0x07;
}

void clear_screen()
{
    char *video = (char *)VIDEO_MEMORY;

    for (int i = 0; i < 80 * 25; i++)
    {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }
}

void print(const char *text, int r, int cpos)
{
    while (*text)
    {
        put_char(*text, r, cpos);
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

void execute_command(char *command)
{
    row++;

    if (equal(command, "help"))
    {
        print("Commands:", row, 0);
        row++;

        print("help  - Show commands", row, 0);
        row++;

        print("clear - Clear screen", row, 0);
        row++;

        print("info  - OS information", row, 0);
    }
    else if (equal(command, "info"))
    {
        print("SahalaOS v0.1", row, 0);
        row++;

        print("32-bit x86 Educational OS", row, 0);
    }
    else if (equal(command, "clear"))
    {
        clear_screen();
        row = 0;

        print("Welcome to SahalaOS!", row, 0);
        row++;

        print("Type help for commands.", row, 0);
        row += 2;
    }
    else if (*command)
    {
        print("Unknown command.", row, 0);
    }
}

void kernel_main()
{
    char command[64];
    int length = 0;

    clear_screen();

    print("Welcome to SahalaOS!", 0, 0);
    print("Keyboard polling driver", 1, 0);
    print("Type help and press Enter.", 2, 0);

    print("SahalaOS> ", 4, 0);

    while (1)
    {
        /*
         * Wait until keyboard data is available.
         */
        while ((inb(0x64) & 1) == 0)
        {
        }

        unsigned char code = inb(0x60);

        /*
         * Ignore key release.
         */
        if (code & 0x80)
            continue;

        /*
         * Enter.
         */
        if (code == 0x1C)
        {
            command[length] = '\0';

            execute_command(command);

            length = 0;

            row++;

            print("SahalaOS> ", row, 0);

            col = 10;

            continue;
        }

        /*
         * Backspace.
         */
        if (code == 0x0E)
        {
            if (length > 0)
            {
                length--;
                col--;

                put_char(' ', row, col);
            }

            continue;
        }

        char c = scancode_to_char(code);

        if (c != 0 && length < 63)
        {
            command[length] = c;
            length++;

            put_char(c, row, col);

            col++;

            if (col >= 79)
            {
                row++;
                col = 10;
            }
        }
    }
}
