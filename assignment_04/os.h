#ifndef OS_H
#define OS_H

void os_init();
void scheduler();
void loader(const char* prog_file, const char* data_file);
void shell();

#endif