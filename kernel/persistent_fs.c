#define FS_MAGIC 0x534F5346
#define FS_VERSION 2

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

        for (j = 0; j < 512 && offset + j < sizeof(fs); j++)
            ((unsigned char *)&fs)[offset + j] = buffer[j];
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
}

int pfs_create_file(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            str_equal(fs.files[i].name, name))
            return -1;
    }

    for (i = 0; i < MAX_FILES; i++)
    {
        if (!fs.files[i].used)
        {
            fs.files[i].used = 1;
            fs.files[i].type = TYPE_FILE;

            str_copy(fs.files[i].name, name);
            fs.files[i].content[0] = '\0';

            return fs_save();
        }
    }

    return -2;
}

int pfs_create_dir(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            str_equal(fs.files[i].name, name))
            return -1;
    }

    for (i = 0; i < MAX_FILES; i++)
    {
        if (!fs.files[i].used)
        {
            fs.files[i].used = 1;
            fs.files[i].type = TYPE_DIR;

            str_copy(fs.files[i].name, name);
            fs.files[i].content[0] = '\0';

            return fs_save();
        }
    }

    return -2;
}

const char *pfs_read(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            fs.files[i].type == TYPE_FILE &&
            str_equal(fs.files[i].name, name))
        {
            return fs.files[i].content;
        }
    }

    return 0;
}

int pfs_write(const char *name, const char *content)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            fs.files[i].type == TYPE_FILE &&
            str_equal(fs.files[i].name, name))
        {
            content_copy(fs.files[i].content, content);
            return fs_save();
        }
    }

    if (pfs_create_file(name) == 0)
        return pfs_write(name, content);

    return -1;
}

int pfs_delete(const char *name)
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used &&
            str_equal(fs.files[i].name, name))
        {
            fs.files[i].used = 0;
            fs.files[i].type = 0;
            fs.files[i].name[0] = '\0';
            fs.files[i].content[0] = '\0';

            return fs_save();
        }
    }

    return -1;
}

void pfs_list(void (*callback)(const char *, int))
{
    int i;

    for (i = 0; i < MAX_FILES; i++)
    {
        if (fs.files[i].used)
            callback(fs.files[i].name, fs.files[i].type);
    }
}
