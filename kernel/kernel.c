#include "../include/kernel.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/persistent_fs.h"
#include "../include/timer.h"
#include "../include/gdt.h"
#include "../include/interrupts.h"
#include "../include/shell.h"

void kernel_main(void)
{
    timer_init();
    gdt_init();
    memory_init();
    interrupts_init();
    pfs_init();
    process_init();

    shell_init();
    shell_run();
}
