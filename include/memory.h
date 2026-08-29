#ifndef MEMORY_H
#define MEMORY_H

void memory_init(void);
void *kmalloc(unsigned int size);
void kfree(void *ptr);
unsigned int memory_used(void);
unsigned int memory_free(void);

#endif
