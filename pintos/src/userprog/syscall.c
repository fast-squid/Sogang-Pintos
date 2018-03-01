#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* added */
#include "devices/shutdown.h"
#include "threads/vaddr.h"
static void syscall_handler (struct intr_frame *);
/* system call */
/*
void halt(void);
void exit(int,int*eax);
int exec(const char*,int* eax);
int wait(tid_t);
bool create(const char* file, unsigned initial_size);
bool remove(const char* file);
int open(const char* file);
int filesize(int fd);
int read(int,void*,unsigned);
int write(int, const void*, unsigned);
*/

/* TO DO... 
check only that a user pointer points below PHYS_BASE
handle "pagefault" by modifying page_fault */
bool
is_valid_addr(const void* uvaddr){
	return is_user_vaddr(uvaddr);
}

/* TO DO...
call shutdown_power off */
void
halt(){
	shutdown_power_off();
}

/* TO DO...
return status to kernel status code 0 or nonezero error code */
void
exit(int status){
	struct thread *cur = thread_current();
	cur->exit_flag = true;
/*	*eax =*/ cur->exit_status = status;
	thread_exit();
}

/* TO DO...	
returns new process id */
int
exec(const char* cmd_line){
	return process_execute(cmd_line);
}

/* TO DO...
wait for child exit */
int wait(tid_t child_tid){
	return process_wait(child_tid);
}

bool create(const char* file, unsigned initial_size){
	return false;
}
bool remove(const char* file){
	return false;
}
int open(const char* file){
	return -1;
}

int filesize(int fd){
	return -1;
}

int
read(int fd, void* buffer, unsigned size){
	int i;
	uint8_t c;
	if(fd == STDIN_FILENO){
		for(i=0;i<size;i++){
			c= input_getc();
			memcpy(buffer+i,&c,1);
		}
	}
	return size;
}

int
write(int fd, const void*buffer, unsigned size){
	if(fd==STDOUT_FILENO){
		putbuf(buffer, size);
		return size;
	}
}

int fibonacci(int num){
	int i;
	int x=0,y=0,z=1;
	if(num<0) thread_exit();
	for(i=0;i<num;i++){
		y= x + z;
		x = z;
		z= y;
	}
	return x;
}

int sum_of_four_integers(int* arr){
	return arr[0]+arr[1]+arr[2]+arr[3];
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
	int syscall_no = *(uint32_t*)f->esp;
	uint32_t *args = (uint32_t*)f->esp;
	int argc[10] = {0,1,1,1,2,1,1,1,3,3};
	int i;
	/* TO DO...
	check address*/
	for(i=0;i<argc[syscall_no]+1;i++){
		if(!is_valid_addr(&args[i])||!&args[i]) 
			thread_exit();	
	}
	switch(syscall_no){
		case SYS_HALT:
			halt(); break;
		case SYS_EXIT:
			exit((int)args[1]/*,&f->eax*/); break;
		case SYS_EXEC:
			f->eax = exec((const char*)args[1]/*,&f->eax*/); break;
		case SYS_WAIT:
			f->eax = wait((tid_t)args[1]); break;
		case SYS_CREATE:
			create((char*)args[1],(unsigned)args[2]); break;
		case SYS_REMOVE:
			remove((char*)args[1]); break;
		case SYS_OPEN:
			open((char*)args[1]); break;
		case SYS_FILESIZE:
			filesize((int)args[1]); break;
		case SYS_READ:
			f->eax = read((int)args[1],(void*)args[2],(unsigned)args[3]); break;
		case SYS_WRITE:
			f->eax = write((int)args[1],(void*)args[2],(unsigned)args[3]); break;
		/*
		case SYS_SEEK:
			seek(); break;
		case SYS_TELL:
			tell(); break;
		case SYS_CLOSE:
			close(); break;
			*/
		case SYS_FIBONACCI:
			f->eax = fibonacci((int)args[1]); break;
		case SYS_SUM_OF_FOUR_INTEGERS:
			f->eax = sum_of_four_integers((int*)args[1]); break;
	}
}
