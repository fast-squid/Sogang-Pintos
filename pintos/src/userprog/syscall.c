#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* added */
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h" 
static void syscall_handler (struct intr_frame *);

//static struct lock file_lock;
//static struct semaphore wrt;


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
int
exit(int status){
	if(status<0){
		thread_exit();
	}
	struct thread *cur = thread_current();
	cur->exit_flag = true;
	cur->exit_status = status;
	thread_exit();
	return status;
}

/* TO DO...	
returns new process id */
int
exec(const char* cmd_line){
	if(!is_valid_addr(cmd_line))
		return -1;
	int res = process_execute(cmd_line);
	return res;
}

/* TO DO...
wait for child exit */
int wait(int child_tid){
	return process_wait(child_tid);
}

bool create(const char* file, unsigned initial_size){
	bool res=false;
	if(!file){
		thread_exit();
	}
	
	lock_acquire(&file_lock);
	res = filesys_create(file,initial_size);
	lock_release(&file_lock);
	
	return res;
}
bool remove(const char* file){
	if(!file){
		return false;
	}
	return filesys_remove(file);
}
int open(const char* file){
	if(!file) return -1;

	lock_acquire(&file_lock);
	struct file* fp = filesys_open(file);
	lock_release(&file_lock);
	
	if(!fp) return -1;	
	else{
		struct thread *cur = thread_current();
		int i;
		for(i=3;;i++){
			if(fd_table[i] == NULL){
				fd_table[i] = fp;
				cur->fd_list[i] = 1;
				break;
			}
		}
		return i;
	}

}
int filesize(int fd){
	struct file* fp=fd_table[fd];	
	if(!fp){
		thread_exit();
	}
	
	int res = file_length(fp);
	return res;
}

int
read(int fd, void* buffer, unsigned size){
	int i;
	uint8_t c;
	off_t res=0;
	if(fd<0||fd==1)
		thread_exit();
	
	if(!is_valid_addr(buffer)||!buffer)
		thread_exit();
		
	lock_acquire(&file_lock);			
		
	if(fd == STDIN_FILENO){
		for(i=0;i<size;i++){
			c= input_getc();
			memcpy(buffer+i,&c,1);
			res++;
		}
	}
	else{
		struct file* fp=fd_table[fd];
		res = file_read(fp,buffer,size);
	}
	lock_release(&file_lock);
		
	return res;
}

int
write(int fd, const void*buffer, unsigned size){
	int res=0;
	if(fd<1) thread_exit();
	if(!is_user_vaddr(buffer)||!buffer) thread_exit();
	
	lock_acquire(&file_lock);
	if(fd==STDOUT_FILENO){
		putbuf(buffer, size);
		res = size;
	}
	else{
		struct file* fp=fd_table[fd];
	
		if(!fp) thread_exit();
		res = file_write(fp,buffer,size);
	}
	lock_release(&file_lock);
	return res;
}

void seek(int fd, unsigned position){
	struct file* fp=fd_table[fd];
	
	file_seek(fp,position);
}

unsigned tell(int fd){
	struct file* fp=fd_table[fd];
	
	return file_tell(fp);
}

void close(int fd){
	struct file* fp=fd_table[fd];	
	struct thread* cur = thread_current();
	struct list_elem* ei=NULL;
	struct fd_node* f_n;
	if(!fp){ 
		thread_exit();
	}
	if(cur->fd_list[fd]){
		file_close(fp);
		fd_table[fd] = NULL;
		cur->fd_list[fd] = 0;
	}
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	lock_init(&file_lock);
}
static void
syscall_handler (struct intr_frame *f) 
{
	int syscall_no = *(uint32_t*)f->esp;
	uint32_t *args = (uint32_t*)f->esp;
	/* TO DO...
	check address*/
	if(syscall_no<0 || syscall_no>15)
		thread_exit();
	//hex_dump((uintptr_t)f->esp,f->esp,10,1);
	switch(syscall_no){
		case SYS_HALT:
			halt(); break;
		case SYS_EXIT:
			f->eax = exit((int)args[1]); break;
		case SYS_EXEC:
			f->eax = exec((const char*)args[1]); break;
		case SYS_WAIT:
			f->eax = wait((int)args[1]); break;
		case SYS_CREATE:
			f->eax = create((char*)args[1],(unsigned)args[2]); break;
		case SYS_REMOVE:
			f->eax = remove((char*)args[1]); break;
		case SYS_OPEN:
			f->eax = open((char*)args[1]); break;
		case SYS_FILESIZE:
			f->eax = filesize((int)args[1]); break;
		case SYS_READ:
			f->eax = read((int)args[1],(void*)args[2],(unsigned)args[3]); break;
		case SYS_WRITE:
			f->eax = write((int)args[1],(void*)args[2],(unsigned)args[3]); break;
		case SYS_SEEK:
			seek((int)args[1],(int)args[2]); break;
		case SYS_TELL:
			f->eax = tell((int)args[1]); break;
		case SYS_CLOSE:
			close((int)args[1]); break;
	}
}
