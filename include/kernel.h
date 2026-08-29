#ifndef KERNEL_H
#define KERNEL_H

#define VIDEO_MEMORY 0xB8000

#define WHITE 0x07
#define GREEN 0x0A
#define CYAN  0x0B
#define RED   0x0C

void put_char(char c, int row, int col, unsigned char color);
void print(const char *text, int row, int col, unsigned char color);
void clear_screen(void);
int equal(const char *a, const char *b);

#endif
