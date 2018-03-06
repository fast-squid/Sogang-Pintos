#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
void process_file_close(struct thread *t);
static bool install_page (void *upage, void *kpage, bool writable,char stack);
#endif /* userprog/process.h */