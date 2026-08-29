#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_CONTENT 256

struct File
{
    char name[MAX_FILENAME];
    char content[MAX_CONTENT];
    int used;
};

static struct File files[MAX_FILES];

static int str_equal(const char *a, const char *b)
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return 0;
        i++;
    }

    return a[i] == b[i];
}

static void str_copy(char *dest, const char *src)
{
    int i = 0;

    while (src[i] != '\0' && i < MAX_FILENAME - 1)
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

static void content_copy(char *dest, const char *src)
{
    int i = 0;

    while (src[i] != '\0' && i < MAX_CONTENT - 1)
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

void fs_init()
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
        files[i].used = 0;
}

int fs_create(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_equal(files[i].name, name))
            return -1;
    }

    for (i = 0; i < MAX_FILES; i++)
    {
        if (!files[i].used)
        {
            files[i].used = 1;
            str_copy(files[i].name, name);
            files[i].content[0] = '\0';
            return 0;
        }
    }

    return -2;
}

const char *fs_read(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_equal(files[i].name, name))
            return files[i].content;
    }

    return 0;
}

int fs_write(const char *name, const char *content)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_equal(files[i].name, name))
        {
            content_copy(files[i].content, content);
            return 0;
        }
    }

    if (fs_create(name) == 0)
        return fs_write(name, content);

    return -1;
}

void fs_list(void (*print_file)(const char *))
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used)
            print_file(files[i].name);
    }
}


int fs_delete(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (files[i].used && str_equal(files[i].name, name))
        {
            files[i].used = 0;
            files[i].name[0] = '\0';
            files[i].content[0] = '\0';
            return 0;
        }
    }

    return -1;
}
