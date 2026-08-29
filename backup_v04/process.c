#define MAX_PROCESSES 16

#define PROCESS_UNUSED 0
#define PROCESS_READY  1
#define PROCESS_RUNNING 2
#define PROCESS_SLEEPING 3
#define PROCESS_KILLED 4

struct Process
{
    int pid;
    char name[32];
    int state;
    unsigned int ticks;
};

static struct Process processes[MAX_PROCESSES];

static int next_pid = 1;
static int current_pid = 0;

static void copy_name(char *dest, const char *src)
{
    int i = 0;

    while (src[i] != '\0' && i < 31)
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

void process_init()
{
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].pid = 0;
        processes[i].name[0] = '\0';
        processes[i].state = PROCESS_UNUSED;
        processes[i].ticks = 0;
    }

    /*
     * PID 1 represents the kernel/shell.
     */
    processes[0].pid = 1;
    copy_name(processes[0].name, "kernel");
    processes[0].state = PROCESS_RUNNING;
    processes[0].ticks = 0;

    next_pid = 2;
    current_pid = 1;
}

int process_create(const char *name)
{
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state == PROCESS_UNUSED)
        {
            processes[i].pid = next_pid++;
            copy_name(processes[i].name, name);
            processes[i].state = PROCESS_READY;
            processes[i].ticks = 0;

            return processes[i].pid;
        }
    }

    return -1;
}

int process_kill(int pid)
{
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid &&
            processes[i].state != PROCESS_UNUSED)
        {
            if (pid == 1)
                return -2;

            processes[i].state = PROCESS_KILLED;

            return 0;
        }
    }

    return -1;
}

void process_tick()
{
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state == PROCESS_RUNNING)
            processes[i].ticks++;
    }
}

int process_count()
{
    int count = 0;
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state != PROCESS_UNUSED &&
            processes[i].state != PROCESS_KILLED)
        {
            count++;
        }
    }

    return count;
}

int process_get_pid(int index)
{
    int count = 0;
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state != PROCESS_UNUSED &&
            processes[i].state != PROCESS_KILLED)
        {
            if (count == index)
                return processes[i].pid;

            count++;
        }
    }

    return -1;
}

const char *process_get_name(int index)
{
    int count = 0;
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state != PROCESS_UNUSED &&
            processes[i].state != PROCESS_KILLED)
        {
            if (count == index)
                return processes[i].name;

            count++;
        }
    }

    return 0;
}

const char *process_get_state(int index)
{
    int count = 0;
    int i;

    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state != PROCESS_UNUSED &&
            processes[i].state != PROCESS_KILLED)
        {
            if (count == index)
            {
                if (processes[i].state == PROCESS_RUNNING)
                    return "RUNNING";

                if (processes[i].state == PROCESS_READY)
                    return "READY";

                if (processes[i].state == PROCESS_SLEEPING)
                    return "SLEEPING";
            }

            count++;
        }
    }

    return 0;
}

void scheduler_tick()
{
    int i;
    int found = 0;

    /*
     * Find the next READY process.
     */
    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].state == PROCESS_READY)
        {
            if (found)
            {
                processes[i].state = PROCESS_RUNNING;
                current_pid = processes[i].pid;
                return;
            }

            found = 1;
        }
    }

    /*
     * Keep kernel running if no runnable process exists.
     */
    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == 1 &&
            processes[i].state != PROCESS_KILLED)
        {
            processes[i].state = PROCESS_RUNNING;
            current_pid = 1;
            return;
        }
    }
}

int process_current_pid()
{
    return current_pid;
}
