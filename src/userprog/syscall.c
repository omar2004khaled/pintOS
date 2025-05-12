#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"   

static void syscall_handler (struct intr_frame *);

/* prototypes */
static void syscall_halt(void);
static int syscall_wait(tid_t tid);

struct lock file_lock;
/*

static void syscall_exit(int status);
//and the rest of the system calls .....

*/
void
syscall_init (void) 
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void check_address(void *addr)
{
  if (addr == NULL || !is_user_vaddr(addr) || addr < (void *)0x08048000)              //found in threads/vaddr.h
  { 
    thread_exit();   //terminate if invalid
  }
}
static int convert_vaddr(void *vaddr)
{
  check_address(vaddr);
  int* adrr = pagedir_get_page(thread_current()->pagedir, vaddr);
  if (adrr == NULL)
  {
    thread_exit();
  }

  return adrr;
}
// checks every char has valid address
static void check_string(void* str)
{
  char* s = str;
  // for (s; s!=0 ; s = *(char*) convert_vaddr(++s));
  while(true){
    // check if the address is valid
    str = convert_vaddr((void*) s);
    if (*s == '\0') 
      break;
    str++;
  }
  
}

static void check_buffer(void *buffer, int size)
{
  char* it = (char* ) buffer;
  for (int i = 0; i < size; i++)
  {
    check_address((void*) it);
    it++;
  }
}
static void syscall_halt(void) 
{
  printf("(halt) begin\n");
  shutdown_power_off();
}

static int syscall_wait(tid_t tid) 
{
  return process_wait(tid);
}

static void syscall_exit(int status){
  struct thread *cur = thread_current();
  if (cur->parent != NULL) {
    cur->parent->child_exit_status = status;
		printf ("%s: exit(%d)\n", cur->name, status);
  }
  thread_exit();
}

bool create(char *file_name, unsigned initial_size) {
  bool success = filesys_create(file_name, initial_size);
  return success;
  
}


int open(char *file_name) {
  struct file *file = filesys_open(file_name);
  int file_fd; // file discriptor (unique id)
  lock_acquire(&file_lock);
  if (file != NULL) {
    file_fd = procces_add_file(file);
  }
  else {
    file_fd = -1;
  }
  lock_release(&file_lock);

  return file_fd;
}

int procces_add_file(struct file *f){
  struct thread *thread = thread_current();

	struct process_file *process_file = malloc(sizeof(struct process_file)); // new pf to add to the fd table
	if (process_file == NULL) {
		return -1; 
	}

	process_file->fd = thread->fd;
  thread->fd++; 
	list_push_back(&thread->file_list, &process_file->elem); // add process file (opened file info) to fd table
	
  return process_file->fd;

}

static void write(struct intr_frame *f) {
  int fd = *((int *)f->esp + 1);
  char *buffer = (char *)(*((int *)f->esp + 2));
  unsigned size = *((unsigned *)f->esp + 3);
  
  check_address(buffer);
  if (fd == 1) {
    lock_acquire(&file_lock);
    putbuf(buffer, size);
    lock_release(&file_lock);
    f->eax = size;
  } else {
    f->eax = -1; 
  }
}

static void
syscall_handler (struct intr_frame *f) 
{
  //printf ("system call!\n"); 
 // Get the system call number from the stack
 int syscall_number = *(int *) convert_vaddr((void *) f->esp);
//  int syscall_number = *(int *)f->esp;
//  check_address(f->esp);

 // load_arg:
 // int* ptr1 = (int*)f->esp + 1;

 switch (syscall_number) {
   case SYS_HALT:
     syscall_halt();
     break;
     
   case SYS_EXIT:{
    int status = *((int *)f->esp + 1);
    f->eax = status; 
    syscall_exit(status);
     break;
   } 
   case SYS_EXEC:
   {
    char* filename =(char*) (*((int *)f->esp + 1));
    f->eax = process_execute(filename);
      break;
   } 
     case SYS_WAIT: {
      tid_t tid = *(tid_t *)(f->esp + 4);
      f->eax = process_wait(tid);
      break;
    }
    
   case SYS_CREATE:{
    // Handle create system call
    // 2 args const char *file, unsigned initial_size
     char* file = (char* )f->esp + 1;
     unsigned* initial_size_ptr = (unsigned *)f->esp + 2;
     check_address((void *)file);
     check_address((void *)initial_size_ptr);
     check_string(file);

     bool success = create(file, *initial_size_ptr);

     f->eax = success;

     break;
   }
     
   case SYS_REMOVE:
     // Handle remove system call
     break;
     
   case SYS_OPEN:{
     // Handle open system call
     //////// IMPORTANT /////// -> When a thread/process exits, all files it opened must be closed, the list file_list must be freed should help
     char *file = (char *)(f->esp + 1);
     check_string((void*) file);

     int fd = open(file);

     f->eax = fd;
     
     break;
   }

   case SYS_FILESIZE:
     // Handle filesize system call
     break;
     
   case SYS_READ:
     // Handle read system call
     // 3 args
     break;
     
   case SYS_WRITE:
    write(f);
     break;
     
   case SYS_SEEK:
     // Handle seek system call
     // 2 args
     break;
     
   case SYS_TELL:
     // Handle tell system call
     break;
     
   case SYS_CLOSE:
     // Handle close system call
     break;
     
   default:
     printf("Unknown system call: %d\n", syscall_number);
     thread_exit ();
     break;
 }
 
 thread_exit ();
}
