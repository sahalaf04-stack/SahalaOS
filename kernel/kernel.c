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
    print("SahalaOS:~$ ", row, 0, GREEN);
    col = 12;
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

        print("echo     - Print text", row, 2, WHITE);
        row++;

        print("clear    - Clear screen", row, 2, WHITE);
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
