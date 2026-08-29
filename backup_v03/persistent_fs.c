#define FS_MAGIC 0x534F5346
#define FS_VERSION 3

#define FS_SECTOR 10

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_CONTENT 256

#define TYPE_FILE 1
#define TYPE_DIR  2

struct DiskFile
{
    char name[MAX_FILENAME];
    char content[MAX_CONTENT];
    char parent[MAX_FILENAME];
    unsigned char used;
    unsigned char type;
};

struct FileSystem
{
    unsigned int magic;
    unsigned int version;
    struct DiskFile files[MAX_FILES];
};

static struct FileSystem fs;
static char current_directory[64] = "/";

extern int ata_read_sector(unsigned int lba, unsigned char *buffer);
extern int ata_write_sector(unsigned int lba, const unsigned char *buffer);

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

static void clear_fs()
{
    int i;

    fs.magic = FS_MAGIC;
    fs.version = FS_VERSION;

    for (i = 0; i < MAX_FILES; i++)
    {
        fs.files[i].used = 0;
        fs.files[i].type = 0;
        fs.files[i].name[0] = '\0';
        fs.files[i].content[0] = '\0';
        fs.files[i].parent[0] = '\0';
    }
}

static int fs_save()
{
    unsigned char buffer[512];
    int offset;
    int j;

    for (offset = 0; offset < sizeof(fs); offset += 512)
    {
        for (j = 0; j < 512; j++)
        {
            if (offset + j < sizeof(fs))
                buffer[j] = ((unsigned char *)&fs)[offset + j];
            else
                buffer[j] = 0;
        }

        if (ata_write_sector(
                FS_SECTOR + (offset / 512),
                buffer) != 0)
        {
            return -1;
        }
    }

    return 0;
}

static int fs_load()
{
    unsigned char buffer[512];
    int offset;
    int j;

    for (offset = 0; offset < sizeof(fs); offset += 512)
    {
        if (ata_read_sector(
                FS_SECTOR + (offset / 512),
                buffer) != 0)
        {
            return -1;
        }

        for (j = 0; j < 512; j++)
        {
            if (offset + j < sizeof(fs))
                ((unsigned char *)&fs)[offset + j] = buffer[j];
        }
    }

    if (fs.magic != FS_MAGIC ||
        fs.version != FS_VERSION)
    {
        clear_fs();
        return fs_save();
    }

    return 0;
}

void pfs_init()
{
    fs_load();

    current_directory[0] = '/';
    current_directory[1] = '\0';
}

static int find_file(const char *name, int type)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            fs.files[i].type == type &&
            str_equal(fs.files[i].name, name) &&
            str_equal(fs.files[i].parent, current_directory))
        {
            return i;
        }
    }

    return -1;
}

int pfs_create_file(const char *name)
{
    int i;

    if (name[0] == '\0')
        return -1;

    if (find_file(name, TYPE_FILE) >= 0 ||
        find_file(name, TYPE_DIR) >= 0)
        return -1;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (!fs.files[i].used)
        {
            fs.files[i].used = 1;
            fs.files[i].type = TYPE_FILE;

            str_copy(fs.files[i].name, name);
            str_copy(fs.files[i].parent, current_directory);

            fs.files[i].content[0] = '\0';

            return fs_save();
        }
    }

    return -2;
}

int pfs_create_dir(const char *name)
{
    int i;

    if (name[0] == '\0')
        return -1;

    if (find_file(name, TYPE_FILE) >= 0 ||
        find_file(name, TYPE_DIR) >= 0)
        return -1;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (!fs.files[i].used)
        {
            fs.files[i].used = 1;
            fs.files[i].type = TYPE_DIR;

            str_copy(fs.files[i].name, name);
            str_copy(fs.files[i].parent, current_directory);

            fs.files[i].content[0] = '\0';

            return fs_save();
        }
    }

    return -2;
}

const char *pfs_read(const char *name)
{
    int i = find_file(name, TYPE_FILE);

    if (i >= 0)
        return fs.files[i].content;

    return 0;
}

int pfs_write(const char *name, const char *content)
{
    int i = find_file(name, TYPE_FILE);

    if (i >= 0)
    {
        content_copy(fs.files[i].content, content);
        return fs_save();
    }

    if (pfs_create_file(name) == 0)
        return pfs_write(name, content);

    return -1;
}

int pfs_delete(const char *name)
{
    int i;

    i = find_file(name, TYPE_FILE);

    if (i < 0)
        i = find_file(name, TYPE_DIR);

    if (i >= 0)
    {
        fs.files[i].used = 0;
        fs.files[i].type = 0;
        fs.files[i].name[0] = '\0';
        fs.files[i].content[0] = '\0';
        fs.files[i].parent[0] = '\0';

        return fs_save();
    }

    return -1;
}

void pfs_list(void (*callback)(const char *, int))
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            str_equal(fs.files[i].parent, current_directory))
        {
            callback(fs.files[i].name, fs.files[i].type);
        }
    }
}

const char *pfs_pwd()
{
    return current_directory;
}

int pfs_cd(const char *name)
{
    int i;
    int len;
    int j;
    char new_path[64];

    if (str_equal(name, "/"))
    {
        current_directory[0] = '/';
        current_directory[1] = '\0';
        return 0;
    }

    if (str_equal(name, "."))
        return 0;

    if (str_equal(name, ".."))
    {
        len = 0;

        while (current_directory[len] != '\0')
            len++;

        if (len <= 1)
            return 0;

        len--;

        while (len > 0 && current_directory[len] != '/')
            len--;

        if (len <= 0)
        {
            current_directory[0] = '/';
            current_directory[1] = '\0';
        }
        else
        {
            current_directory[len] = '\0';
        }

        return 0;
    }

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            fs.files[i].type == TYPE_DIR &&
            str_equal(fs.files[i].name, name) &&
            str_equal(fs.files[i].parent, current_directory))
        {
            len = 0;

            while (current_directory[len] != '\0')
                len++;

            if (len == 1 && current_directory[0] == '/')
                new_path[len++] = '/';
            else
                new_path[len++] = '/';

            j = 0;

            while (name[j] != '\0' && len < 63)
                new_path[len++] = name[j++];

            new_path[len] = '\0';

            str_copy(current_directory, new_path);

            return 0;
        }
    }

    return -1;
}
