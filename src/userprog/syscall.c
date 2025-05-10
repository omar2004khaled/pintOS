#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
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
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void check_address(void *addr)
{
  if (addr == NULL || !is_user_vaddr(addr) || addr < (void *)0x08048000)              //found in threads/vaddr.h
  { 
    thread_exit();   //terminate if invalid
  }
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
 int syscall_number = convert_vaddr((void *) f->esp);
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
    
     
   case SYS_CREATE:
     // Handle create system call
     // 2 args
     break;
     
   case SYS_REMOVE:
     // Handle remove system call
     break;
     
   case SYS_OPEN:
     // Handle open system call
     break;
     
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
