#ifndef PROCESS_H
#define PROCESS_H

void process_init(void);
int process_create(const char *name);
int process_kill(int pid);
void process_tick(void);
int process_count(void);
int process_get_pid(int index);
const char *process_get_name(int index);
const char *process_get_state(int index);
void scheduler_tick(void);
int process_current_pid(void);

#endif
