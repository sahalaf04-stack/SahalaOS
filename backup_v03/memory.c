#define HEAP_SIZE 65536

static unsigned char heap[HEAP_SIZE];

struct Block
{
    unsigned int offset;
    unsigned int size;
    int used;
};

#define MAX_BLOCKS 128

static struct Block blocks[MAX_BLOCKS];
static int block_count = 0;

void memory_init()
{
    block_count = 1;

    blocks[0].offset = 0;
    blocks[0].size = HEAP_SIZE;
    blocks[0].used = 0;
}

void *kmalloc(unsigned int size)
{
    int i;

    if (size == 0)
        return 0;

    size = (size + 3) & ~3;

    for (i = 0; i < block_count; i++)
    {
        if (!blocks[i].used && blocks[i].size >= size)
        {
            unsigned int remaining = blocks[i].size - size;

            if (remaining > 16 && block_count < MAX_BLOCKS)
            {
                int j;

                for (j = block_count; j > i + 1; j--)
                    blocks[j] = blocks[j - 1];

                blocks[i + 1].offset = blocks[i].offset + size;
                blocks[i + 1].size = remaining;
                blocks[i + 1].used = 0;

                blocks[i].size = size;

                block_count++;
            }

            blocks[i].used = 1;

            return (void *)(heap + blocks[i].offset);
        }
    }

    return 0;
}

void kfree(void *ptr)
{
    int i;

    if (!ptr)
        return;

    for (i = 0; i < block_count; i++)
    {
        if ((void *)(heap + blocks[i].offset) == ptr)
        {
            blocks[i].used = 0;

            if (i + 1 < block_count && !blocks[i + 1].used)
            {
                blocks[i].size += blocks[i + 1].size;

                int j;

                for (j = i + 1; j < block_count - 1; j++)
                    blocks[j] = blocks[j + 1];

                block_count--;
            }

            if (i > 0 && !blocks[i - 1].used)
            {
                blocks[i - 1].size += blocks[i].size;

                int j;

                for (j = i; j < block_count - 1; j++)
                    blocks[j] = blocks[j + 1];

                block_count--;
            }

            return;
        }
    }
}

unsigned int memory_used()
{
    unsigned int total = 0;
    int i;

    for (i = 0; i < block_count; i++)
    {
        if (blocks[i].used)
            total += blocks[i].size;
    }

    return total;
}

unsigned int memory_free()
{
    unsigned int total = 0;
    int i;

    for (i = 0; i < block_count; i++)
    {
        if (!blocks[i].used)
            total += blocks[i].size;
    }

    return total;
}
